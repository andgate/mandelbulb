#include "CameraController.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/System/Vector3.hpp>

#include <iostream>
#include <cmath>
using namespace std;

#include "Vector3f.h"

CameraController::CameraController(float viewportWidth, float viewportHeight)
	: camera(new Camera(viewportWidth, viewportHeight))
	, degreesPerPixel(0.2f)
	, scalePerScroll(1.01f)
{}

void CameraController::update(const float dt)
{}

void CameraController::keyHeld(const sf::Keyboard::Key &key, const float dt)
{
	float bulb_dist = camera->estimateMandelbulbDistance();
	 
	float velocity;
	if (bulb_dist > 0)
		velocity = 1.0f * sqrt(bulb_dist) / camera->zoom;
	else
		velocity = 1.0f / camera->zoom;
	
	float offset = velocity * dt;

	switch (key)
	{

	// Move forwards
	case sf::Keyboard::W:
		{
			Vector3f tmp(camera->direction);
			tmp.normalize();
			tmp *= abs(offset);
			camera->position += tmp;
		}
		break;

	// Move backwards
	case sf::Keyboard::S:
		{
			Vector3f tmp(camera->direction);
			tmp.normalize();
			tmp *= abs(offset);
			camera->position -= tmp;
		}
		break;

	// Move left
	case sf::Keyboard::A:
		{
			Vector3f tmp(camera->direction);
			tmp.cross(camera->up).normalize();
			tmp *= abs(offset);
			camera->position += tmp;
		}
		break;

	// Move right
	case sf::Keyboard::D:
		{
			Vector3f tmp(camera->up);
			tmp.cross(camera->direction).normalize();
			tmp *= abs(offset);
			camera->position += tmp;
		}
		break;

	default:
		break;
	}
}


const float clamp(const float &n, const float &lower, const float &upper) {
  return std::max(lower, std::min(n, upper));
}

void CameraController::mouseMoved(const int mouseX, const int mouseY, const int dx, const int dy, const float dt)
{
	float max_distance = camera->estimateMandelbulbDistance();
	float deltaX = (float)dx * degreesPerPixel / camera->zoom;
	float deltaY = (float)dy * degreesPerPixel / camera->zoom;

	camera->direction.rotate(camera->up, deltaX);

	Vector3f right(camera->direction);
	right.cross(camera->up).normalize();

	camera->direction.rotate(right, deltaY);
	camera->up.rotate(right, deltaY);
}

void CameraController::mouseScrolled(const float scrollDelta, const int mouseX, const int mouseY, const float timeDelta)
{
	float scale = scalePerScroll * min(2.0f, abs(scrollDelta));
	
	if (scrollDelta > 0)
        camera->zoom *= scale;
	else if (scrollDelta < 0)
		camera->zoom /= scale;

	camera->zoom = max(camera->zoom, 1.0f);

	cout << "zoom = " << camera->zoom << endl;
}

// Unused functions
void CameraController::keyPressed(const sf::Keyboard::Key &key, const float dt) {}
void CameraController::keyReleased(const sf::Keyboard::Key &key, const float dt) {}
void CameraController::mouseReleased(const sf::Mouse::Button &btn, const int mouseX, const int mouseY, const float dt) {}
void CameraController::mousePressed(const sf::Mouse::Button &btn, const int mouseX, const int mouseY, const float dt) {}