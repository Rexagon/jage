#pragma once

#include <memory>

#include <SFML/Graphics/Font.hpp>

#include "AbstractFactory.h"

class FontFactory : public AbstractFactory
{
public:
	FontFactory(const std::string& filename);

	void* load() override;
	void clear() override;

private:
	std::string m_filename;

	std::unique_ptr<sf::Font> m_data;
	std::string m_buffer;
};