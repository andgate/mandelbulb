#include "MandelbulbViewer.h"

#include "Constants.h"

#include <iostream>
#include <sstream>
#include <cstdlib>
using namespace std;

MandelbulbViewer::MandelbulbViewer(const int &windowWidth, const int &windowHeight, const int& max_fps)
	: engine(nullptr)
	, viewer(new MandelbulbViewer::ViewerInputListener())
	, cam(new CameraController((float)windowWidth, (float)windowHeight))
	, shader(new sf::Shader())
	, quad(nullptr)
	, infoFont()
	, info()
	, infoBg()
{
	this->engine = new Engine("Mandelbulb Viewer", windowWidth, windowHeight, max_fps);
}

MandelbulbViewer::~MandelbulbViewer()
{
	if(engine != nullptr) delete engine;
	if(viewer != nullptr) delete viewer;
	if(cam != nullptr) delete cam;
	if(shader != nullptr) delete shader;

	if(quad != nullptr) delete quad;
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
		system("PAUSE");
		exit(EXIT_FAILURE);
	}
	if (!shader->loadFromFile("mandelbulb.vert", "mandelbulb.frag"))
	{
		// some error occurred
 		std::cout << "Unable to load shaders." << std::endl;
		system("PAUSE");
		exit(EXIT_FAILURE);
	}

	float screenWidth = (float)engine->getWindow()->getSize().x;
	float screenHeight = (float)engine->getWindow()->getSize().y;
	quad = new sf::RectangleShape(sf::Vector2f(engine->getWindow()->getSize()));

	infoBg.setFillColor(sf::Color(0, 0, 0, 150));

	if (!infoFont.loadFromFile("arial.ttf")) {
		 cout << "Unable to find font arial.tff" << endl;
		system("PAUSE");
		 exit(EXIT_FAILURE);
	}

	info.setString("hello");
	info.setFont(infoFont);
	info.setCharacterSize(INFO_FONT_SIZE_PX);
	info.setFillColor(sf::Color::White);

	sf::Shader::bind(shader);
	sf::Transform viewMatrix = engine->getWindow()->getView().getTransform();
	shader->setUniform("projViewMatrix", (sf::Glsl::Mat4)viewMatrix.getMatrix());
	shader->setUniform("screen_width", screenWidth);
	shader->setUniform("screen_height", screenHeight);
	sf::Shader::bind(NULL);

	engine->registerInputListener(viewer);
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
	shader->setUniform("epsilon_limit", EPSILON_LIMIT);
	shader->setUniform("max_dist", MAX_DIST);
	shader->setUniform("max_bailout", MAX_BAILOUT);
	shader->setUniform("max_iter", MAX_ITER);
	shader->setUniform("max_steps", MAX_STEPS);
	shader->setUniform("power", POWER);

	sf::Shader::bind(NULL);

	if (viewer->infoToggle) updateInfo();
}

void MandelbulbViewer::draw()
{
	engine->getWindow()->draw(*quad, shader);
	if (viewer->infoToggle) {
		engine->getWindow()->draw(infoBg);
		engine->getWindow()->draw(info);
	}
}

void MandelbulbViewer::updateInfo()
{
	stringstream ss; 

	float fps = engine->getFPS();
	ss << "fps: " << fps << endl;

	float speed = cam->getSpeed();
	ss << "speed: " << speed << endl;

	
	float mindist = cam->camera->estimateMandelbulbDistance();
	ss << "mindist: " << mindist << endl;

	float scale = cam->camera->scale();
	ss << "scale: " << scale << endl;

	float epsilon = EPSILON_FACTOR * scale;
	ss << "epsilon: " << epsilon << endl;

	float viewlimit = MAX_DIST * scale;
	ss << "viewlimit: " << viewlimit << endl;

	info.setString(ss.str());
	infoBg.setSize(sf::Vector2f(info.getGlobalBounds().width, info.getGlobalBounds().height));
}


MandelbulbViewer::ViewerInputListener::ViewerInputListener() : infoToggle(false) {}

void MandelbulbViewer::ViewerInputListener::update(const float dt) {}

void MandelbulbViewer::ViewerInputListener::keyPressed(const sf::Keyboard::Key &key, const float dt)
{
	if (key == sf::Keyboard::Tab) infoToggle = !infoToggle;
}

void MandelbulbViewer::ViewerInputListener::keyHeld(const sf::Keyboard::Key &key, const float dt) {}
void MandelbulbViewer::ViewerInputListener::keyReleased(const sf::Keyboard::Key &key, const float dt) {}

	
void MandelbulbViewer::ViewerInputListener::mouseMoved(const int mouseX, const int mouseY, const int dx, const int dy, const float dt) {}
void MandelbulbViewer::ViewerInputListener::mousePressed(const sf::Mouse::Button &btn, const int mouseX, const int mouseY, const float dt) {}
void MandelbulbViewer::ViewerInputListener::mouseReleased(const sf::Mouse::Button &btn, const int mouseX, const int mouseY, const float dt) {}
void MandelbulbViewer::ViewerInputListener::mouseScrolled(const float delta, const int mouseX, const int mouseY, const float dt) {}