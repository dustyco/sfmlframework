

#include <cstdio>
#include <sstream>
#include <iostream>
#include <iomanip>
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
	int size = 8;
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
		(const GLvoid*)data
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
	
	// Draw a rectangle
	glBegin(GL_QUADS);	
		glTexCoord2f(0, 1);
		glVertex2f(0, 0);

		glTexCoord2f(1, 1);
		glVertex2f(w, 0);
		
		glTexCoord2f(1, 0);
		glVertex2f(w, h);
		
		glTexCoord2f(0, 0);
		glVertex2f(0, h);
	glEnd();
	
	glUseProgram(0);
	
	glDisable (GL_FRAGMENT_SHADER);
	glEnable (GL_LINE_SMOOTH);
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
