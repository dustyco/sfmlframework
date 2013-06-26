

#pragma once
#include <cmath>
#include <sstream>
#include <iostream>
#include <map>
#include <list>
#include "hmath/linear_algebra.h"
using namespace std;
using namespace hmath;

typedef double Real;
typedef vec<2,Real>  Vec2;
typedef vec<3,Real>  Vec3;
typedef mat<3,Real>  Mat3;
typedef vec<2,float> Vec2f;
typedef list<Vec2f>  Poly2f;

const Real PI = 3.14159265358979323846;
const Real PI2 = PI*2;
const Real G = 6.6742867e-11;
const Real RAD_PER_DEG = PI/180;
const Real SEC_PER_HOUR = 3600;
const Real SEC_PER_DAY = SEC_PER_HOUR*24;
const Real M_PER_KM = 1000;
const Real M_PER_AU = 149597870700;


struct SolarSystem
{
	struct Orbit
	{
		Real period;
		Real eccentricity;
		Real semimajor_axis;
		Real inclination;
		Real longitude_of_ascending_node;
		Real argument_of_periapsis;
		Real mean_anomaly;
		Mat3 local_plane;
	};
	struct Object;
	typedef list<Object> ObjectList;
	struct Object
	{
		string     name;
		string     parent;
		Real       mass;
		bool       has_orbit;
		Orbit      orbit;
		ObjectList children;
		Vec3       position;
		Vec3       velocity;
		
		Object () : has_orbit(false), position(Vec3::ORIGIN), velocity(Vec3::ORIGIN) {}
		void calculateLocalPlane   ();
		void calculateStateVectors (const Object& parent, Real delta_time);
		void update                (const Object& parent, Real delta_time);
	};
	
	Real                delta_time;
	Real                time_scale;
	Object              root_object;
	map<string,Object*> by_name;
	
	bool init           ();
	void tick           (double dt);
	Real timeSinceJ2000 ();
	Real timeSinceJ2000 (const string& iso);
	bool load           (const string& filename);
	void clear          ();
};

