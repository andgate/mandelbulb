#include <string>
using namespace std;

#include "MandelbulbViewer.h"
#include "Constants.h"

int main (int argc, char** argv){
	MandelbulbViewer viewer(SCREEN_WIDTH, SCREEN_HEIGHT, DEFAULT_FPS);
	return viewer.run();
}