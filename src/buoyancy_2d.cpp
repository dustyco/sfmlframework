

/*
	Buoyancy2D
	
	
*/


#include <sstream>
#include <iostream>
#include <iomanip>
#include "Buoyancy2D.h"
using namespace std;


struct App : public SFMLApp, public Buoyancy2D {
	double       t_last;
	float        zoom;
	sf::View     view;
	sf::Texture  line_texture, dot_texture;
	sf::Sprite   line_sprite, dot_sprite;
	
	bool setup       ();
	bool loop        (int w, int h, double t);
	bool cleanup     ();
	void handleInput ();
	void drawLine    (const Vec& a, const Vec& b);
	void drawPoly    (const Poly& poly, sf::Color color, bool outline = false, bool points = false);
	void drawPoint   (const Vec& point, sf::Color color);
	void drawObject  (const Object& object, sf::Color color);
	Vec  viewTransformInverse (const Vec& v);
};

bool App::setup ()
{
	t_last = 0.0;
	init();
	zoom = 4;
	
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
	this->t = t;
	float dt = (t-t_last)/TICKS_PER_FRAME;
	for (int i=0; i<TICKS_PER_FRAME; i++) tick(dt);
	t_last = t;
	
	// Set the view transform
	float screen_aspect = float(w)/h;
/*	if (screen_aspect>FIELD_ASPECT) view.reset(sf::FloatRect(-screen_aspect/2, 0.5, screen_aspect, -1));
	else                            view.reset(sf::FloatRect(-FIELD_ASPECT/2, 0.5/screen_aspect*FIELD_ASPECT, FIELD_ASPECT, -1.0/screen_aspect*FIELD_ASPECT));
*/	
	view.reset(sf::FloatRect(-screen_aspect/2*zoom, 0.5*zoom, screen_aspect*zoom, -zoom));
	view.setViewport(sf::FloatRect(0, 0, 1, 1));
	window.setView(view);
	
	sf::Color white(255, 255, 255, 255);
	sf::Color gray(255, 255, 255, 100);
	sf::Color teal(100, 255, 255, 100);
	
/*	int GRID_N = 1;
	for (int x=-GRID_N; x<=GRID_N; x++)
	for (int y=-GRID_N; y<=GRID_N; y++)
		drawPoint(Vec(x,y), white);
*/	
	
	for (std::list<Buoyancy2D::Object>::iterator it=objects.begin(); it!=objects.end(); it++) {
		drawObject(*it, white);
	}
	drawPoly(current, white, true, true);
//	drawPoly(shadow, teal);
	
	if (grab) drawLine(grab_r_abs, mouse);
	
	// Intersection test
	drawLine(water.a, water.b);
	
//	line_sprite.rotate(dt*30);
//	line_sprite.setOrigin(0, 1.5);
//	line_sprite.setScale(1, 1.0/h*zoom*2);
//	line_sprite.setScale(0.02, 1);
//	window.draw(line_sprite);
	
	return true;
}

bool App::cleanup () {
	return true;
}

void App::handleInput ()
{
	// State based controls
	grab = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	sf::Vector2i sf_mouse_pos = sf::Mouse::getPosition(window);
	mouse = viewTransformInverse(Vec(sf_mouse_pos.x, sf_mouse_pos.y));
	
	// Event based controls
	while (window.pollEvent(event)) {
		switch (event.type) {
			case sf::Event::Closed:             running = false; return;
			case sf::Event::LostFocus:          releaseControls(); break;
//			case sf::Event::GainedFocus:        break;
//			case sf::Event::Resized:            break;
//			case sf::Event::MouseMoved:         event.mouseMove.x; break;
			case sf::Event::MouseButtonPressed:
				switch (event.mouseButton.button) {
					case sf::Mouse::Left:       finalizeObject(); break;
					case sf::Mouse::Right:      placePoint(); break;
				}
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
					case sf::Keyboard::Escape:  running = false; return;
					case sf::Keyboard::Return:  finalizeObject(); break;
				}
				break;
		}
	}
}

Vec App::viewTransformInverse (const Vec& v) {
	Vec r;
	sf::Vector2u win = window.getSize();
	float screen_aspect = float(win.y)/win.x;
	r.x = (v.x-win.x/2)/win.y*zoom;
	r.y = (-v.y+win.y/2)/win.y*zoom;
	return r;
}

void App::drawLine (const Vec& a, const Vec& b) {
	line_sprite.setOrigin(0, 1.5);
	line_sprite.setPosition(a.x, a.y);
	line_sprite.setScale(length(b-a), 1.0/window.getSize().y*zoom*2.5);
	line_sprite.setRotation(angle_aa(b-a)*57.29578);
	window.draw(line_sprite);
}

void App::drawPoly (const Poly& poly, sf::Color color, bool outline, bool points) {
	if (poly.size()>=2) {
		sf::ConvexShape sf_shape(poly.size());
		color.a = 150;
		sf_shape.setOutlineColor(color);
		color.a = 50;
		sf_shape.setFillColor(color);
	
		int i = 0;
		Vec last = *(--poly.end());
		for (Poly::const_iterator it = poly.begin(); it!=poly.end(); it++,i++) {
			sf_shape.setPoint(i, sf::Vector2f(it->x, it->y));
			Vec now = *it;
			Vec mid = (now+last)*0.5f;
			Vec dif = (now-last);
			Vec norm(-dif.y, dif.x);
			
			if (outline) drawLine(last, now);
			last = now;
		}
		window.draw(sf_shape);
	}
	if (points) {
		color.a = 250;
		for (Poly::const_iterator it = poly.begin(); it!=poly.end(); it++) {
			drawPoint(*it, color);
		}
	}
}

void App::drawPoint (const Vec& point, sf::Color color) {
	dot_sprite.setPosition(point.x, point.y);
	dot_sprite.setScale(1.0/window.getSize().y*zoom, 1.0/window.getSize().y*zoom);
	window.draw(dot_sprite);
}

void App::drawObject (const Object& object, sf::Color color) {
	Mat rot(object.rot);
	Poly p;
	for (Poly::const_iterator it = object.points.begin(); it!=object.points.end(); it++)
		p.push_back(rot*(*it) + object.pos);
	drawPoly(p, color, true, false);
	drawPoint(object.pos, color);
}

int main (int argc, char const* argv[]) {
	App app;
	return app.go("Buoyancy2D");
}
