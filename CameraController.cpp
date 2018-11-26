#include "CameraController.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/System/Vector3.hpp>

#include <iostream>
#include <cmath>
using namespace std;

#include "Vector3f.h"
#include "Constants.h"

CameraController::CameraController(float viewportWidth, float viewportHeight)
	: camera(new Camera(viewportWidth, viewportHeight))
	, speed(CAM_UNIT_SPEED)
{}

float CameraController::getSpeed() const
{
	return speed;
}

void CameraController::update(const float dt)
{}

void CameraController::keyHeld(const sf::Keyboard::Key &key, const float dt)
{
	float velocity = speed * max(0.00001f, camera->scale());
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
	float deltaX = (float)dx * CAM_DEGREES_PER_PIXEL;
	float deltaY = (float)dy * CAM_DEGREES_PER_PIXEL;

	camera->direction.rotate(camera->up, deltaX);

	Vector3f right(camera->direction);
	right.cross(camera->up).normalize();

	camera->direction.rotate(right, deltaY);
	camera->up.rotate(right, deltaY);
}

void CameraController::mouseScrolled(const float scrollDelta, const int mouseX, const int mouseY, const float timeDelta)
{
	speed += scrollDelta*0.2f;
	speed = max(0.1f, speed);
	cout << speed << endl;
}

// Unused functions
void CameraController::keyPressed(const sf::Keyboard::Key &key, const float dt) {}
void CameraController::keyReleased(const sf::Keyboard::Key &key, const float dt) {}
void CameraController::mouseReleased(const sf::Mouse::Button &btn, const int mouseX, const int mouseY, const float dt) {}
void CameraController::mousePressed(const sf::Mouse::Button &btn, const int mouseX, const int mouseY, const float dt) {}