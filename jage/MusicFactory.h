#pragma once

#include <memory>
#include <string>

#include <SFML/Audio/Music.hpp>

#include "AbstractFactory.h"

class MusicFacroty : public AbstractFactory
{
public:
	MusicFacroty(const std::string& filename);

	void* load() override;
	void clear() override;

private:
	std::string m_filename;

	std::unique_ptr<sf::Music> m_data;
	std::string m_buffer;
};