#include "MandelbulbViewer.h"

#include "Constants.h"

#include <iostream>
#include <cstdlib>

MandelbulbViewer::MandelbulbViewer(int windowWidth, int windowHeight)
	: engine(nullptr)
	, cam(new CameraController((float)windowWidth, (float)windowHeight))
	, shader(new sf::Shader())
	, quad(nullptr)
{
	this->engine = new Engine(windowWidth, windowHeight, "Mandelbulb Viewer");
}

MandelbulbViewer::~MandelbulbViewer()
{
	if(engine != nullptr) delete engine;
	if(cam != nullptr) delete cam;
	if(shader != nullptr) delete shader;
}

int MandelbulbViewer::run()
{
	engine->setInitFunc(std::bind(&MandelbulbViewer::init, this));
	engine->setPreupdateFunc(std::bind(&MandelbulbViewer::preupdate, this));
	engine->setUpdateFunc(std::bind(&MandelbulbViewer::update, this, std::placeholders::_1));
	engine->setDrawFunc(std::bind(&MandelbulbViewer::draw, this));

	return engine->run();
}

void MandelbulbViewer::init()
{
	if (!sf::Shader::isAvailable())
	{
		std::cout << "Shading not supported!" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (!shader->loadFromFile("mandelbulb.vert", "mandelbulb.frag"))
	{
		// some error occurred
 		std::cout << "Unable to load shaders." << std::endl;
		exit(EXIT_FAILURE);
	}

	float screenWidth = (float)engine->getWindow()->getSize().x;
	float screenHeight = (float)engine->getWindow()->getSize().y;
	quad = new sf::RectangleShape(sf::Vector2f(engine->getWindow()->getSize()));

	sf::Shader::bind(shader);
	sf::Transform viewMatrix = engine->getWindow()->getView().getTransform();
	shader->setUniform("projViewMatrix", (sf::Glsl::Mat4)viewMatrix.getMatrix());
	shader->setUniform("screen_width", screenWidth);
	shader->setUniform("screen_height", screenHeight);
	sf::Shader::bind(NULL);

	engine->registerInputListener(cam);
}

void MandelbulbViewer::preupdate()
{

}

void MandelbulbViewer::update(const float dt)
{
	// this is when the camera needs to be passed to the shader
	sf::Shader::bind(shader);

	sf::Vector3f camera_position = cam->camera->position.asSFML();
	shader->setUniform("camera_position", (sf::Glsl::Vec3)camera_position);

	sf::Vector3f camera_direction = cam->camera->direction.asSFML();
	shader->setUniform("camera_direction", (sf::Glsl::Vec3)camera_direction);

	sf::Vector3f camera_up = cam->camera->up.asSFML();
	shader->setUniform("camera_up", (sf::Glsl::Vec3)camera_up);

	float scale = cam->camera->scale();
	shader->setUniform("scale", scale);

	float screenWidth = (float)engine->getWindow()->getSize().x;
	float screenHeight = (float)engine->getWindow()->getSize().y;
	shader->setUniform("aspect", screenWidth/screenHeight);
	shader->setUniform("fov", FOV);
	
	shader->setUniform("screen_width", screenWidth);
	shader->setUniform("screen_height", screenHeight);
	
	shader->setUniform("epsilon_factor", EPSILON_FACTOR);
	shader->setUniform("max_dist", MAX_DIST);
	shader->setUniform("max_bailout", MAX_BAILOUT);
	shader->setUniform("max_iter", MAX_ITER);
	shader->setUniform("max_steps", MAX_STEPS);
	shader->setUniform("power", POWER);

	sf::Shader::bind(NULL);
}

void MandelbulbViewer::draw()
{
	engine->getWindow()->draw(*quad, shader);
}