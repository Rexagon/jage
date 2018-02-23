#include "FrameBuffer.h"

#include "Log.h"

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height, bool depthEnabled) :
	m_isDepthEnabled(depthEnabled)
{
	init(width, height, depthEnabled);
}

FrameBuffer::FrameBuffer(const uvec2 & size, bool depthEnabled) :
	m_isDepthEnabled(depthEnabled)
{
	init(size.x, size.y, depthEnabled);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_id);
	if (m_isDepthEnabled) {
		glDeleteRenderbuffers(1, &m_depthBuffer);
	}
}

void FrameBuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void FrameBuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

sf::Texture & FrameBuffer::getColorTexture()
{
	return m_colorTexture;
}

void FrameBuffer::init(unsigned int width, unsigned int height, bool depthEnabled)
{
	if (!m_colorTexture.create(width, height)) {
		throw std::runtime_error("Unable to create framebuffer color texture");
	}
	glGenFramebuffers(1, &m_id);
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture.getNativeHandle(), 0);
	Log::write(m_id, glGetError());

	if (depthEnabled) {
		glGenRenderbuffers(1, &m_depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}