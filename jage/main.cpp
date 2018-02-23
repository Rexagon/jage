#include "Core.h"

#include "Game.h"

int main() {
	Core::Parameters parameters;
	parameters.width = 1024;
	parameters.height = 768;
	parameters.title = "jage test";
	parameters.vsync = true;
	Core::init(parameters);

	try {
		SceneManager::addScene<Game>();

		Core::run();
	}
	catch (const std::exception& e) {
		Log::write("Error:", e.what());
	}

	Core::close();

	return 0;
}
