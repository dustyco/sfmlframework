#pragma once
#include <list>
#include "sfmlframework.h"
#include "geometry.h"
using namespace hmath;
typedef vec<2,float> Vec;
typedef mat<2,float> Mat;
typedef line<2,float> Line;
typedef std::list<Vec> Poly;


// SI units
const static float GRAVITY         = 6.67384e-11;  // m^2 / kg / s^2
const static float EARTH_RADIUS    = 6371e3;       // m
const static float EARTH_MASS      = 5.972e24;     // kg
const static float EARTH_ACCEL     = 9.80665;      // m / s^2

const static float GRAB_STRENGTH   = 10.0;       // kg / m^3

const static float WATER_DENSITY   = 1000.0;       // kg / m^3
const static float OBJECT_DENSITY  = 100.0;        // kg / m^3
const static int   TICKS_PER_FRAME = 1;


struct Buoyancy2D {
	
	struct Object {
		Poly  points;
		Vec   pos, posv;
		float rot, rotv;
		float area, moment;
		
		void init () {
			Poly::iterator first = points.begin();
			Poly::iterator last = --points.end();
			float x0, y0, x1, y1, a;
			area = 0;
			Vec centroid = Vec::origin();
			for (Poly::iterator it=first; it!=last;) {
				x0 = it->x;
				y0 = it->y;
				++it;
				x1 = it->x;
				y1 = it->y;
				a = x0*y1 - x1*y0;
				area += a;
				centroid.x += (x0 + x1)*a;
				centroid.y += (y0 + y1)*a;
			}
			// Last and first pair
			x0 = last->x;
			y0 = last->y;
			x1 = first->x;
			y1 = first->y;
			a = x0*y1 - x1*y0;
			area += a;
			centroid.x += (x0 + x1)*a;
			centroid.y += (y0 + y1)*a;
			// Finalize
			area *= 0.5f;
			centroid /= (6.0f*area);
			// Flip if it's not counterclockwise
			if (area<0) {
				area = -area;
				points.reverse();
			}
			// Reposition vertices
			for (Poly::iterator it=points.begin(); it!=points.end(); ++it)
				*it -= centroid;
			pos = centroid;
			// Default other states
			posv = Vec(0.01, 0.01);
			rot = 0;
			rotv = 1;
			moment = area*2/3;
		}
	};
	
	float t;
	Vec   mouse;
	bool  grab, grab_last;
	Vec   grab_r, grab_r_abs;
	Line  water;
	Poly  current, shadow;
	std::list<Object> objects;
	
	void init            ();
	void tick            (float dt);
	void placePoint      ();
	void finalizeObject  ();
	void releaseControls ();
};

void Buoyancy2D::init () {
	releaseControls();
	water = Line(Vec(-10, 0), Vec(10, 0));
}

void Buoyancy2D::tick (float dt) {
	// Update drawing shadow
	shadow.clear();
	if (current.size()>=2) {
		shadow.push_back(*current.begin());
		shadow.push_back(*(--current.end()));
		shadow.push_back(mouse);
	}
	// Each object
	for (std::list<Object>::iterator it_obj=objects.begin(); it_obj!=objects.end(); ++it_obj) {
		Object& obj = *it_obj;
		Mat rot(obj.rot);
		
		// Apply gravity
		obj.posv.y -= EARTH_ACCEL*dt;
		
		// Apply buoyancy
		Vec pt_last = rot*obj.points.back() + obj.pos;
		bool below_last = is_left(pt_last, water);
		for (Poly::iterator it_pt=obj.points.begin(); it_pt!=obj.points.end(); ++it_pt) {
			Vec pt = rot*(*it_pt) + obj.pos;
			bool below = is_left(pt, water);
			Vec force, r;
			
			// Test the nature of the segment
			bool apply_buoyancy = true;
			if (below!=below_last) {
				// Crosses the water line
				// Find the intersect and process only the submerged part
				Vec transition = intersect(Line(pt_last, pt), water);
				if (below_last) pt = transition;
				else pt_last = transition;
			} else if (!below) {
				// Entirely in air
				apply_buoyancy = false;
			}
			
			// Apply it if appropriate
			if (apply_buoyancy) {
				Vec center = (pt_last + pt)*0.5f;
				float pressure = distance(center, water)*50;
				force = normal(Line(pt_last, pt))*pressure;
				r = center-obj.pos;
				
				obj.rotv += cross(r, force)/obj.moment*dt;
				obj.posv += force/obj.area*dt;
			}
			
			pt_last = rot*(*it_pt) + obj.pos;
			below_last = below;
		}
		
		// Quick and incorrect drag
		obj.rotv *= 0.99f;
		obj.posv *= 0.99f;
		
		// Integrate movement
		obj.rot += obj.rotv*dt;
		obj.pos += obj.posv*dt;
	}
	
	{
		Object& obj = objects.front();
		if (grab && !grab_last) {
			// Begin grab
			grab_r = Mat(-obj.rot)*(mouse-obj.pos);
		}
		if (grab) {
			// Apply grab
			grab_r_abs = obj.pos + Mat(obj.rot)*grab_r;
			Vec grab_force = mouse-grab_r_abs;
			float torque = cross(grab_r_abs-obj.pos, grab_force);
			obj.posv += grab_force/obj.area*dt*GRAB_STRENGTH;
			obj.rotv += torque/obj.moment*dt*GRAB_STRENGTH;
		}
		grab_last = grab;
	}
}

void Buoyancy2D::placePoint () {
	current.push_back(mouse);
}

void Buoyancy2D::finalizeObject () {
	if (current.size()>=3) {
		Object o;
		o.points.swap(current);
		o.init();
		objects.push_back(o);
	}
	current.clear();
}

void Buoyancy2D::releaseControls () {
	grab = false;
}
