/* Header file */
#include "Engine.h"

/* Standard Libary */
#include <string>
#include <iostream>
using namespace std;

/* SFML */
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>


Engine::Engine(int width, int height, const string& title)
	: deltaTime(1.0f / 60.0f) // assume initial dt is at 60fps
	, clock()
	, listeners(new vector<InputListener*>())
	, heldKeys(new unordered_set<sf::Keyboard::Key>())
{
	// Have to pass title as a c_str,
	// or else sfml crashes
	sf::String title_sf = sf::String(title.c_str());

	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 4;
	settings.minorVersion = 4;

	window = new sf::RenderWindow(sf::VideoMode(width, height), title_sf, sf::Style::Default, settings);
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
	window->setVerticalSyncEnabled(true);
	window->setActive(true);

	//initGL();
	init();

	clock.restart();

	while (window->isOpen())
    {
		sf::Event event;
		while (window->pollEvent(event))
		{
            if (event.type == sf::Event::Closed)
            {
                if (event.type == sf::Event::Closed)
                window->close();
			}

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape) window->close();
				notifyKeyPressed(event.key.code, deltaTime);
				heldKeys->insert(event.key.code);
			}

			if (event.type == sf::Event::KeyReleased)
			{
				notifyKeyReleased(event.key.code, deltaTime);
				heldKeys->erase(event.key.code);
			}

			if (event.type == sf::Event::MouseMoved)
			{
				notifyMouseMoved(event.mouseMove, deltaTime);
			}

			if (event.type == sf::Event::MouseButtonPressed)
			{
				notifyMousePressed(event.mouseButton, deltaTime);
			}

			if (event.type == sf::Event::MouseButtonReleased)
			{
				notifyMouseReleased(event.mouseButton, deltaTime);
			}

			if (event.type == sf::Event::MouseWheelScrolled)
			{
				notifyMouseScrolled(event.mouseWheelScroll, deltaTime);
			}
        }

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

void Engine::notifyMouseMoved(const sf::Event::MouseMoveEvent &ev, const float dt) const
{
	for (auto& l : *listeners)
	{
		l->mouseMoved(ev.x, ev.y, dt);
	}
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


/** OpenGL Helpers **/

void Engine::initGL()
{
	// Set color and depth clear value
	glClearDepth(1.f);
	glClearColor(0.f, 0.f, 0.f, 0.f);

	// Enable Z-buffer read and write
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glEnable(GL_SMOOTH);

	// Setup a perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	perspectiveGL(90.f, 1.f, 1.f, 500.f);
}

void Engine::perspectiveGL( double fovY, double aspect, double zNear, double zFar )
{
    const double pi = 3.1415926535897932384626433832795;
    double fW, fH;

    //fH = tan( (fovY / 2) / 180 * pi ) * zNear;
    fH = tan( fovY / 360 * pi ) * zNear;
    fW = fH * aspect;

    glFrustum( (GLdouble)  -fW, (GLdouble)fW
		     , (GLdouble)  -fH, (GLdouble)fH
			 , (GLdouble)zNear, (GLdouble)zFar );
}