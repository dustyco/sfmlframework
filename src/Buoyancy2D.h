#pragma once
#include <list>
#include "sfmlframework.h"
#include "geometry.h"
using namespace linear_algebra;
using namespace geometry;
typedef vec<2,float> Vec;
typedef mat<2,float> Mat;
typedef std::list<Vec> Poly;


// SI units
const static float GRAVITY         = 6.67384e-11;  // m^2 / (kg*s^2)
const static float WATER_DENSITY   = 1000.0;       // kg / m^3
const static float OBJECT_DENSITY  = 100.0;        // kg / m^3
const static float FIELD_ASPECT    = 16.0/9.0;
const static int   TICKS_PER_FRAME = 1;


struct Buoyancy2D {
	struct Controls {
		Vec mouse;
	} controls;
	
	struct Object {
		Poly  points;
		Vec   pos, posv;
		float rot, rotv;
		float area;
		
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
		}
	};
	
	std::list<Object> objects;
	Poly current, shadow;
	float t;
	
	void init ();
	void tick (float dt);
	void clickPrimary   ();
	void clickSecondary ();
};

void Buoyancy2D::init () {
	// Set up
}

void Buoyancy2D::tick (float dt) {
	shadow.clear();
	if (current.size()>=2) {
		shadow.push_back(*current.begin());
		shadow.push_back(*(--current.end()));
		shadow.push_back(controls.mouse);
	}
	for (std::list<Object>::iterator it=objects.begin(); it!=objects.end(); it++) {
		it->posv.x = std::sin(t*3)/10;
		it->posv.y = std::cos(t*3)/10;
		it->rot += it->rotv*dt;
		it->pos += it->posv*dt;
	}
}

void Buoyancy2D::clickPrimary () {
	cout << "clickPrimary" << controls.mouse << endl;
	current.push_back(controls.mouse);
}

void Buoyancy2D::clickSecondary () {
	cout << "clickSecondary" << controls.mouse << endl;
	if (current.size()>=3) {
		Object o;
		o.points.swap(current);
		o.init();
		objects.push_back(o);
	}
	current.clear();
}
