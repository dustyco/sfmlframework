

#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

#include "sfmlframework.h"

class App : public SFMLApp {
	bool setup ();
	bool loop (int w, int h, double t);
	bool cleanup ();
	
	sf::Shader       shader;
	sf::RenderStates states;
	
	sf::Texture  tiles;
	sf::Texture  bridge;
	sf::Sprite   sprite;
	sf::Sprite   tiles_sprite;
	
	sf::Font     font;
	sf::Text     text;
	
	int frame;
};
bool App::setup () {
	if (!sf::Shader::isAvailable()) {
		cerr << "Post-effects are not supported" << endl;
		return false;
	}
	
	if (!shader.loadFromFile("test.sfx", sf::Shader::Fragment)) {
		return false;
	}
	
	if (tiles.loadFromFile("tiles1.png")) {
		tiles.setSmooth(true);
		tiles_sprite.setTexture(tiles);
		tiles_sprite.setTextureRect(sf::IntRect(64*4, 64*5, 64, 64));
		sf::Vector2<unsigned int> size = tiles.getSize();
//		tiles_sprite.setOrigin(size.x/2, size.y/2);
		float scale = 1.0;
		tiles_sprite.scale(scale, scale);
	} else return false;
	
	if (bridge.loadFromFile("bridge.jpg")) {
		bridge.setSmooth(true);
		sprite.setTexture(bridge);
		sf::Vector2<unsigned int> size = bridge.getSize();
		sprite.setOrigin(size.x/2, size.y/2);
		float scale = 1.0;
		sprite.scale(scale, scale);
	} else return false;
	
	if (font.loadFromFile("DroidSans-Bold.ttf")) {
		text.setString("Hello, world!");
		text.setFont(font);
		text.setColor(sf::Color(255, 255, 255));
		text.setCharacterSize(50);
	}
	frame = 0;
	return true;
}
bool App::loop (int w, int h, double t) {
	sprite.rotate(0.1);
	sprite.setPosition(w/2, h/2);
	tiles_sprite.setPosition(w/2, h/2);
	
	stringstream s; s << setw(2) << setfill('0') << frame/60 << ":" << setw(2) << setfill('0') << frame%60;
	text.setString(s.str());
	frame++;
	
	window.draw(sprite);
	
	srand(0);
	for (int y=0; y<10; y++)
	for (int x=0; x<10; x++) {
		int col = 64;
		
		unsigned char depth = rand()/(RAND_MAX/256);
		
		tiles_sprite.setPosition(w/2 + col/2*x - col/2*y, h/2 + col/4*y + col/4*x + depth/64);
//		states.shader = &shader;
		unsigned char color = 255-depth/4;
		tiles_sprite.setColor(sf::Color(color, color, color, 255));
		sf::IntRect rect = (rand()%2==0)?sf::IntRect(64*5, 64*5, 64, 64):sf::IntRect(64*4, 64*5, 64, 64);
		tiles_sprite.setTextureRect(rect);
		window.draw(tiles_sprite, states);
	}
	window.draw(text);
	return true;
}
bool App::cleanup () {
	return true;
}

int main (int argc, char const* argv[]) {
	App app;
	return app.go("sfmlframework");
}
