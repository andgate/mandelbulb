#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include "Camera.h"
#include "InputListener.h"
#include "Vector3f.h"

class CameraController : public InputListener
{
public:
	Camera *camera;

	CameraController(float viewportWidth, float viewportHeight);

	const float* getCameraMatrix() const;

	void update(const float dt);

	void keyPressed(const sf::Keyboard::Key &key, const float dt);
	void keyHeld(const sf::Keyboard::Key &key, const float dt);
	void keyReleased(const sf::Keyboard::Key &key, const float dt);

	
	void mouseMoved(const int mouseX, const int mouseY, const int dx, const int dy, const float dt);
	void mousePressed(const sf::Mouse::Button &btn, const int mouseX, const int mouseY, const float dt);
	void mouseReleased(const sf::Mouse::Button &btn, const int mouseX, const int mouseY, const float dt);
	void mouseScrolled(const float delta, const int mouseX, const int mouseY, const float dt);

private:
	float speed_up;
};


#endif /* CAMERA_CONTROLLER_H */