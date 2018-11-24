#include <string>
using namespace std;

#include "MandelbulbViewer.h"

int main (int argc, char** argv)
{
	MandelbulbViewer viewer(800, 600);
	//MandelbulbViewer viewer(1024, 768);
	return viewer.run();
}