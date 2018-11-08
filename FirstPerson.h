#ifndef FIRST_PERSON_h
#define FIRST_PERSON_H

#include "Camera.h"
#include "InputListener.h"

class FirstPerson : public InputListener
{
public:
	FirstPerson();

	Camera* getCamera() const;
	const float* getCameraMatrix() const;

	void update(const float dt);

	void keyPressed(const sf::Keyboard::Key &key, const float dt);
	void keyHeld(const sf::Keyboard::Key &key, const float dt);
	void keyReleased(const sf::Keyboard::Key &key, const float dt);

	
	void mouseMoved(const int mouseX, const int mouseY, const float dt);
	void mousePressed(const sf::Mouse::Button &btn, const int mouseX, const int mouseY, const float dt);
	void mouseReleased(const sf::Mouse::Button &btn, const int mouseX, const int mouseY, const float dt);
	void mouseScrolled(const float delta, const int mouseX, const int mouseY, const float dt);

private:
	Camera *camera;
	float velocity;
};


#endif /* FIRST_PERSON_H */