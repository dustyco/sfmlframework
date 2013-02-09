

#include <iostream>
using namespace std;

#include "sfmlframework.h"

class App : public SFMLApp {
	bool setup ();
	bool loop (int w, int h, double t);
	bool cleanup ();
	
	sf::Texture  bridge;
	sf::Sprite   sprite;
};
bool App::setup () {
	if (bridge.loadFromFile("bridge.jpg")) {
		bridge.setSmooth(true);
		sprite.setTexture(bridge);
		sf::Vector2<unsigned int> size = bridge.getSize();
		sprite.setOrigin(size.x/2, size.y/2);
		float scale = 1.0;
		sprite.scale(scale, scale);
	} else return false;
	return true;
}
bool App::loop (int w, int h, double t) {
	sprite.rotate(1);
	sprite.setPosition(w/2, h/2);
	window.draw(sprite);
	return true;
}
bool App::cleanup () {
	return true;
}

int main (int argc, char const* argv[]) {
	App app;
	return app.go("sfmlframework");
}
