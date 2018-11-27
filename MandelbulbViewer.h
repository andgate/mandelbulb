#ifndef MANDELBULB_VIEWER_H
#define MANDELBULB_VIEWER_H

#include "Engine.h"
#include "CameraController.h"
#include "InputListener.h"
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Text.hpp>

class MandelbulbViewer
{
public:
	MandelbulbViewer(const int &windowWidth, const int &windowHeight, const int &max_fps);
	~MandelbulbViewer();

	int run();

private:
	class ViewerInputListener : public InputListener
	{
	public:
		bool infoToggle;
		bool fogToggle;

		ViewerInputListener();

		void update(const float dt);

		void keyPressed(const sf::Keyboard::Key &key, const float dt);
		void keyHeld(const sf::Keyboard::Key &key, const float dt);
		void keyReleased(const sf::Keyboard::Key &key, const float dt);

	
		void mouseMoved(const int mouseX, const int mouseY, const int dx, const int dy, const float dt);
		void mousePressed(const sf::Mouse::Button &btn, const int mouseX, const int mouseY, const float dt);
		void mouseReleased(const sf::Mouse::Button &btn, const int mouseX, const int mouseY, const float dt);
		void mouseScrolled(const float delta, const int mouseX, const int mouseY, const float dt);
	};

private:
	Engine *engine;
	ViewerInputListener *viewer;
	CameraController *cam;
	sf::Shader *shader;
	sf::RectangleShape *quad;
	sf::Font infoFont;
	sf::Text info;
	sf::RectangleShape infoBg;

	void init();
	void preupdate();
	void update(const float dt);
	void draw();

	void updateInfo();

	
};

#endif // MANDELBULB_VIEWER_H