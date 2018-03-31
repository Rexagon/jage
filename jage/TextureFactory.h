#pragma once

#include <memory>

#include "Texture.h"

#include "AbstractFactory.h"

class TextureFactory : public AbstractFactory
{
public:
	TextureFactory(const std::string& filename);

	void* load() override;
	void clear() override;

private:
	std::string m_filename;

	std::unique_ptr<Texture> m_data;
};
