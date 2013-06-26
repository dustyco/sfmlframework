

#include <boost/chrono.hpp>
#include <ctime>
#include <iostream>
using namespace std;
using namespace boost;
typedef chrono::system_clock Clock;
typedef Clock::time_point Time;
typedef Clock::duration Duration;


int main (int argc, char const* argv[])
{
	time_t rawtime;
	tm*    timeinfo;
	char   buffer[80];

	time(&rawtime);

	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%I:%M:%S %p %Z", timeinfo);
	cout << buffer << endl;

	timeinfo = gmtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%I:%M:%S %p %Z", timeinfo);
	cout << buffer << endl;
	
	
	
	time_t J2000;
	{
		tm _info;
		_info.tm_sec = 0;
		_info.tm_min = 0;
		_info.tm_hour = 0;
		_info.tm_mday = 1;
		_info.tm_mon = 0;
		_info.tm_year = 100;
		J2000 = mktime(&_info);
		strftime(buffer, sizeof(buffer), "%c %Z", &_info);
		cout << buffer << endl;
	}
	
	cout << endl << "===========================" << endl << endl;
	
	cout << Clock::now() << endl;
	
	return 0;
}
