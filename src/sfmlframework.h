
#include <iostream>
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
using namespace std;


// A simple realtime OpenGL render window template for quick projects
// Usage: inherit, override, and call go()

/*
Copy/paste sample:

#include "sfmlframework.h"
class App : public SFMLApp {
	// Override these
	bool setup ();
	bool loop (int w, int h, double t);
	bool cleanup ();
};
bool App::setup () {
	return true;
}
bool App::loop (int w, int h, double t) {
	return true;
}
bool App::cleanup () {
	return true;
}


-lsfml-system -lsfml-window -lGL
*/

class SFMLApp {
public:
	// Call this to kick things off
	int go (string window_title = "SFMLApp");
	
protected:
	// Override these
	virtual bool setup ();
	virtual bool loop (int w, int h, double t);
	virtual bool cleanup ();
	
	// NON-USER MEMBERS ///////////////////////////////////////////////////////
	bool _setup ();
	bool _input ();
	bool _loop ();
	bool _cleanup ();
	
	sf::RenderWindow    window;
	sf::VideoMode       video_mode;
	sf::Event           event;
	sf::Clock           clock;
	string              window_title;
	bool                running;
	int                 mouse_x, mouse_y;
};




// DEFAULT FUNCTIONS (OVERRIDE THEM IN YOUR SUBCLASS) /////////////////////////
bool SFMLApp::setup () {return true;}
bool SFMLApp::loop (int w, int h, double t)
{
	float x = (float)w/2.0 + cos(t)*(float)w/3.0;
	float y = (float)h/2.0 + sin(t)*(float)h/3.0;
	
	// Clear the buffer
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Set the viewport
	glViewport		(	0,0,w,h		);
	glMatrixMode	(	GL_PROJECTION	);
	glLoadIdentity	();
	glOrtho			(	0,w,0,h,-1,1	);
	glMatrixMode	(	GL_MODELVIEW	);
	glLoadIdentity	();
	glDisable		(	GL_DEPTH_TEST	);
	
	// Draw a rectangle
	int BOXR = 20;
	glColor3f(1,1,1);
	glBegin(GL_QUADS);
		glVertex2f( x-BOXR,y-BOXR );
		glVertex2f( x+BOXR,y-BOXR );
		glVertex2f( x+BOXR,y+BOXR );
		glVertex2f( x-BOXR,y+BOXR );
	glEnd();
	
	glColor3f(.2,.2,.2);
	glBegin(GL_LINES);
		glVertex2f(0,h/2);
		glVertex2f(w,h/2);
		
		glVertex2f(w/2,0);
		glVertex2f(w/2,h);
	glEnd();
	
	return true;
}
bool SFMLApp::cleanup () {return true;}








// NON-USER IMPLEMENTATION ////////////////////////////////////////////////////
//SFMLApp::SFMLApp () {}
//SFMLApp::~SFMLApp () {}

int SFMLApp::go (string window_title) {
	this->window_title = window_title;
	if (_setup()) {
		_loop();
		_cleanup();
		return 0;
	} else return -1;
}
bool SFMLApp::_setup () {
	// Pre-user setup
	video_mode = sf::VideoMode::getDesktopMode();
	if (!video_mode.isValid()) {
		cerr << "Invalid video mode?" << endl;
		return false;
	}
//	window.create( video_mode, window_title, sf::Style::Fullscreen | sf::Style::Close );
	window.create( video_mode, window_title, sf::Style::Resize | sf::Style::Close );
	window.setVerticalSyncEnabled(true);
	window.setMouseCursorVisible(true);
	
	// User setup
	if (!setup()) return false;
	
	// Post-user setup
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	clock.restart();
	
	return true;
}
bool SFMLApp::_input () {
	while (window.pollEvent(event)) {
		switch (event.type) {
			// Window events
			case sf::Event::Closed:
				running = false;
				return true;
			case sf::Event::Resized:
				break;
			case sf::Event::GainedFocus:
				break;
			case sf::Event::LostFocus:
			//	controls->releaseKeys();
				break;
			
			// Mouse + keyboard events
			case sf::Event::MouseMoved:
				mouse_x = event.mouseMove.x;
				mouse_y = window.getSize().y - event.mouseMove.y;
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				//	case sf::Keyboard::W:			controls->key_w = true;		break;
				//	case sf::Keyboard::A:			controls->key_a = true;		break;
				//	case sf::Keyboard::S:			controls->key_s = true;		break;
				//	case sf::Keyboard::D:			controls->key_d = true;		break;
				//	case sf::Keyboard::Space:		controls->key_space = true;	break;
				//	case sf::Keyboard::LControl:	controls->key_ctrl = true;	break;
					case sf::Keyboard::Escape:		running = false; return true;
				/*	case sf::Keyboard::BackSlash:
						window.create(
							video_mode,
							"carputer1",
							sf::Style::Resize | sf::Style::Close
							);
						window.setVerticalSyncEnabled(true);
						window.setMouseCursorVisible(true);
						break; */
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
				}
				break;
		}
	}
	return true;
}
bool SFMLApp::_loop () {
	running = true;
	while (running) {
		// Pre-user loop
		window.clear();
		double time = (double)(clock.getElapsedTime().asMicroseconds())*1e-6;
		_input();
	
		// User loop
		if (!loop(window.getSize().x, window.getSize().y, time)) return false;
	
		// Post-user loop
		window.display();
	}
	return true;
}
bool SFMLApp::_cleanup () {
	// Pre-user cleanup
	
	// User cleanup
	if (!cleanup()) return false;
	
	// Post-user cleanup
	
	return true;
}





