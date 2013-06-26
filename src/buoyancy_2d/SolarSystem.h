

#pragma once
#include <cmath>
#include <iostream>
#include <boost/date_time.hpp>
#include "hmath/linear_algebra.h"
using namespace std;
using namespace boost;
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
		Real argument_of_pariapsis;
		Real mean_anomaly;
	};
	struct Object
	{
		Real  mass;
		Orbit orbit;
		
		void calculateStateVectors (const Object& parent, Real delta_time, Vec3& ret_position, Vec3& ret_velocity);
	};
	
	Real delta_time;
	Real time_scale;
	Object sun, earth;
	
	bool init ();
	void tick (double dt);
};


// http://downloads.rene-schwarz.com/download/M001-Keplerian_Orbit_Elements_to_Cartesian_State_Vectors.pdf
void SolarSystem::Object::calculateStateVectors (const Object& parent, Real delta_time, Vec3& ret_position, Vec3& ret_velocity)
{
	// For convenience
	Real e = orbit.eccentricity;
	Real a = orbit.semimajor_axis;
	
	// Mean anomaly at reference + delta_time
	Real M = fmod(orbit.mean_anomaly + PI2*delta_time/orbit.period, PI2);
	cout << "M = " << M << endl;
	
	// Eccentric anomaly
	// http://en.wikipedia.org/wiki/Kepler's_equation#Numerical_approximation_of_inverse_problem
	int i = 0;
	Real E = M;
	while (true)
	{
		Real E_next = E - (E - e*sin(E) - M) / (Real(1) - e*cos(E));
		Real delta = E_next-E;
//		cout << i << ", E = " << E_next << ", delta = " << delta << endl;
		E = E_next;
		++i;
		if (i==10 || fabs(delta) == Real(0)) break;
	}
	
	// True anomaly
	// http://en.wikipedia.org/wiki/True_anomaly#From_the_eccentric_anomaly
	// C++ and PDF: atan2(y, x), wikipedia version is (x, y) so be careful
	Real E_2 = E/2;
	Real v = atan2( sqrt(Real(1)+e)*sin(E_2), sqrt(Real(1)-e)*cos(E_2) );
	
	// Distance to central body
	Real r = a*(Real(1) - e*cos(E));
	cout << "r = " << r/M_PER_AU << " au" << endl;
	
	// Vectors relative to orbital plane
	Real term = sqrt(a*G*parent.mass)/r;
	ret_position(r*cos(v), r*sin(v), 0);
	ret_velocity(term*-sin(E), term*sqrt(Real(1)-e*e)*cos(E), 0);
	
	// Transform to inertial frame
	Mat3 R =
		Mat3(0, 0, -orbit.longitude_of_ascending_node)*
		Mat3(-orbit.inclination, 0, 0)*
		Mat3(0, 0, -orbit.argument_of_pariapsis);
	ret_position = R*ret_position;
	ret_velocity = R*ret_velocity;
	
	cout << "position = " << position/M_PER_AU << " au" << endl;
	cout << "velocity = " << velocity << " m/s" << endl;
}

bool SolarSystem::init ()
{
	delta_time = 0;
	time_scale = 100;
	
	earth.orbit.period                      = Real(365.2563666)*SEC_PER_DAY;
	earth.orbit.eccentricity                = Real(0.01671022);
	earth.orbit.semimajor_axis              = Real(149.597887155765777e6)*M_PER_KM;
	earth.orbit.inclination                 = Real(0.00005)*RAD_PER_DEG;
	earth.orbit.longitude_of_ascending_node = Real(-11.26064)*RAD_PER_DEG;
	earth.orbit.argument_of_pariapsis       = Real(102.94719)*RAD_PER_DEG;
	earth.orbit.mean_anomaly                = Real(357.51716)*RAD_PER_DEG;
	
	sun.mass = 1.9891e30;
}

void SolarSystem::tick (double dt)
{
	delta_time += dt*time_scale;
}
 
Real timeSinceJ2000 ()
{
	static const posix_time::ptime J2000(gregorian::date(2000, 1, 1), posix_time::hours(12));
	auto now = posix_time::from_time_t(time(NULL));
	return (now - J2000).total_seconds(); // / 86400.0 / 36525.0;
}
 
Real timeSinceJ2000 (const std::string& iso)
{
	static const posix_time::ptime J2000(gregorian::date(2000, 1, 1), posix_time::hours(12));
	auto t = posix_time::from_iso_string(iso);
	return (t - J2000).total_seconds(); // / 86400.0 / 36525.0;
}
