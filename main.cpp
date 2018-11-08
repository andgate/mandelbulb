#include <string>
using namespace std;

#include "MandelbulbViewer.h"

int main (int argc, char** argv)
{
	MandelbulbViewer viewer(500, 500);
	return viewer.run();
}