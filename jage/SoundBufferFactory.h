#pragma once

#include <memory>
#include <string>

#include <SFML/Audio/SoundBuffer.hpp>

#include "AbstractFactory.h"

class SoundBufferFactory : public AbstractFactory
{
public:
	SoundBufferFactory(const std::string& filename);

	void* load() override;
	void clear() override;

private:
	std::string m_filename;

	std::unique_ptr<sf::SoundBuffer> m_data;
};