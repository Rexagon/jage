#include "SoundBufferFactory.h"

#include "FileManager.h"

SoundBufferFactory::SoundBufferFactory(const std::string & filename) :
	AbstractFactory(tag<sf::SoundBuffer>{}), m_data(nullptr),
	m_filename(filename)
{
}

void * SoundBufferFactory::load()
{
	if (m_data == nullptr) {
		std::unique_ptr<sf::SoundBuffer> sound = std::make_unique<sf::SoundBuffer>();

		std::string data = FileManager::open(m_filename);
		if (!sound->loadFromMemory(data.data(), data.size())) {
			throw std::runtime_error("Unable to load sound: \"" + m_assignedName + "\" (" + m_filename + ")");
		}

		m_data = std::move(sound);
	}
	
	return m_data.get();
}

void SoundBufferFactory::clear()
{
	m_data.reset();
}
