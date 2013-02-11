

#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

#include "sfmlframework.h"

class App : public SFMLApp {
	bool setup ();
	bool loop (int w, int h, double t);
	bool cleanup ();
	
	sf::Shader        shader;
	sf::RenderStates  states;
	sf::RenderTexture rt;
	
	int frame;
	
	int w_last, h_last;
};
bool App::setup () {
	if (!sf::Shader::isAvailable()) {
		cerr << "Post-effects are not supported" << endl;
		return false;
	}
	
	if (!shader.loadFromFile("shader.glslf", sf::Shader::Fragment)) {
		return false;
	}
	
	w_last = -1;
	
	return true;
}
bool App::loop (int w, int h, double t) {
	
	if (w!=w_last || h!=h_last) {
		rt.create(w, h);
		w_last = w;
		h_last = h;
	}
	
	shader.setParameter("time", float(t));
	shader.setParameter("mouse", float(mouse_x)/w, float(mouse_y)/h);
	shader.setParameter("resolution", w, h);
	states.shader = &shader;
	
	sf::Sprite sprite;
	sprite.setTexture(rt.getTexture());
	window.draw(sprite, states);
	
	return true;
}
bool App::cleanup () {
	return true;
}

int main (int argc, char const* argv[]) {
	App app;
	return app.go("sfmlframework");
}
