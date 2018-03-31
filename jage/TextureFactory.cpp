#include "TextureFactory.h"

#include <SFML/Graphics/Image.hpp>

#include "FileManager.h"

TextureFactory::TextureFactory(const std::string & filename) :
	AbstractFactory(tag<Texture>{}), m_data(nullptr),
	m_filename(filename)
{
}

void * TextureFactory::load()
{
	if (m_data == nullptr) {
		std::unique_ptr<Texture> texture = std::make_unique<Texture>();

		std::string data = FileManager::open(m_filename);
		sf::Image image;
		if (!image.loadFromMemory(data.data(), data.size())) {
			throw std::runtime_error("Unable to load texture: \"" + m_assignedName + "\" (" + m_filename + ")");
		}

		if (!texture->init(image.getSize().x, image.getSize().y, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, (void*)image.getPixelsPtr())) {
			throw std::runtime_error("Unable to init texture: \"" + m_assignedName + "\" (" + m_filename + ")");
		}

		m_data = std::move(texture);
	}

	return m_data.get();
}

void TextureFactory::clear()
{
	m_data.reset(nullptr);
}
