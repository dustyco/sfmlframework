

#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

#include "sfmlframework.h"


// Distance unit: field-height
// Speed unit:    field-height/second
const static float FIELD_ASPECT   = 4.0/3.0;
const static float LINE_THICKNESS = 1.0/30.0;
const static float BALL_SPEED     = 1.0/1.0;
const static float PADDLE_SPEED   = 1.0/1.0;
const static float PADDLE_WIDTH   = 0.2;
const static float TIME_SPEED     = 1.0;


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
void Pong::startGame ()
{
	resetBall();
	score1 = score2 = 0;
	ball_vx = ball_vy = BALL_SPEED;
	pad1 = pad2 = 0;
	score1 = score2 = 0;
}
void Pong::tick (double dt)
{
	// Scale time
	dt *= TIME_SPEED;
	
	// Delay countdown
	pause -= dt;
	if (pause > 0) return;

	// Move the ball
	ball_px += ball_vx*dt;
	ball_py += ball_vy*dt;
	
	// Ball collision checks
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
	
	// Control paddles
	if (controls.key_w && pad1 + PADDLE_WIDTH/2 < 0.5 - LINE_THICKNESS*2)
		pad1 += PADDLE_SPEED*dt;
	if (controls.key_s && pad1 - PADDLE_WIDTH/2 > -0.5 + LINE_THICKNESS*2)
		pad1 -= PADDLE_SPEED*dt;
		
	if (controls.key_up && pad2 + PADDLE_WIDTH/2 < 0.5 - LINE_THICKNESS*2)
		pad2 += PADDLE_SPEED*dt;
	if (controls.key_down && pad2 - PADDLE_WIDTH/2 > -0.5 + LINE_THICKNESS*2)
		pad2 -= PADDLE_SPEED*dt;
	
}
void Pong::resetBall ()
{
	ball_px = ball_py = 0;
	pause = 1;
}

class App : public SFMLApp, public Pong {
	bool setup ();
	bool loop (int w, int h, double t);
	bool cleanup ();
	void handleInput();
	void drawScore (float x, float y, int score);
	
	sf::Font     font;
	sf::Text     text;
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
	tick(t-t_last);
	t_last = t;
	
	// Set the view transform
	float screen_aspect = float(w)/h;
	if (screen_aspect>FIELD_ASPECT) view.reset(sf::FloatRect(-screen_aspect/2, 0.5, screen_aspect, -1));
	else                            view.reset(sf::FloatRect(-FIELD_ASPECT/2, 0.5/screen_aspect*FIELD_ASPECT, FIELD_ASPECT, -1.0/screen_aspect*FIELD_ASPECT));
	view.setViewport(sf::FloatRect(0, 0, 1, 1));
	window.setView(view);
	
	// Draw dummy box
	{
		sf::RectangleShape rect;
		rect.setFillColor(sf::Color(20, 20, 20, 255));
		rect.setSize(sf::Vector2f(FIELD_ASPECT, 1));
		rect.setOrigin(sf::Vector2f(FIELD_ASPECT/2, 0.5));
		rect.setPosition(0, 0);
		window.draw(rect);
	}
	
	// Draw field
	{
		sf::RectangleShape rect;
		rect.setFillColor(sf::Color(255, 255, 255, 255));
		rect.setSize(sf::Vector2f(FIELD_ASPECT, LINE_THICKNESS));
		rect.setOrigin(sf::Vector2f(FIELD_ASPECT/2, LINE_THICKNESS/2));
		
		rect.setPosition(0, 0.5 - LINE_THICKNESS*1.5);
		window.draw(rect);
		rect.setPosition(0, -0.5 + LINE_THICKNESS*1.5);
		window.draw(rect);
	}
	
	// Draw ball
	{
		sf::RectangleShape rect;
		rect.setFillColor(sf::Color(255, 255, 255, 255));
		rect.setSize(sf::Vector2f(LINE_THICKNESS, LINE_THICKNESS));
		rect.setOrigin(sf::Vector2f(LINE_THICKNESS/2, LINE_THICKNESS/2));
		rect.setPosition(ball_px, ball_py);
		window.draw(rect);
	}
	
	// Draw paddles
	{
		sf::RectangleShape rect;
		rect.setFillColor(sf::Color(255, 255, 255, 255));
		rect.setSize(sf::Vector2f(LINE_THICKNESS, PADDLE_WIDTH));
		rect.setOrigin(sf::Vector2f(LINE_THICKNESS/2, PADDLE_WIDTH/2));
		// Draw paddle 1
		rect.setPosition(-FIELD_ASPECT/2 + LINE_THICKNESS*1.5, pad1);
		window.draw(rect);
		// Draw paddle 2
		rect.setPosition(FIELD_ASPECT/2 - LINE_THICKNESS*1.5, pad2);
		window.draw(rect);
	}
	
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


int main (int argc, char const* argv[]) {
	App app;
	return app.go("PONG");
}
