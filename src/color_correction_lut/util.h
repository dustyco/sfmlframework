

#include <fstream>
using namespace std;


// Allocates buffer (you must delete it)
bool loadFile (const string& path, char** buffer, int* size)
{
	ifstream file(path.c_str(), ifstream::binary);

	// Get size of file
	file.seekg(0, ifstream::end);
	*size = file.tellg();
	file.seekg(0);
	if (*size<1) return false;

	// Allocate buffer
	*buffer = new char[*size];

	// Read to buffer
	if (!file.read(*buffer, *size)) {
		delete *buffer;
		return false;
	}
	return true;
}
