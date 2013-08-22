

#include <cstdio>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <GL/glew.h>
#include "GL/gl.h"
#include "sfmlframework.h"
#include "geometry.h"
#include "lut.h"
#include "util.h"
using namespace std;
using namespace hmath;


class App : public SFMLApp
{
	sf::Texture       texture;
	int               frame;
	int               w_last, h_last;
	GLuint            program;
	GLuint            lut_texture;
	GLuint            lut_sampler;
	sf::Texture       line_texture, dot_texture;
	sf::Sprite        line_sprite, dot_sprite;
	
	bool setup       ();
	bool loop        (int w, int h, double t);
	void handleInput ();
	bool cleanup     ();
	void drawLine    (const Vec2f& a, const Vec2f& b, sf::Color color);
	void drawPoint   (const Vec2f& point, sf::Color color);
	
	bool lutInit     ();
};

bool App::setup ()
{
	sf::ContextSettings settings = window.getSettings();
	std::cout << "OpenGL version: " << settings.majorVersion << "." << settings.minorVersion << std::endl;

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
	
	if (!texture.loadFromFile("color_correction_lut.jpg")) {
		return false;
	}
	
	//Initialize GLEW
	GLenum glewError = glewInit();
	if( glewError != GLEW_OK )
	{
		printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
		return false;
	}
	
	// Create shader object
	GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
	
	// Load shader
	char* shader_source;
	int   shader_source_size;
	if (!loadFile("color_correction_lut.glslf", &shader_source, &shader_source_size)) {
		cout << "Error loading shader source" << endl;
		return false;
	}
	glShaderSourceARB(shader, 1, (const char**)&shader_source, &shader_source_size);
	delete shader_source;
	
	// Compile shader
	glCompileShaderARB(shader);
	GLint compiled;
	glGetProgramiv(shader, GL_COMPILE_STATUS, &compiled);
	if (compiled!=GL_TRUE) {
		GLint blen = 0;	
		GLsizei slen = 0;
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH , &blen);
		if (blen > 1) {
			GLchar* compiler_log = (GLchar*)malloc(blen);
			glGetInfoLogARB(shader, blen, &slen, compiler_log);
			printf("compiler_log: %s\n", compiler_log);
			free (compiler_log);
			return false;
		}
	}  
	
	// Link shader
	program = glCreateProgram();
	glAttachShader(program, shader);
	glLinkProgram(program);
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		cout << "Error linking shader" << endl;
		return false;
	}
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "IN_TEX"), 0);
	glUniform1i(glGetUniformLocation(program, "IN_LUT"), 1);
	
	lutInit();
	
	
	w_last = -1;
	
	return true;
}

