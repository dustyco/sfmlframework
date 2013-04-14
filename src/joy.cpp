

#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

#include <OIS/OIS.h>

#include "sfmlframework.h"


class App : public SFMLApp, public OIS::JoyStickListener
{
	bool setup ();
	bool loop (int w, int h, double t);
	bool cleanup ();
	
	bool buttonPressed  (const OIS::JoyStickEvent& event, int);
	bool buttonReleased (const OIS::JoyStickEvent& event, int);
	bool axisMoved      (const OIS::JoyStickEvent& event, int);
	
	OIS::JoyStickState js;
	OIS::JoyStick*     joystick;
	OIS::InputManager* input_manager;
};

bool App::buttonPressed (const OIS::JoyStickEvent& event, int) {
	js = event.state;
	return true;
}
bool App::buttonReleased (const OIS::JoyStickEvent& event, int) {
	js = event.state;
	return true;
}
bool App::axisMoved (const OIS::JoyStickEvent& event, int) {
	js = event.state;
	return true;
}

bool App::setup ()
{
	// Set up the input manager
	input_manager = OIS::InputManager::createInputSystem(window.getSystemHandle());
	
	// Joystick
	cout << input_manager->getNumberOfDevices(OIS::OISJoyStick) << " joysticks" << endl;
	OIS::DeviceList::iterator it = input_manager->listFreeDevices().begin();
	OIS::DeviceList::iterator end = input_manager->listFreeDevices().end();
	for (; it!=end; it++) {
		cout << it->first << ", " << it->second << endl;
	}
	
	joystick = static_cast<OIS::JoyStick*>(input_manager->createInputObject(OIS::OISJoyStick, true));
	joystick->setEventCallback(this);
	
	return true;
}
bool App::loop (int w, int h, double t)
{
	joystick->capture();
	
	cout << "============== Event ==============" << endl;
	cout << js.mButtons.size() << " buttons" << endl;
	cout << js.mAxes.size() << " axes" << endl;
	
	return true;
}
bool App::cleanup ()
{
	return true;
}


int main (int argc, char const* argv[]) {
	App app;
	return app.go("sfmlframework");
}
