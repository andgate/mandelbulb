#ifndef MANDELBULB_VIEWER_H
#define MANDELBULB_VIEWER_H

#include "Engine.h"
#include "CameraController.h"
#include <SFML/Graphics/Shader.hpp>

class MandelbulbViewer
{
public:
	MandelbulbViewer(int windowWidth, int windowHeight);
	~MandelbulbViewer();

	int run();

private:
	Engine *engine;
	CameraController *cam;
	sf::Shader *shader;
	sf::RectangleShape *quad;

	void init();
	void preupdate();
	void update(const float dt);
	void draw();
};

#endif // MANDELBULB_VIEWER_H