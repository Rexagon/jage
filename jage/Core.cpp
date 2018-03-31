#include "Core.h"

bool Core::m_isInitialized = false;
bool Core::m_isRunning = false;
uint64_t Core::m_globalTimer = 0;

sf::RenderWindow Core::m_window;

void Core::init(const Parameters& parameters)
{
	Log::init();

	sf::ContextSettings settings;
	settings.majorVersion = 4;
    settings.minorVersion = 3;
	settings.antialiasingLevel = 0;
	settings.depthBits = 24;
	settings.stencilBits = 8;

	sf::Uint32 windowStyle = sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize;
	if (parameters.fullscreen) {
		windowStyle |= sf::Style::Fullscreen;
	}

    m_window.create(sf::VideoMode(parameters.width, parameters.height), parameters.title, windowStyle, settings);
	m_window.setVerticalSyncEnabled(parameters.vsync);
	m_window.setActive(true);

	if (glewInit() != GLEW_OK) {
		throw std::runtime_error("Unable to initialize glew");
	}

	RenderStateManager::init();
	ResourceManager::init();
	CursorManager::init(m_window.getSystemHandle());
	FileManager::init<DefaultFileSystem>();
	MaterialManager::init();
	
	m_isInitialized = true;
}

void Core::close()
{
	if (m_isInitialized == false || m_isRunning == true) {
		return;
	}

	SceneManager::close();
	CursorManager::close();
	ResourceManager::close();
	FileManager::close();

	m_window.close();

	m_isRunning = false;
	m_isInitialized = false;
}

void Core::run()
{
	if (m_isInitialized == false || m_isRunning == true) {
		return;
	}

	m_isRunning = true;
	m_globalTimer = 0;

	sf::Clock timer;
	
	while (m_isRunning) {
		sf::Time timeElapsed = timer.restart();
		m_globalTimer += timeElapsed.asMilliseconds();
		float dt = timeElapsed.asSeconds();

		// Handle window and keyboard events
		handleEvents();

		BaseScene* currentScene;

		if (m_isRunning == true &&
			(currentScene = SceneManager::getCurrentScene()) != nullptr)
		{
			//m_guiDesktop->Update(dt);
		}

		if (m_isRunning == true &&
			(currentScene = SceneManager::getCurrentScene()) != nullptr) 
		{
			currentScene->onUpdate(dt);
		}

		m_window.display();

		m_isRunning &= SceneManager::getCurrentScene() != nullptr;
	}
}

void Core::stop()
{
	m_isRunning = false;
}

sf::RenderWindow& Core::getWindow()
{
	return m_window;
}

unsigned long long Core::getGlobalTimer()
{
	return m_globalTimer;
}

void Core::handleEvents()
{
	Input::update();

	sf::Event e;
	while (m_window.pollEvent(e)) {
		switch (e.type) {
		case sf::Event::Closed:
			stop();
			break;

		case sf::Event::Resized:
			if (SceneManager::hasScenes()) {
				SceneManager::getCurrentScene()->onResize(ivec2(
					static_cast<int>(e.size.width),
					static_cast<int>(e.size.height)
				));
			}
			break;

		case sf::Event::GainedFocus:
			if (SceneManager::hasScenes()) {
				SceneManager::getCurrentScene()->onFocusGained();
			}
			break;

		case sf::Event::LostFocus:
			if (SceneManager::hasScenes()) {
				SceneManager::getCurrentScene()->onFocusLost();
			}
			break;

		default:
			break;
		}

		Input::handleEvent(e);
	}
}