bool App::lutInit ()
{
	int size = 4;
	
	std::vector<Vec3f> lut;
	lut.push_back(Vec3f( 0, 0, 0 ));
	lut.push_back(Vec3f( 91, 0, 0 ));
	lut.push_back(Vec3f( 170, 0, 0 ));
	lut.push_back(Vec3f( 255, 0, 0 ));
	lut.push_back(Vec3f( 0, 102, 0 ));
	lut.push_back(Vec3f( 89, 98, 0 ));
	lut.push_back(Vec3f( 196, 99, 0 ));
	lut.push_back(Vec3f( 255, 67, 0 ));
	lut.push_back(Vec3f( 0, 195, 0 ));
	lut.push_back(Vec3f( 40, 170, 0 ));
	lut.push_back(Vec3f( 158, 182, 0 ));
	lut.push_back(Vec3f( 255, 160, 0 ));
	lut.push_back(Vec3f( 0, 255, 0 ));
	lut.push_back(Vec3f( 0, 0, 255 ));
	lut.push_back(Vec3f( 94, 255, 0 ));
	lut.push_back(Vec3f( 200, 255, 0 ));
	lut.push_back(Vec3f( 0, 0, 63 ));
	lut.push_back(Vec3f( 97, 0, 64 ));
	lut.push_back(Vec3f( 187, 0, 57 ));
	lut.push_back(Vec3f( 255, 0, 50 ));
	lut.push_back(Vec3f( 0, 102, 80 ));
	lut.push_back(Vec3f( 77, 77, 60 ));
	lut.push_back(Vec3f( 169, 70, 43 ));
	lut.push_back(Vec3f( 255, 50, 32 ));
	lut.push_back(Vec3f( 0, 169, 62 ));
	lut.push_back(Vec3f( 50, 157, 43 ));
	lut.push_back(Vec3f( 160, 181, 35 ));
	lut.push_back(Vec3f( 255, 160, 23 ));
	lut.push_back(Vec3f( 0, 255, 60 ));
	lut.push_back(Vec3f( 0, 255, 54 ));
	lut.push_back(Vec3f( 84, 255, 24 ));
	lut.push_back(Vec3f( 208, 255, 0 ));
	lut.push_back(Vec3f( 0, 0, 133 ));
	lut.push_back(Vec3f( 83, 0, 98 ));
	lut.push_back(Vec3f( 170, 0, 105 ));
	lut.push_back(Vec3f( 255, 0, 140 ));
	lut.push_back(Vec3f( 0, 78, 161 ));
	lut.push_back(Vec3f( 87, 63, 120 ));
	lut.push_back(Vec3f( 170, 51, 104 ));
	lut.push_back(Vec3f( 255, 15, 91 ));
	lut.push_back(Vec3f( 0, 180, 137 ));
	lut.push_back(Vec3f( 46, 162, 117 ));
	lut.push_back(Vec3f( 155, 160, 120 ));
	lut.push_back(Vec3f( 255, 142, 108 ));
	lut.push_back(Vec3f( 0, 255, 137 ));
	lut.push_back(Vec3f( 0, 255, 54 ));
	lut.push_back(Vec3f( 86, 255, 98 ));
	lut.push_back(Vec3f( 217, 255, 83 ));
	lut.push_back(Vec3f( 0, 0, 208 ));
	lut.push_back(Vec3f( 93, 0, 106 ));
	lut.push_back(Vec3f( 168, 0, 153 ));
	lut.push_back(Vec3f( 255, 0, 212 ));
	lut.push_back(Vec3f( 0, 0, 224 ));
	lut.push_back(Vec3f( 100, 0, 205 ));
	lut.push_back(Vec3f( 187, 35, 199 ));
	lut.push_back(Vec3f( 255, 0, 180 ));
	lut.push_back(Vec3f( 0, 153, 255 ));
	lut.push_back(Vec3f( 73, 132, 255 ));
	lut.push_back(Vec3f( 154, 130, 185 ));
	lut.push_back(Vec3f( 255, 128, 186 ));
	lut.push_back(Vec3f( 0, 239, 255 ));
	lut.push_back(Vec3f( 0, 241, 195 ));
	lut.push_back(Vec3f( 83, 255, 200 ));
	lut.push_back(Vec3f( 255, 255, 255 ));
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
/*	lut.push_back(Vec3f(0, 0, 0));
	lut.push_back(Vec3f(0, 0, 63));
	lut.push_back(Vec3f(0, 0, 133));
	lut.push_back(Vec3f(0, 0, 208));
	lut.push_back(Vec3f(0, 102, 0));
	lut.push_back(Vec3f(0, 102, 80));
	lut.push_back(Vec3f(0, 78, 161));
	lut.push_back(Vec3f(0, 0, 224));
	lut.push_back(Vec3f(0, 195, 0));
	lut.push_back(Vec3f(0, 169, 62));
	lut.push_back(Vec3f(0, 180, 137));
	lut.push_back(Vec3f(0, 153, 255));
	lut.push_back(Vec3f(0, 255, 0));
	lut.push_back(Vec3f(0, 255, 60));
	lut.push_back(Vec3f(0, 255, 137));
	lut.push_back(Vec3f(0, 239, 255));
	lut.push_back(Vec3f(91, 0, 0));
	lut.push_back(Vec3f(97, 0, 64));
	lut.push_back(Vec3f(83, 0, 98));
	lut.push_back(Vec3f(93, 0, 106));
	lut.push_back(Vec3f(89, 98, 0));
	lut.push_back(Vec3f(77, 77, 60));
	lut.push_back(Vec3f(87, 63, 120));
	lut.push_back(Vec3f(100, 0, 205));
	lut.push_back(Vec3f(40, 170, 0));
	lut.push_back(Vec3f(50, 157, 43));
	lut.push_back(Vec3f(46, 162, 117));
	lut.push_back(Vec3f(73, 132, 255));
	lut.push_back(Vec3f(0, 0, 255));
	lut.push_back(Vec3f(0, 255, 54));
	lut.push_back(Vec3f(0, 255, 54));
	lut.push_back(Vec3f(0, 241, 195));
	lut.push_back(Vec3f(170, 0, 0));
	lut.push_back(Vec3f(187, 0, 57));
	lut.push_back(Vec3f(170, 0, 105));
	lut.push_back(Vec3f(168, 0, 153));
	lut.push_back(Vec3f(196, 99, 0));
	lut.push_back(Vec3f(169, 70, 43));
	lut.push_back(Vec3f(170, 51, 104));
	lut.push_back(Vec3f(187, 35, 199));
	lut.push_back(Vec3f(158, 182, 0));
	lut.push_back(Vec3f(160, 181, 35));
	lut.push_back(Vec3f(155, 160, 120));
	lut.push_back(Vec3f(154, 130, 185));
	lut.push_back(Vec3f(94, 255, 0));
	lut.push_back(Vec3f(84, 255, 24));
	lut.push_back(Vec3f(86, 255, 98));
	lut.push_back(Vec3f(83, 255, 200));
	lut.push_back(Vec3f(255, 0, 0));
	lut.push_back(Vec3f(255, 0, 50));
	lut.push_back(Vec3f(255, 0, 140));
	lut.push_back(Vec3f(255, 0, 212));
	lut.push_back(Vec3f(255, 67, 0));
	lut.push_back(Vec3f(255, 50, 32));
	lut.push_back(Vec3f(255, 15, 91));
	lut.push_back(Vec3f(255, 0, 180));
	lut.push_back(Vec3f(255, 160, 0));
	lut.push_back(Vec3f(255, 160, 23));
	lut.push_back(Vec3f(255, 142, 108));
	lut.push_back(Vec3f(255, 128, 186));
	lut.push_back(Vec3f(200, 255, 0));
	lut.push_back(Vec3f(208, 255, 0));
	lut.push_back(Vec3f(217, 255, 83));
	lut.push_back(Vec3f(255, 255, 255));
*/

	for (std::vector<Vec3f>::iterator it=lut.begin(); it!=lut.end(); ++it)
	{
		 *it *= 1.0f/255.0f;
//		 cout << *it << endl;
	}
	
	
	float* data = new float[3*size*size*size];
	ColorTransform f;
	makeLut(f, (Vec3f*)data, size);
	
	
	glGenTextures(1, &lut_texture);
	glBindTexture(GL_TEXTURE_3D, lut_texture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage3D(
		GL_TEXTURE_3D, 0, GL_RGB,
		size, size, size,
		0,
		GL_RGB, GL_FLOAT,
		(const GLvoid*)(&lut[0])
	);
	delete data;
	
	return true;
}

bool App::loop (int w, int h, double t)
{
	handleInput();
	
	
	// Set the view
	glMatrixMode	(GL_MODELVIEW);
	glLoadIdentity	();
	
	glMatrixMode	(GL_TEXTURE);
	glLoadIdentity	();
	
	glMatrixMode	(GL_PROJECTION);
	glLoadIdentity	();
	glOrtho			(0, w, 0, h, -1, 1);
	
	glViewport		(0, 0, w, h);
	glDisable		(GL_DEPTH_TEST);
	
	
	glUseProgram(program);
	
	glActiveTexture(GL_TEXTURE0);
	sf::Texture::bind(&texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, lut_texture);
	
	float aspect = float(texture.getSize().x)/texture.getSize().y;
	float window_aspect = float(w)/h;
	
	// Draw a rectangle
	glBegin(GL_QUADS);	
		glTexCoord2f(0, 1);
		glVertex2f(0, 0);

		glTexCoord2f(1, 1);
		glVertex2f(aspect*w/window_aspect, 0);
		
		glTexCoord2f(1, 0);
		glVertex2f(aspect*w/window_aspect, h);
		
		glTexCoord2f(0, 0);
		glVertex2f(0, h);
	glEnd();
	
	glUseProgram(0);
	
	glDisable (GL_FRAGMENT_SHADER);
/*	glEnable (GL_LINE_SMOOTH);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth (2);
	
	glColor3f (1.0, 1.0, 1.0);
	glBegin (GL_LINES);
		glVertex2f (0.3*w, 0.1*h);
		glVertex2f (0.9*w, 0.8*h);
	glEnd ();
	
	glActiveTexture(GL_TEXTURE0);
	window.pushGLStates();
	
	{
		sf::View view;
		float screen_aspect = float(w)/float(h);
		view.reset(sf::FloatRect(0, h, w, -h));
		view.setViewport(sf::FloatRect(0, 0, 1, 1));
		window.setView(view);
	}
	Vec2f scale(w, h);
	drawLine(Vec2f(0.1, 0.1)*scale, Vec2f(0.5, 0.7)*scale, sf::Color(255, 100, 0, 255));
	drawLine(Vec2f(0.5, 0.7)*scale, Vec2f(0.9, 0.9)*scale, sf::Color(255, 100, 0, 255));
*/	
	
	window.popGLStates();
	
	return true;
}

void App::handleInput ()
{
	// State based controls
	sf::Vector2i sf_mouse_pos = sf::Mouse::getPosition(window);
	mouse_x = sf_mouse_pos.x;
	mouse_y = sf_mouse_pos.y;
	
	// Event based controls
	while (window.pollEvent(event)) {
		switch (event.type) {
			case sf::Event::Closed:             running = false; return;
//			case sf::Event::LostFocus:          releaseControls(); break;
//			case sf::Event::GainedFocus:        break;
//			case sf::Event::Resized:            break;
//			case sf::Event::MouseMoved:         event.mouseMove.x; break;
			case sf::Event::MouseButtonPressed:
				switch (event.mouseButton.button) {
//					case sf::Mouse::Left:       finalizeObject(); break;
//					case sf::Mouse::Right:      placePoint(); break;
				}
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
					case sf::Keyboard::Escape:  running = false; return;
				}
				break;
		}
	}
}

bool App::cleanup ()
{
	return true;
}

void App::drawPoint (const Vec2f& point, sf::Color color = sf::Color::White) {
	dot_sprite.setColor(color);
	dot_sprite.setPosition(point.x, point.y);
	dot_sprite.setScale(1.0/window.getSize().y, 1.0/window.getSize().y);
	window.draw(dot_sprite);
}

void App::drawLine (const Vec2f& a, const Vec2f& b, sf::Color color = sf::Color::White) {
	line_sprite.setColor(color);
	line_sprite.setOrigin(0, 1.5);
	line_sprite.setPosition(a.x, a.y);
	line_sprite.setScale(length(b-a), 3);
	line_sprite.setRotation(angle_aa(b-a)*57.29578);
	window.draw(line_sprite);
}

int main (int argc, char const* argv[])
{
	App app;
	return app.go("sfmlframework");
}
