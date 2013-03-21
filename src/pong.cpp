

#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

#include "sfmlframework.h"


// Distance unit: field-height
// Speed unit:    field-height/second
const static float FIELD_ASPECT   = 1.5;
const static float LINE_THICKNESS = 1.0/25.0;
const static float BALL_SPEED     = 1.0/1.0;
const static float PADDLE_SPEED   = 1.0/1.0;
const static float PADDLE_WIDTH   = 0.2;

const static bool x = true;
const static bool o = false;
const static bool NUM[10][20] = {
	{
		// 0
		x, x, x, o,
		x, o, x, o,
		x, o, x, o,
		x, o, x, o,
		x, x, x, o
	}, {
		// 1
		o, x, o, o,
		o, x, o, o,
		o, x, o, o,
		o, x, o, o,
		o, x, o, o
	}, {
		// 2
		x, x, x, o,
		o, o, x, o,
		x, x, x, o,
		x, o, o, o,
		x, x, x, o
	}, {
		// 3
		x, x, x, o,
		o, o, x, o,
		x, x, x, o,
		o, o, x, o,
		x, x, x, o
	}, {
		// 4
		x, o, x, o,
		x, o, x, o,
		x, x, x, o,
		o, o, x, o,
		o, o, x, o
	}, {
		// 5
		x, x, x, o,
		x, o, o, o,
		x, x, x, o,
		o, o, x, o,
		x, x, x, o
	}, {
		// 6
		x, x, x, o,
		x, o, o, o,
		x, x, x, o,
		x, o, x, o,
		x, x, x, o
	}, {
		// 7
		x, x, x, o,
		o, o, x, o,
		o, o, x, o,
		o, o, x, o,
		o, o, x, o
	}, {
		// 8
		x, x, x, o,
		x, o, x, o,
		x, x, x, o,
		x, o, x, o,
		x, x, x, o
	}, {
		// 9
		x, x, x, o,
		x, o, x, o,
		x, x, x, o,
		o, o, x, o,
		o, o, x, o
	}


};


class Pong {
public:
	void startGame ();
	void tick (double dt);
	void resetBall ();

	struct Controls {
		Controls () { releaseKeys(); }
		void releaseKeys () { key_up = key_down = key_w = key_s = false; }
		bool key_up, key_down, key_w, key_s;
	} controls;
	
	float ball_px, ball_py;
	float ball_vx, ball_vy;
	float pad1, pad2;
	float pause;
	int score1, score2;
};
void Pong::startGame () {
	resetBall();
	score1 = score2 = 0;
	ball_vx = ball_vy = BALL_SPEED;
	pad1 = pad2 = 0;
}
void Pong::tick (double dt) {
	// Delay countdown
	pause -= dt;

	if (pause > 0) return;

	// Move the ball
	ball_px += ball_vx*dt;
	ball_py += ball_vy*dt;
	
	// Move the paddles
	
	// Collision check
	if (ball_py>(0.5-LINE_THICKNESS*2.5)) {
		// Top wall
		ball_vy = -fabs(ball_vy);
	} else if (-ball_py>(0.5-LINE_THICKNESS*2.5)) {
		// Bottom wall
		ball_vy = fabs(ball_vy);
	}
	if (ball_px>(FIELD_ASPECT*0.5-LINE_THICKNESS*2.5)) {
		// Right paddle
		if (ball_py < pad2+PADDLE_WIDTH-LINE_THICKNESS/2 && ball_py > pad2-PADDLE_WIDTH+LINE_THICKNESS/2) {
			ball_vx = -fabs(ball_vx);
		} else {
			// Player 1 gains point
			score1 += 1;
			// Reset ball
			resetBall();
		}
	} else if (-ball_px>(FIELD_ASPECT*0.5-LINE_THICKNESS*2.5)) {
		// Left paddle
		if (ball_py < pad1+PADDLE_WIDTH-LINE_THICKNESS/2 && ball_py > pad1-PADDLE_WIDTH+LINE_THICKNESS/2) {
			ball_vx = fabs(ball_vx);
		} else {
			// Player 2 gains point
			score2 += 1;
			// Reset ball
			resetBall();
		}
	}
	// Control paddle 1
	if (controls.key_w && pad1 + PADDLE_WIDTH/2 < 0.5 - LINE_THICKNESS*2) {
		pad1 += PADDLE_SPEED*dt;
	}
	if (controls.key_s && pad1 - PADDLE_WIDTH/2 > -0.5 + LINE_THICKNESS*2) {
		pad1 -= PADDLE_SPEED*dt;
	}
	// Control paddle 2
	if (controls.key_up && pad2 + PADDLE_WIDTH/2 < 0.5 - LINE_THICKNESS*2) {
		pad2 += PADDLE_SPEED*dt;
	}
	if (controls.key_down && pad2 - PADDLE_WIDTH/2 > -0.5 + LINE_THICKNESS*2) {
		pad2 -= PADDLE_SPEED*dt;
	}
	
}
void Pong::resetBall () {
	ball_px = ball_py = 0;
	pause = 1;
}

