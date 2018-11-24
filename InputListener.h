#ifndef INPUT_LISTENER_H
#define INPUT_LISTENER_H

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

class InputListener
{
public:
	virtual void update(const float dt) = 0;

	virtual void keyPressed(const sf::Keyboard::Key &key, const float dt) = 0;
	virtual void keyHeld(const sf::Keyboard::Key &key, const float dt) = 0;
	virtual void keyReleased(const sf::Keyboard::Key &key, const float dt) = 0;

	virtual void mouseMoved(const int mouseX, const int mouseY, const int dx, const int dy, const float dt) = 0;
	virtual void mousePressed(const sf::Mouse::Button &btn, const int mouseX, const int mouseY, const float dt) = 0;
	virtual void mouseReleased(const sf::Mouse::Button &btn, const int mouseX, const int mouseY, const float dt) = 0;
	virtual void mouseScrolled(const float delta, const int mouseX, const int mouseY, const float dt) = 0;
};

#endif /* INPUT_LISTENER_H */