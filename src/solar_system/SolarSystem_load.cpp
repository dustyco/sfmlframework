

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/foreach.hpp>
#include "SolarSystem.h"
using namespace boost;

 
bool SolarSystem::load (const std::string& filename)
{
	clear();
	using namespace boost::property_tree;
	
	try
	{
		ptree pt_file;
		info_parser::read_info(filename, pt_file);
		ptree pt = pt_file.get_child("SolarSystem");
		
		// Add till you can't add no more
		bool added;
		do
		{
			added = false;
			BOOST_FOREACH(ptree::value_type& v, pt) {
				if (v.first.compare("Object")==0) {
					ptree& pt_obj = v.second;
					Object obj;
					
					// Name, parent, mass
					obj.name = pt_obj.get<string>("name", "");
					if (obj.name.empty() || by_name.count(obj.name)==1) continue;  // No name or already added
					obj.parent = pt_obj.get<string>("parent", "");
					if (!obj.parent.empty() && by_name.count(obj.parent)==0) continue;  // Parent hasn't been added yet
					obj.mass = pt_obj.get<Real>("mass", 0);
					
					// Get orbit data
					obj.has_orbit = false;
					if (pt_obj.count("Orbit")>0)
					{
						ptree pt_orbit = pt_obj.get_child("Orbit");
						try
						{
							obj.orbit.period                      = pt_orbit.get<Real>("period")*SEC_PER_DAY;
							obj.orbit.eccentricity                = pt_orbit.get<Real>("eccentricity");
							obj.orbit.semimajor_axis              = pt_orbit.get<Real>("semimajor_axis")*M_PER_KM;
							obj.orbit.inclination                 = pt_orbit.get<Real>("inclination")*RAD_PER_DEG;
							obj.orbit.longitude_of_ascending_node = pt_orbit.get<Real>("longitude_of_ascending_node")*RAD_PER_DEG;
							obj.orbit.argument_of_periapsis       = pt_orbit.get<Real>("argument_of_periapsis")*RAD_PER_DEG;
							obj.orbit.mean_anomaly                = pt_orbit.get<Real>("mean_anomaly")*RAD_PER_DEG;
							obj.calculateLocalPlane();
							obj.has_orbit = true;
						}
						catch (ptree_error& e) { cout << "Warning: Missing orbit element for " << obj.name << ": " << e.what() << endl; }
					}
					
					// Add it
					added = true;
					Object* parent = &root_object;
					if (!obj.parent.empty() && by_name.count(obj.parent)==1) parent = by_name.find(obj.parent)->second;
					parent->children.push_back(obj);
					by_name[obj.name] = &(parent->children.back());
				}
			}
		}
		while (added);
	}
	catch (ptree_error& e)
	{
		cout << "Warning: Failed to load SolarSystem: " << e.what() << endl;
		clear();
		return false;
	}
	
	return true;
}