class App : public SFMLApp, public Pong {
	bool setup ();
	bool loop (int w, int h, double t);
	bool cleanup ();
	void handleInput();
	
	sf::Font     font;
	sf::Text     text;
	
	double t_last;
};
bool App::setup () {
	if (font.loadFromFile("DroidSans-Bold.ttf")) {
		text.setString("Hello, world!");
		text.setFont(font);
		text.setColor(sf::Color(255, 255, 255));
		text.setCharacterSize(20);
	}
	
	t_last = 0.0;
	startGame();
	
	return true;
}
bool App::loop (int w, int h, double t) {
	
	App::handleInput();

	tick(t-t_last);
	t_last = t;
	
	// Drawing scale factor
	float draw_scale;
	float screen_aspect = float(w)/h;
	if (screen_aspect>FIELD_ASPECT) draw_scale = h;
	else                            draw_scale = w/FIELD_ASPECT;
	
	// Debug text        
	stringstream s; s << h;
	text.setString(s.str());
	text.setPosition(0, h-20);
	window.draw(text);
	
	// Draw field
	{
		sf::RectangleShape rect;
		rect.setFillColor(sf::Color(255, 255, 255, 255));
		rect.setSize(sf::Vector2f(draw_scale*FIELD_ASPECT, LINE_THICKNESS*draw_scale));
		
		rect.setPosition(float(w/2) - draw_scale*0.5*FIELD_ASPECT, float(h/2) - draw_scale*(0.5-LINE_THICKNESS));
		window.draw(rect);
		rect.setPosition(float(w/2) - draw_scale*0.5*FIELD_ASPECT, float(h/2) + draw_scale*(0.5-LINE_THICKNESS*2));
		window.draw(rect);
	}
	
	// Draw ball
	{
		sf::RectangleShape rect;
		rect.setFillColor(sf::Color(255, 255, 255, 255));
		rect.setSize(sf::Vector2f(LINE_THICKNESS*draw_scale, LINE_THICKNESS*draw_scale));
		rect.setOrigin(sf::Vector2f(LINE_THICKNESS*draw_scale/2, LINE_THICKNESS*draw_scale/2));
		rect.setPosition(w/2 + draw_scale*ball_px, h/2 - draw_scale*ball_py);
		window.draw(rect);
	}
	
	// Draw paddles
	{
		sf::RectangleShape rect;
		rect.setFillColor(sf::Color(255, 255, 255, 255));
		rect.setSize(sf::Vector2f(LINE_THICKNESS*draw_scale, PADDLE_WIDTH*draw_scale));
		rect.setOrigin(sf::Vector2f(LINE_THICKNESS*draw_scale/2, PADDLE_WIDTH*draw_scale/2));
		// Draw paddle 1
		rect.setPosition(LINE_THICKNESS*draw_scale*3.5, h/2 - draw_scale*pad1);
		window.draw(rect);
		// Draw paddle 2
		rect.setPosition(w - LINE_THICKNESS*draw_scale*3.5, h/2 - draw_scale*pad2);
		window.draw(rect);
	}
	return true;
}
bool App::cleanup () {
	return true;
}
void App::handleInput () {


	
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

					case sf::Keyboard::Up:		controls.key_up = true;		break;
					case sf::Keyboard::Down:	controls.key_down = true;	break;
						
					case sf::Keyboard::W:		controls.key_w = true;		break;
					case sf::Keyboard::S:		controls.key_s = true;	break;
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
						
					case sf::Keyboard::Up:		controls.key_up = false;	break;
					case sf::Keyboard::Down:	controls.key_down = false;	break;
						
					case sf::Keyboard::W:		controls.key_w = false;	break;
					case sf::Keyboard::S:		controls.key_s = false;	break;
				}
				break;
		}
	}
}


int main (int argc, char const* argv[]) {
	App app;
	return app.go("sfmlframework");
}
