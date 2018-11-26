#ifndef ENGINE_H
#define ENGINE_H

/* Standard Libary */
#include <functional>
#include <unordered_set>
#include <string>
using namespace std;

/* SFML */
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "InputListener.h"


class Engine
{
public:
	Engine(const string &title, const int &width, const int &height, const int &max_fps);
	~Engine();

	int run();

	sf::RenderWindow* getWindow();
	sf::Clock& getClock();

	void setInitFunc  (std::function<void(void)> f);
	void setPreupdateFunc(std::function<void(void)> f);
	void setUpdateFunc(std::function<void(const float)> f);
	void setDrawFunc  (std::function<void(void)> f);

	void registerInputListener(InputListener *listener);

	float getFPS();

private:
	int max_fps;
	int cur_fps;
	float deltaTime;
	int mouseX, mouseY;
	sf::String title;
	sf::ContextSettings settings;
	bool isFullscreen;

	sf::RenderWindow *window;
	sf::Clock clock;

	std::vector<InputListener*> *listeners;
	std::unordered_set<sf::Keyboard::Key> *heldKeys;

	std::function<void(void)> init;
	std::function<void(void)> preupdate;
	std::function<void(const float)> update;
	std::function<void(void)> draw;


	void updateInputListeners(const float dt) const;

	void notifyKeyPressed(const sf::Keyboard::Key &key, const float dt) const;
	void notifyKeyReleased(const sf::Keyboard::Key &key, const float dt) const;
	void notifyKeyHeld(const sf::Keyboard::Key &key, const float dt) const;

	void notifyMouseMoved(const sf::Event::MouseMoveEvent &ev, const float dt);
	void notifyMousePressed(const sf::Event::MouseButtonEvent &ev, const float dt) const;
	void notifyMouseReleased(const sf::Event::MouseButtonEvent &ev, const float dt) const;
	void notifyMouseScrolled(const sf::Event::MouseWheelScrollEvent &ev, const float dt) const;
	
	void openWindow();
	void openDefaultWindow();
	void openFullscreenWindow();

	void centerMouse();
	void wrapMouse();

	void checkInput(const sf::Event &e);

	/** OpenGL Helpers **/
	void initGL();
	void perspectiveGL( double fovY, double aspect, double zNear, double zFar );
};

#endif /* ENGINE_H */