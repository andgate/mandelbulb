#include "FirstPerson.h"

#include <SFML/Window/Keyboard.hpp>

#include <iostream>
using namespace std;

#include "Vector3f.h"

FirstPerson::FirstPerson()
	: camera(new Camera())
	, velocity(0.5f)
{}

const float* FirstPerson::getCameraMatrix() const
{
	return camera->getMatrix();
}

Camera* FirstPerson::getCamera() const
{
	return camera;
}

void FirstPerson::update(const float dt)
{
	camera->update();
}

void FirstPerson::keyHeld(const sf::Keyboard::Key &key, const float dt)
{
	switch (key)
	{

	// Move forwards
	case sf::Keyboard::W:
		{
			Vector3f tmp(camera->direction);
			tmp.normalize();
			tmp *= abs(dt * velocity);
			camera->position += tmp;
		}
		break;

	// Move backwards
	case sf::Keyboard::S:
		{
			Vector3f tmp(camera->direction);
			tmp.normalize();
			tmp *= abs(dt * velocity);
			camera->position -= tmp;
		}
		break;

	// Move left
	case sf::Keyboard::A:
		{
			Vector3f tmp(camera->direction);
			tmp.cross(camera->up).normalize();
			tmp *= abs(dt * velocity);
			camera->position += tmp;
		}
		break;

	// Move right
	case sf::Keyboard::D:
		{
			Vector3f tmp(camera->up);
			tmp.cross(camera->direction).normalize();
			tmp *= abs(dt * velocity);
			camera->position += tmp;
		}
		break;

	// Float up
	case sf::Keyboard::Q:
		{
			Vector3f tmp(camera->up);
			tmp.normalize();
			tmp *= abs(dt * velocity);
			camera->position += tmp;
		}
		break;

	// Move Down
	case sf::Keyboard::E:
		{
			Vector3f tmp(camera->up);
			tmp.normalize();
			tmp *= abs(dt * velocity);
			camera->position -= tmp;
		}
		break;

	default:
		break;
	}
}



void FirstPerson::mouseMoved(const int mouseX, const int mouseY, const float dt) {}
void FirstPerson::mouseScrolled(const float delta, const int mouseX, const int mouseY, const float dt) {}

// Unused functions
void FirstPerson::keyPressed(const sf::Keyboard::Key &key, const float dt) {}
void FirstPerson::keyReleased(const sf::Keyboard::Key &key, const float dt) {}
void FirstPerson::mouseReleased(const sf::Mouse::Button &btn, const int mouseX, const int mouseY, const float dt) {}
void FirstPerson::mousePressed(const sf::Mouse::Button &btn, const int mouseX, const int mouseY, const float dt) {}
