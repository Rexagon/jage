#include "TextureFactory.h"

#include "FileManager.h"

TextureFactory::TextureFactory(const std::string & filename) :
	AbstractFactory(tag<sf::Texture>{}), m_data(nullptr),
	m_filename(filename)
{
}

void * TextureFactory::load()
{
	if (m_data == nullptr) {
		std::unique_ptr<sf::Texture> texture = std::make_unique<sf::Texture>();

		std::string data = FileManager::open(m_filename);
		if (!texture->loadFromMemory(data.data(), data.size())) {
			throw std::runtime_error("Unable to load texture: \"" + m_assignedName + "\" (" + m_filename + ")");
		}

		m_data = std::move(texture);
	}

	return m_data.get();
}

void TextureFactory::clear()
{
	m_data.reset(nullptr);
}
