#include "FrameBuffer.h"

#include "Log.h"

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height, GLenum type, unsigned int colorAttachmentCount, bool hasDepthStencil) :
	m_size(width, height), m_hasDepthStencil(hasDepthStencil)
{
	glGenFramebuffers(1, &m_id);
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);

	m_colorAttachments.resize(colorAttachmentCount);
	for (unsigned int i = 0; i < colorAttachmentCount; ++i) {
		Texture& texture = m_colorAttachments[static_cast<size_t>(i)];
		texture.setFilters(GL_NEAREST, GL_NEAREST, false);
		texture.setWrapMode(GL_CLAMP_TO_EDGE);

		GLenum internalFormat;
		switch (type) {
		case GL_HALF_FLOAT:
			internalFormat = GL_RGBA16F;
			break;

		case GL_FLOAT:
			internalFormat = GL_RGBA32F;
			break;

		default:
			internalFormat = GL_RGBA;
			break;
		}
		texture.init(width, height, internalFormat, GL_RGBA, type, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture.getHandle(), 0);
	}

	if (hasDepthStencil) {
		m_depthStencilAttachment.setFilters(GL_NEAREST, GL_NEAREST, false);
		m_depthStencilAttachment.setWrapMode(GL_CLAMP_TO_EDGE);
		m_depthStencilAttachment.init(width, height, GL_DEPTH_STENCIL, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthStencilAttachment.getHandle(), 0);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		Log::write("Unable to create framebuffer");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_id);
}

void FrameBuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void FrameBuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::resize(const uvec2& size)
{
	m_size = size;

	for (size_t i = 0; i < m_colorAttachments.size(); ++i) {
		m_colorAttachments[i].resize(size.x, size.y);
	}

	if (m_hasDepthStencil) {
		m_depthStencilAttachment.resize(size.x, size.y);
	}
}

uvec2 FrameBuffer::getSize() const
{
	return m_size;
}

Texture * FrameBuffer::getColorTexture(unsigned int index)
{
	if (index < m_colorAttachments.size()) {
		return &m_colorAttachments[index];
	}
	else {
		return nullptr;
	}
}

Texture * FrameBuffer::getDepthStencilTexture()
{
	if (m_hasDepthStencil) {
		return &m_depthStencilAttachment;
	}
	else {
		return nullptr;
	}
}