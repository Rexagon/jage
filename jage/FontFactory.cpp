#include "FontFactory.h"

#include "FileManager.h"

FontFactory::FontFactory(const std::string& filename) :
	AbstractFactory(tag<sf::Font>{}), m_data(nullptr),
	m_filename(filename)
{
}

void * FontFactory::load()
{
	if (m_data == nullptr) {
		std::unique_ptr<sf::Font> font = std::make_unique<sf::Font>();

		m_buffer = FileManager::open(m_filename);
		if (!font->loadFromMemory(m_buffer.data(), m_buffer.size())) {
			throw std::runtime_error("Unable to load font: \"" + m_assignedName + "\" (" + m_filename + ")");
		}

		m_data = std::move(font);
	}

	return m_data.get();
}

void FontFactory::clear()
{
	m_data.reset(nullptr);
	m_buffer.clear();
}