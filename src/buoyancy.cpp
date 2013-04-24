

#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;


#include "Pong.h"


class App : public SFMLApp, public Pong {
	bool setup       ();
	bool loop        (int w, int h, double t);
	bool cleanup     ();
	void handleInput ();
	void drawScore   (float x, float y, int score);
	void drawRect    (const Rect& rect, const sf::Color& color);
	
	sf::View     view;
	sf::Texture  numbers;
	
	double t_last;
};
bool App::setup ()
{
	if (numbers.loadFromFile("numbers.png")) {
		numbers.setSmooth(false);
	} else return false;
	
	t_last = 0.0;
	startGame();
	
	return true;
}
bool App::loop (int w, int h, double t)
{
	// Grab input
	handleInput();

	// Advance the simulation
	float dt = (t-t_last)/TICKS_PER_FRAME;
	for (int i=0; i<TICKS_PER_FRAME; i++) tick(dt);
	t_last = t;
	
	// Set the view transform
	float screen_aspect = float(w)/h;
	if (screen_aspect>FIELD_ASPECT) view.reset(sf::FloatRect(-screen_aspect/2, 0.5, screen_aspect, -1));
	else                            view.reset(sf::FloatRect(-FIELD_ASPECT/2, 0.5/screen_aspect*FIELD_ASPECT, FIELD_ASPECT, -1.0/screen_aspect*FIELD_ASPECT));
	view.setViewport(sf::FloatRect(0, 0, 1, 1));
	window.setView(view);
	
	sf::Color white(255, 255, 255, 255);
	
	
	// Draw background
	drawRect(Rect(Vec::origin(), FIELD_ASPECT/2, 0.5), sf::Color(20, 20, 20, 255));
	
	// Draw game
	drawRect(field_top, white);
	drawRect(field_bottom, white);
	drawRect(pad1, white);
	drawRect(pad2, white);
	drawRect(ball, white);
	
	// Draw scores
	drawScore(-FIELD_ASPECT/4, 0.25, score1);
	drawScore(FIELD_ASPECT/4, 0.25, score2);
	
	return true;
}
bool App::cleanup () {
	return true;
}
void App::handleInput ()
{
	while (window.pollEvent(event)) {
		switch (event.type) {
			// Window events
			case sf::Event::Closed:
				running = false;
				return;
			case sf::Event::Resized:
				break;
			case sf::Event::GainedFocus:
				break;
			case sf::Event::LostFocus:
				controls.releaseKeys();
				break;
			
			// Mouse + keyboard events
			case sf::Event::MouseMoved:
			//	mouse_x = event.mouseMove.x;
			//	mouse_y = window.getSize().y - event.mouseMove.y;
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				//	case sf::Keyboard::W:			controls->key_w = true;		break;
				//	case sf::Keyboard::A:			controls->key_a = true;		break;
				//	case sf::Keyboard::S:			controls->key_s = true;		break;
				//	case sf::Keyboard::D:			controls->key_d = true;		break;
				//	case sf::Keyboard::Space:		controls->key_space = true;	break;
				//	case sf::Keyboard::LControl:	controls->key_ctrl = true;	break;
					case sf::Keyboard::Escape:		running = false; return;

						
					case sf::Keyboard::W:		controls.pad1_up   = true;	break;
					case sf::Keyboard::S:		controls.pad1_down = true;	break;
					case sf::Keyboard::Up:		controls.pad2_up   = true;	break;
					case sf::Keyboard::Down:	controls.pad2_down = true;	break;
				}
				break;
			case sf::Event::KeyReleased:
				switch (event.key.code) {
				//	case sf::Keyboard::W:		controls->key_w = false;	break;
				//	case sf::Keyboard::A:		controls->key_a = false;	break;
				//	case sf::Keyboard::S:		controls->key_s = false;	break;
				//	case sf::Keyboard::D:		controls->key_d = false;	break;
				//	case sf::Keyboard::Space:	controls->key_space = false;	break;
				//	case sf::Keyboard::LControl:	controls->key_ctrl = false;	break;
					case sf::Keyboard::Escape:	break;
						
					case sf::Keyboard::W:		controls.pad1_up   = false;	break;
					case sf::Keyboard::S:		controls.pad1_down = false;	break;
					case sf::Keyboard::Up:		controls.pad2_up   = false;	break;
					case sf::Keyboard::Down:	controls.pad2_down = false;	break;
				}
				break;
		}
	}
}
void App::drawScore (float x, float y, int score)
{
	sf::Sprite digit;
	digit.setTexture(numbers);
	digit.setOrigin(4.0/2, 5.0/2);
	digit.setScale(LINE_THICKNESS, -LINE_THICKNESS);
	
	int i = 0;
	stringstream s; s << score;
	string digits = s.str();
	for (string::iterator it=digits.begin(); it!=digits.end(); it++) {
		int num = *it - '0';
		if (num<0 || num>9) continue;
		digit.setTextureRect(sf::IntRect(4*num, 0, 4, 5));
		digit.setPosition(x + LINE_THICKNESS*4*i, y);
		window.draw(digit);
		i++;
	}
}
void App::drawRect (const Rect& rect, const sf::Color& color)
{
	sf::RectangleShape sfrect;
	sfrect.setFillColor(color);
	sfrect.setSize(sf::Vector2f(rect.rx*2, rect.ry*2));
	sfrect.setOrigin(sf::Vector2f(rect.rx, rect.ry));
	sfrect.setPosition(rect.p.x, rect.p.y);
	window.draw(sfrect);
}


int main (int argc, char const* argv[]) {
	App app;
	return app.go("PONG");
}
