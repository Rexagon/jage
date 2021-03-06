#pragma once

#include <GL/glew.h>

#include "Math.h"

class Texture
{
public:
	Texture(GLenum target = GL_TEXTURE_2D);
	~Texture();

	// init as 1D texture
	bool init(unsigned int width, 
		GLenum internalFormat, GLenum format, GLenum type, void* data);

	// init as 2D texture
	bool init(unsigned int width, unsigned int height,
		GLenum internalFormat, GLenum format, GLenum type, void* data);

	// init as 3D texture
	bool init(unsigned int width, unsigned int height, unsigned int depth,
		GLenum internalFormat, GLenum format, GLenum type, void* data);

	void resize(unsigned int width, unsigned int height = 0, unsigned int depth = 0);
	
	void setFilters(GLenum minFilter, GLenum maxFilter, bool bind = true);
	void setWrapMode(GLenum wrapMode, bool bind = true);

	void generateMipmap();

	void bind(unsigned int unit);

	GLuint getHandle() const;

private:
	GLuint m_id;

	GLenum m_target;

	GLenum m_internalFormat;
	GLenum m_format;
	GLenum m_type;

	GLenum m_minFilter;
	GLenum m_maxFilter;

	GLenum m_wrapS;
	GLenum m_wrapT;
	GLenum m_wrapR;

	ivec3 m_size;

	bool m_initialized;
};