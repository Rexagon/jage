#pragma once

#include <GL/glew.h>
#include <SFML/Graphics.hpp>

#include <SFGUI/Desktop.hpp>
#include <SFGUI/SFGUI.hpp>

#include "ResourceManager.h"
#include "CursorManager.h"
#include "SceneManager.h"
#include "FileManager.h"

#include "SoundBufferFactory.h"
#include "TextureFactory.h"
#include "ShaderFactory.h"
#include "MusicFactory.h"
#include "ModelFactory.h"
#include "FontFactory.h"

#include "Delegate.h"
#include "Input.h"
#include "Log.h"
#include "Time.h"

class Core
{
public:
	struct Parameters
	{
		Parameters() : title("game"), width(800), height(600), fullscreen(false), vsync(true) {}

		std::string title;
		unsigned int width;
		unsigned int height;
		bool fullscreen;
		bool vsync;
	};

	// Creates window, initializes all managers
	static void init(const Parameters& parameters);

    // Clears up all data
	static void close();

	// Starts main game loop
	static void run();

	// Stops game loop
	static void stop();

	// Returns SFML window
	static sf::RenderWindow& getWindow();

	static sfg::SFGUI* getGui();
	static sfg::Desktop* getGuiDesktop();

	// Returns millisecond count since application start
	static unsigned long long getGlobalTimer();

private:
	static void handleEvents();

	static bool m_isInitialized;
	static bool m_isRunning;

	static sf::RenderWindow m_window;

	static std::unique_ptr<sfg::SFGUI> m_gui;
	static std::unique_ptr<sfg::Desktop> m_guiDesktop;

	static unsigned long long m_globalTimer;
};
