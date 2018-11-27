/* Header file */
#include "Engine.h"

/* Standard Libary */
#include <string>
#include <iostream>
using namespace std;

/* SFML */
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

#include "Constants.h"

Engine::Engine(const string& title, const int &width, const int &height, const int &max_fps)
	: max_fps(max_fps)
	, deltaTime(1.0f / (float)max_fps) // assume initial dt is at 60fps
	, mouseX(0) , mouseY(0)
	, clock()
	, listeners(new vector<InputListener*>())
	, heldKeys(new unordered_set<sf::Keyboard::Key>())
	, title(sf::String(title.c_str()))
	, settings()
	, isFullscreen(IS_FULLSCREEN)
{
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 4;
	settings.minorVersion = 4;

	window = new sf::RenderWindow(sf::VideoMode(width, height), title, sf::Style::Default, settings);
	window->setMouseCursorGrabbed(true);
	window->setMouseCursorVisible(false);

	std::cout << "version:" << settings.majorVersion << "." << settings.minorVersion << std::endl;
}

Engine::~Engine()
{
	if (window != nullptr) delete window;
	if (listeners != nullptr) delete listeners;
	if (heldKeys != nullptr) delete heldKeys;
}


int Engine::run()
{
	window->setVerticalSyncEnabled(false);
	window->setActive(true);

	init();

	clock.restart();

	centerMouse();
	while (window->isOpen())
    {
		preupdate();

		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window->close();

			if (event.type == sf::Event::GainedFocus) {
				window->setMouseCursorGrabbed(true);
				window->setMouseCursorVisible(false);
			}

			if (event.type == sf::Event::LostFocus) {
				window->setMouseCursorGrabbed(false);
				window->setMouseCursorVisible(true);
			}

			// Check for input events when a window has focus
			if (window->hasFocus()) checkInput(event);
		}

		if (window->hasFocus()) centerMouse();  // THIS MUST BE OUTSIDE THE EVENT POLLING LOOP

		if (!heldKeys->empty())
		{
			for (const auto& k : *heldKeys)
			{
				notifyKeyHeld(k, deltaTime);
			}
		}

		updateInputListeners(deltaTime);
		update(deltaTime);

		window->clear(sf::Color::Black);
		draw();
		window->display();

		deltaTime = clock.restart().asSeconds();
		cur_fps = 1.0f / deltaTime;

		
    }

	return EXIT_SUCCESS;
}

sf::RenderWindow* Engine::getWindow()
{
	return this->window;
}

sf::Clock& Engine::getClock()
{
	return this->clock;
}


void Engine::setInitFunc(std::function<void(void)> f)
{
	this->init = f;
}

void Engine::setPreupdateFunc(std::function<void(void)> f)
{
	this->preupdate = f;
}

void Engine::setUpdateFunc(std::function<void(const float)> f)
{
	this->update = f;
}

void Engine::setDrawFunc  (std::function<void(void)> f)
{
	this->draw = f;
}


void Engine::registerInputListener(InputListener *listener)
{
	listeners->push_back(listener);
}

void Engine::updateInputListeners(const float dt) const
{
	for (auto& l : *listeners)
	{
		l->update(dt);
	}
}

void Engine::notifyKeyPressed(const sf::Keyboard::Key &key, const float dt) const
{
	for (auto& l : *listeners)
	{
		l->keyPressed(key, dt);
	}
}

void Engine::notifyKeyReleased(const sf::Keyboard::Key &key, const float dt) const
{
	for (auto& l : *listeners)
	{
		l->keyReleased(key, dt);
	}
}

void Engine::notifyKeyHeld(const sf::Keyboard::Key &key, const float dt) const
{
	for (auto& l : *listeners)
	{
		l->keyHeld(key, dt);
	}
}

void Engine::notifyMouseMoved(const sf::Event::MouseMoveEvent &ev, const float dt)
{
	sf::Vector2i halfscreen = sf::Vector2i(window->getSize());
	halfscreen /= 2;

	int dx = ev.x - mouseX;
	int dy = ev.y - mouseY;

	for (auto& l : *listeners)
	{
		l->mouseMoved(ev.x, ev.y, dx, dy, dt);
	}

	mouseX = ev.x;
	mouseY = ev.y;
}

void Engine::notifyMousePressed(const sf::Event::MouseButtonEvent &ev, const float dt) const
{
	for (auto& l : *listeners)
	{
		l->mousePressed(ev.button, ev.x, ev.y, dt);
	}
}

void Engine::notifyMouseReleased(const sf::Event::MouseButtonEvent &ev, const float dt) const
{
	for (auto& l : *listeners)
	{
		l->mouseReleased(ev.button, ev.x, ev.y, dt);
	}
}

void Engine::notifyMouseScrolled(const sf::Event::MouseWheelScrollEvent &ev, const float dt) const
{
	for (auto& l : *listeners)
	{
		l->mouseScrolled(ev.delta, ev.x, ev.y, dt);
	}
}

void Engine::openWindow()
{
	if (isFullscreen)
		openFullscreenWindow();
	else
		openDefaultWindow();
}

void Engine::openDefaultWindow()
{
	window->create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), title, sf::Style::Default, settings);
	window->setMouseCursorGrabbed(true);
	window->setMouseCursorVisible(false);
	isFullscreen = false;
}

void Engine::openFullscreenWindow()
{
	window->create(sf::VideoMode::getFullscreenModes() [0], title, sf::Style::Fullscreen, settings);	
	window->setMouseCursorGrabbed(true);
	window->setMouseCursorVisible(false);
	isFullscreen = true;
}



void Engine::centerMouse()
{
	mouseX = window->getSize().x/2;
	mouseY = window->getSize().y/2;
	sf::Mouse::setPosition(sf::Vector2i(mouseX, mouseY), *window);
}

void Engine::wrapMouse()
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
	sf::Vector2i screen = sf::Vector2i(window->getSize());

	if (mousePos.x <= 1)
		mousePos.x = screen.x-1;

	if (mousePos.x >= screen.x-1)
		mousePos.x = 1;

	sf::Mouse::setPosition(mousePos, *window);

	mouseX = mousePos.x;
	mouseY = mousePos.y;
}

void Engine::checkInput(const sf::Event &event)
{
	switch (event.type)
	{
	case sf::Event::KeyPressed:
		if (event.key.code == sf::Keyboard::Escape) window->close();
		if (event.key.code == sf::Keyboard::F) {
			isFullscreen = !isFullscreen; openWindow();
		}
		notifyKeyPressed(event.key.code, deltaTime);
		heldKeys->insert(event.key.code);
		break;

	case sf::Event::KeyReleased:
		notifyKeyReleased(event.key.code, deltaTime);
		heldKeys->erase(event.key.code);
		break;

	case sf::Event::MouseMoved:
		notifyMouseMoved(event.mouseMove, deltaTime);
		break;

	case sf::Event::MouseButtonPressed:
		notifyMousePressed(event.mouseButton, deltaTime);
		break;

	case sf::Event::MouseButtonReleased:
		notifyMouseReleased(event.mouseButton, deltaTime);
		break;

	case sf::Event::MouseWheelScrolled:
		notifyMouseScrolled(event.mouseWheelScroll, deltaTime);
		break;

	default:
		break;
	}
}

float Engine::getFPS()
{
	return cur_fps;
}