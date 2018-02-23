#pragma once

#include <GL/glew.h>
#include <SFML/Graphics/Texture.hpp>

#include "Math.h"

class FrameBuffer
{
public:
	FrameBuffer(unsigned int width, unsigned int height, bool depthEnabled = true);
	FrameBuffer(const uvec2& size, bool depthEnabled = true);
	~FrameBuffer();

	void bind();
	void unbind();

	sf::Texture& getColorTexture();

	GLuint getHandle() const { return m_id; }

private:
	void init(unsigned int width, unsigned int height, bool depthEnabled);

	GLuint m_id;
	bool m_isDepthEnabled;

	sf::Texture m_colorTexture;
	GLuint m_depthBuffer;
};