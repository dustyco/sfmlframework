#pragma once


#include "sfmlframework.h"
#include "geometry.h"
using namespace hmath;
typedef vec<2,float> Vec;
typedef rect_aa<float> Rect;


// Distance unit: field-height
// Speed unit:    field-height/second
const static float FIELD_ASPECT    = 4.0/3.0;
const static float LINE_THICKNESS  = 1.0/30.0;
const static float BALL_SPEED      = 1.0/1.0;
const static float PADDLE_SPEED    = 1.0/1.0;
const static float PADDLE_WIDTH    = 0.2;
const static float TIME_SPEED      = 1.5;
const static int   TICKS_PER_FRAME = 10;


class Pong {
public:
	void startGame ();
	void tick (float dt);

	struct Controls {
		Controls () { releaseKeys(); }
		void releaseKeys () { pad1_up = pad1_down = pad2_up = pad2_down = false; }
		bool pad1_up, pad1_down;
		bool pad2_up, pad2_down;
	} controls;
	
	Vec ball_v;
	Rect ball;
	Rect field_top, field_bottom;
	Rect pad1, pad2;
	int serving_player;
	
	float pause;
	int score1, score2;
};
void Pong::startGame ()
{
	// Ball
	ball.p = Vec::origin();
	ball_v = Vec(BALL_SPEED, BALL_SPEED);
	ball.rx = ball.ry = LINE_THICKNESS/2;
	
	// Field
	field_top    = Rect(Vec(0,  0.5-LINE_THICKNESS*1.5), FIELD_ASPECT/2, LINE_THICKNESS/2);
	field_bottom = Rect(Vec(0, -0.5+LINE_THICKNESS*1.5), FIELD_ASPECT/2, LINE_THICKNESS/2);
	
	// Paddles
	pad1 = Rect(Vec(-FIELD_ASPECT/2+LINE_THICKNESS*1.5, 0), LINE_THICKNESS/2, PADDLE_WIDTH/2);
	pad2 = Rect(Vec( FIELD_ASPECT/2-LINE_THICKNESS*1.5, 0), LINE_THICKNESS/2, PADDLE_WIDTH/2);
	
	// Score
	score1 = score2 = 0;
	
	// Delay
	pause = 1;
}
void Pong::tick (float dt)
{
	// Scale time
	dt *= TIME_SPEED;
	
	// Move paddles
	if (controls.pad1_up   && !touching(pad1, field_top   )) pad1.p.y += PADDLE_SPEED*dt;
	if (controls.pad1_down && !touching(pad1, field_bottom)) pad1.p.y -= PADDLE_SPEED*dt;
	if (controls.pad2_up   && !touching(pad2, field_top   )) pad2.p.y += PADDLE_SPEED*dt;
	if (controls.pad2_down && !touching(pad2, field_bottom)) pad2.p.y -= PADDLE_SPEED*dt;
	
	
	// Delay
	pause -= dt;
	if (pause > 0) {
		// Delay - don't simulate ball
		switch (serving_player) {
			case 1:
				ball.p = pad1.p + Vec(LINE_THICKNESS*2, 0);
				break;
			case 2:
				ball.p = pad2.p - Vec(LINE_THICKNESS*2, 0);
				break;
		}
		
	} else {
		// Normal play

		// Move the ball
		ball.p += ball_v*dt;
	
		// Ball-field collisions //////////////////////////////////////////////
		if (touching(ball, field_top) || ball.p.y>field_top.p.y) {
			// Bounce down
			ball_v.y = -fabs(ball_v.y);
		}
		if (touching(ball, field_bottom) || ball.p.y<field_bottom.p.y) {
			// Bounce up
			ball_v.y = fabs(ball_v.y);
		}
	
		// Ball-paddle collisions //////////////////////////////////////////////
		if (touching(ball, pad1) && ball.p.x>pad1.p.x) {
			// Player 1 hit
			ball_v.x = fabs(ball_v.x);
		}
		if (touching(ball, pad2) && ball.p.x<pad2.p.x) {
			// Player 2 hit
			ball_v.x = -fabs(ball_v.x);
		}
	
		// Ball out of bounds //////////////////////////////////////////////////
		if (ball.p.x < -FIELD_ASPECT/2) {
			// Player 1 missed
			score2 += 1;
			serving_player = 1;
			ball_v.x = fabs(ball_v.x);
			pause = 1;
		}
		if (ball.p.x > FIELD_ASPECT/2) {
			// Player 2 missed
			score1 += 1;
			serving_player = 2;
			ball_v.x = -fabs(ball_v.x);
			pause = 1;
		}
	}
	
}
