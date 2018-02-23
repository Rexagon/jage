#pragma once

#include <memory>

#include <SFML/Graphics/Texture.hpp>

#include "AbstractFactory.h"

class TextureFactory : public AbstractFactory
{
public:
	TextureFactory(const std::string& filename);

	void* load() override;
	void clear() override;

private:
	std::string m_filename;

	std::unique_ptr<sf::Texture> m_data;
};
