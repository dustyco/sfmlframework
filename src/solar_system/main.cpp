

#include <vector>
#include <cmath>
#include "sfmlframework.h"
#include "SolarSystem.h"


struct App : public SFMLApp, public SolarSystem
{
	double       t_last;
	Real         cam_zoom;
	Vec3         cam_position;
	sf::Texture  line_texture, dot_texture;
	sf::Sprite   line_sprite, dot_sprite;
	
	bool setup       ();
	bool loop        (int w, int h, double t);
	bool cleanup     ();
	void handleInput ();
	void drawPoint   (const Vec2f& point, sf::Color color);
	void drawLine    (const Vec2f& a, const Vec2f& b);
	void drawPoly    (const Poly2f& poly, sf::Color color, bool outline = false, bool points = false, bool fill = true);
	void drawObject  (Object& object, Object& parent, sf::Color color);
};

bool App::setup ()
{
	t_last = 0.0;
	init();
	cam_zoom = M_PER_AU*2;
	
	if (line_texture.loadFromFile("line.png")) {
		line_texture.setSmooth(true);
		line_sprite.setTexture(line_texture);
//		line_sprite.setTextureRect(sf::IntRect(64*4, 64*5, 64, 64));
		sf::Vector2<unsigned int> size = line_texture.getSize();
//		tiles_sprite.setOrigin(size.x/2, size.y/2);
//		float scale = 1.0;
	} else return false;
	
	if (dot_texture.loadFromFile("dot.png")) {
		dot_texture.setSmooth(true);
		dot_sprite.setTexture(dot_texture);
		sf::Vector2<unsigned int> size = dot_texture.getSize();
		dot_sprite.setOrigin(size.x/2, size.y/2);
	} else return false;
	
	return true;
}

bool App::loop (int w, int h, double t)
{
	// Grab input
	handleInput();

	// Advance the simulation
	const int TICKS_PER_FRAME = 1;
	float dt = (t-t_last)/TICKS_PER_FRAME;
	for (int i=0; i<TICKS_PER_FRAME; i++) tick(dt);
	t_last = t;
	
	// Set the view transform
	float screen_aspect = float(w)/h;
	sf::View view;
	cam_position = by_name["Earth"]->position;
//	view.reset(sf::FloatRect(-screen_aspect/2*cam_zoom, 0.5*cam_zoom, screen_aspect*cam_zoom, -cam_zoom));
	view.setCenter(cam_position.x, cam_position.y);
	view.setSize(screen_aspect*cam_zoom, cam_zoom);
	view.setViewport(sf::FloatRect(0, 0, 1, 1));
	window.setView(view);
	
	
	// Draw all objects
	for (ObjectList::iterator it=root_object.children.begin(); it!=root_object.children.end(); ++it)
		drawObject(*it, root_object, sf::Color::White);
	
	
	return true;
}

bool App::cleanup () {
	return true;
}

void App::handleInput ()
{
	// State based controls
//	grab = sf::Mouse::isButtonPressed(sf::Mouse::Left);
//	sf::Vector2i sf_mouse_pos = sf::Mouse::getPosition(window);
//	mouse = viewTransformInverse(Vec(sf_mouse_pos.x, sf_mouse_pos.y));
	
	// Event based controls
	while (window.pollEvent(event)) {
		switch (event.type) {
			case sf::Event::Closed:             running = false; return;
//			case sf::Event::LostFocus:          releaseControls(); break;
//			case sf::Event::GainedFocus:        break;
//			case sf::Event::Resized:            break;
//			case sf::Event::MouseMoved:         event.mouseMove.x; break;
			case sf::Event::MouseButtonPressed:
				switch (event.mouseButton.button) {
//					case sf::Mouse::Left:       finalizeObject(); break;
//					case sf::Mouse::Right:      placePoint(); break;
				}
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
					case sf::Keyboard::Escape:  running = false; return;
//					case sf::Keyboard::Return:  finalizeObject(); break;
				}
				break;
			case sf::Event::MouseWheelMoved:
				cam_zoom *= pow(0.9, event.mouseWheel.delta);
				cam_zoom = min(max(cam_zoom, Real(40000)), M_PER_AU*50);
				break;
		}
	}
}

void App::drawLine (const Vec2f& a, const Vec2f& b) {
	line_sprite.setOrigin(0, 1.5);
	line_sprite.setPosition(a.x, a.y);
	line_sprite.setScale(length(b-a), cam_zoom/window.getSize().y*2.5);
	line_sprite.setRotation(angle_aa(b-a)*57.29578);
	window.draw(line_sprite);
}

void App::drawPoly (const Poly2f& poly, sf::Color color, bool outline, bool points, bool fill) {
	if (poly.size()>=2) {
		sf::ConvexShape sf_shape(poly.size());
		color.a = 150;
		sf_shape.setOutlineColor(color);
		color.a = 50;
		sf_shape.setFillColor(color);
	
		int i = 0;
		Vec2f last = *(--poly.end());
		for (Poly2f::const_iterator it = poly.begin(); it!=poly.end(); it++,i++) {
			if (fill) sf_shape.setPoint(i, sf::Vector2f(it->x, it->y));
			Vec2f now = *it;
			Vec2f mid = (now+last)*0.5f;
			Vec2f dif = (now-last);
			Vec2f norm(-dif.y, dif.x);
			
			if (outline) drawLine(last, now);
			last = now;
		}
		if (fill) window.draw(sf_shape);
	}
	if (points) {
		color.a = 250;
		for (Poly2f::const_iterator it = poly.begin(); it!=poly.end(); it++) {
			drawPoint(*it, color);
		}
	}
}

void App::drawPoint (const Vec2f& point, sf::Color color) {
	dot_sprite.setPosition(point.x, point.y);
	dot_sprite.setScale(1.0/window.getSize().y*cam_zoom, 1.0/window.getSize().y*cam_zoom);
	window.draw(dot_sprite);
}

void App::drawObject (Object& object, Object& parent, sf::Color color)
{
	
	// Draw the current position
	drawPoint(Vec2f(object.position.x, object.position.y), color);
	
	if (object.has_orbit)
	{
		// Draw the orbit
		Poly2f phases;
		vector<sf::Vertex> lines;
		for (Real phase=0; phase<object.orbit.period; phase+=object.orbit.period/20)
		{
			object.calculateStateVectors(parent, phase);
			object.position += parent.position;
			object.velocity += parent.velocity;
			phases.push_back(Vec2f(object.position.x, object.position.y));
		
			lines.push_back(sf::Vertex(sf::Vector2f(object.position.x, object.position.y)));
		}
		lines.push_back(lines.front());
//		drawPoly(phases, sf::Color(255, 255, 255, 30), true, false, false);
		window.draw(&lines[0], lines.size(), sf::LinesStrip);
	}
	
	// Draw children
	for (ObjectList::iterator it=object.children.begin(); it!=object.children.end(); ++it)
		drawObject(*it, object, sf::Color::White);
	
}


int main (int argc, char const* argv[])
{
	App app;
	return app.go("SolarSystem");
}


