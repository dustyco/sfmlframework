

#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

#include "sfmlframework.h"


// Distance unit: field-height
// Speed unit:    field-height/second
const static float FIELD_ASPECT   = 1.5;
const static float LINE_THICKNESS = 1.0/30.0;
const static float BALL_SPEED     = 1.0/2.0;
const static float PADDLE_SPEED   = 1.0/2.0;
const static float PADDLE_WIDTH   = 1.5;


class Pong {
public:
	void startGame ();
	void tick (double dt);
	
	float ball_px, ball_py;
	float ball_vx, ball_vy;
	float pad1, pad2;
	int score1, score2;
};
void Pong::startGame () {
	score1 = score2 = 0;
	ball_px = ball_py = 0;
	ball_vx = ball_vy = BALL_SPEED;
}
void Pong::tick (double dt) {

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
		ball_vx = -fabs(ball_vx);
	} else if (-ball_px>(FIELD_ASPECT*0.5-LINE_THICKNESS*2.5)) {
		// Left paddle
		ball_vx = fabs(ball_vx);
	}
}



class App : public SFMLApp, public Pong {
	bool setup ();
	bool loop (int w, int h, double t);
	bool cleanup ();
	
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
	
	return true;
}
bool App::cleanup () {
	return true;
}


int main (int argc, char const* argv[]) {
	App app;
	return app.go("sfmlframework");
}
