

#include <boost/date_time.hpp>
#include "SolarSystem.h"
using namespace boost;

 
Real SolarSystem::timeSinceJ2000 ()
{
	static const posix_time::ptime J2000(gregorian::date(2000, 1, 1), posix_time::hours(12));
	return (posix_time::from_time_t(time(NULL)) - J2000).total_seconds(); // / 86400.0 / 36525.0;
}
 
Real SolarSystem::timeSinceJ2000 (const std::string& iso)
{
	static const posix_time::ptime J2000(gregorian::date(2000, 1, 1), posix_time::hours(12));
	return (posix_time::from_iso_string(iso) - J2000).total_seconds(); // / 86400.0 / 36525.0;
}
