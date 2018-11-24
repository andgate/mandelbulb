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
	, mouseX(0) , mouseY(0)
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

	centerMouse();

	while (window->isOpen())
    {
		preupdate();

		sf::Event event;
		while (window->pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
                window->close();
				break;

			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape) window->close();
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

		centerMouse();

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

/** OpenGL Helpers **/

void Engine::initGL()
{
	// Set color and depth clear value
	glClearDepth(1.f);
	glClearColor(0.f, 0.f, 0.f, 0.f);

	// Enable Z-buffer read and write
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);


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