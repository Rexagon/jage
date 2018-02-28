#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "Shader.h"

class Grid : public GameObject
{
public:
	Grid(const std::string& name);
	virtual ~Grid();

	void onDraw() override;

private:
	Shader* m_gridShader;

	GLuint m_VAO;
	GLuint m_VBO;

	unsigned int m_vertexCount;
};