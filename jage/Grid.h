#pragma once

#include "GameObject.h"
#include "Shader.h"

class Grid
{
public:
	Grid(unsigned int size);
	virtual ~Grid();

	void draw();

private:
	GLuint m_VAO;
	GLuint m_VBO;

	int m_size;
	unsigned int m_vertexCount;
};