#pragma once

#include <GL/glew.h>

#include "Texture.h"
#include "Math.h"

class FrameBuffer
{
public:
	FrameBuffer(unsigned int width, unsigned int height, GLenum type, unsigned int colorAttachmentCount = 1, bool hasDepthStencil = true);
	~FrameBuffer();

	void bind();
	void unbind();

	void resize(unsigned int width, unsigned int height);

	Texture* getColorTexture(unsigned int index = 0);
	Texture* getDepthStencilTexture();

	GLuint getHandle() const { return m_id; }

private:
	void init(unsigned int width, unsigned int height, bool depthEnabled);

	GLuint m_id;

	std::vector<Texture> m_colorAttachments;
	Texture m_depthStencilAttachment;

	bool m_hasDepthStencil;
	uvec2 m_size;
};