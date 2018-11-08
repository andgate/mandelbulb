#include "MandelbulbViewer.h"

#include <iostream>
#include <cstdlib>

MandelbulbViewer::MandelbulbViewer(int windowWidth, int windowHeight)
	: engine(nullptr)
	, firstPerson(new FirstPerson())
	, shader(new sf::Shader())
	, quad(nullptr)
{
	this->engine = new Engine(windowWidth, windowHeight, "Mandelbulb Viewer");
}

MandelbulbViewer::~MandelbulbViewer()
{
	if(engine != nullptr) delete engine;
	if(firstPerson != nullptr) delete firstPerson;
	if(shader != nullptr) delete shader;
}

int MandelbulbViewer::run()
{
	engine->setInitFunc(std::bind(&MandelbulbViewer::init, this));
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

	engine->registerInputListener(firstPerson);
}

void MandelbulbViewer::update(const float dt)
{
	// this is when the camera needs to be passed to the shader
	sf::Shader::bind(shader);
	
	const float* camera_combined = firstPerson->getCamera()->getMatrix();
	shader->setUniform("camera_combined", (sf::Glsl::Mat4)camera_combined);

	sf::Vector3f camera_position = firstPerson->getCamera()->position.asSFML();
	shader->setUniform("camera_position", (sf::Glsl::Vec3)camera_position);

	sf::Vector3f camera_direction = firstPerson->getCamera()->direction.asSFML();
	shader->setUniform("camera_direction", (sf::Glsl::Vec3)camera_direction);

	sf::Vector3f camera_up = firstPerson->getCamera()->up.asSFML();
	shader->setUniform("camera_up", (sf::Glsl::Vec3)camera_up);


	sf::Shader::bind(NULL);
}

void MandelbulbViewer::draw()
{
	engine->getWindow()->draw(*quad, shader);
}