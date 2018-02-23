#include "MusicFactory.h"

#include "FileManager.h"

MusicFacroty::MusicFacroty(const std::string & filename) :
	AbstractFactory(tag<sf::Music>{}), m_data(nullptr),
	m_filename(filename)
{
}

void * MusicFacroty::load()
{
	if (m_data == nullptr) {
		std::unique_ptr<sf::Music> music = std::make_unique<sf::Music>();

		m_buffer = FileManager::open(m_filename);
		if (!music->openFromMemory(m_buffer.data(), m_buffer.size())) {
			throw std::runtime_error("Unable to load music: \"" + m_assignedName + "\" (" + m_filename + ")");
		}

		m_data = std::move(music);
	}

	return m_data.get();
}

void MusicFacroty::clear()
{
	m_data.reset(nullptr);
	m_buffer.clear();
}
