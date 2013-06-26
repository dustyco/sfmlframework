

#include "SolarSystem.h"


void SolarSystem::Object::calculateLocalPlane ()
{
	orbit.local_plane =
		Mat3(0, 0, -orbit.longitude_of_ascending_node)*
		Mat3(-orbit.inclination, 0, 0)*
		Mat3(0, 0, -orbit.argument_of_periapsis);
}

// http://downloads.rene-schwarz.com/download/M001-Keplerian_Orbit_Elements_to_Cartesian_State_Vectors.pdf
void SolarSystem::Object::calculateStateVectors (const Object& parent, Real delta_time)
{
	// For convenience
	Real e = orbit.eccentricity;
	Real a = orbit.semimajor_axis;
	
	// Mean anomaly at reference + delta_time
	Real M = fmod(orbit.mean_anomaly + PI2*delta_time/orbit.period, PI2);
//	cout << endl << "delta_time = " << delta_time/SEC_PER_DAY << " days" << endl;
//	cout << "M = " << M << endl;
	
	// Eccentric anomaly
	// http://en.wikipedia.org/wiki/Kepler's_equation#Numerical_approximation_of_inverse_problem
	int i = 0;
	Real E = M;
//	stringstream ss;
	while (true)
	{
		Real E_next = E - (E - e*sin(E) - M) / (Real(1) - e*cos(E));
		Real delta = E_next-E;
//		ss << i << ", E = " << E_next << ", delta = " << delta << endl;
		E = E_next;
		++i;
//		if (i==10) cout << ss.str() << "newton's method max iterations" << endl;
		if (i==10 || fabs(delta) < Real(1e-14)) break;
	}
//	cout << "E = " << E << endl;
	
	// True anomaly
	// http://en.wikipedia.org/wiki/True_anomaly#From_the_eccentric_anomaly
	// C++ and PDF: atan2(y, x), wikipedia version is (x, y) so be careful
	Real E_2 = E*0.5;
	Real v = atan2( sqrt(Real(1)+e)*sin(E_2), sqrt(Real(1)-e)*cos(E_2) )*2;
//	cout << "v = " << v << endl;
	
	// Distance to central body
	Real r = a*(Real(1) - e*cos(E));
//	cout << "r = " << r/M_PER_AU << " au" << endl;
	
	// Vectors relative to orbital plane
	Real term = sqrt(a*G*parent.mass)/r;
	position = Vec3(r*cos(v), r*sin(v), 0);
	velocity = Vec3(term*-sin(E), term*sqrt(Real(1)-e*e)*cos(E), 0);
	
	// Transform to local plane
	position = orbit.local_plane*position;
	velocity = orbit.local_plane*velocity;
	
//	cout << "position = " << ret_position/M_PER_AU << " au" << endl;
//	cout << "velocity = " << ret_velocity << " m/s" << endl;
}

void SolarSystem::Object::update (const Object& parent, Real delta_time)
{
	if (has_orbit) calculateStateVectors(parent, delta_time);
	else position = velocity = Vec3::ORIGIN;
	position += parent.position;
	velocity += parent.velocity;
	
	for (ObjectList::iterator it=children.begin(); it!=children.end(); ++it)
		it->update(*this, delta_time);
}

bool SolarSystem::init ()
{
	delta_time = 0;
	time_scale = 1000000;
	
	if (!load("SolarSystem.txt")) return false;
	
	return true;
}

void SolarSystem::tick (double dt)
{
	// Advance time
	delta_time += dt*time_scale;
	
	// Recalculate all positions
	for (ObjectList::iterator it=root_object.children.begin(); it!=root_object.children.end(); ++it)
		it->update(root_object, delta_time);
}

void SolarSystem::clear ()
{
	root_object.children.clear();
}
