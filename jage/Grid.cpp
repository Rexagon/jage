#include "Grid.h"

Grid::Grid(const std::string & name) :
	GameObject(name)
{
	std::vector<vec3> gridVertices = { 
		vec3(0.0f, 0.0f, 0.0f) 
	};
	m_vertexCount = static_cast<unsigned int>(gridVertices.size());

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * gridVertices.size(), gridVertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Grid::~Grid()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

void Grid::onDraw()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(m_VAO);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_POINTS, 0, 1);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	glDisable(GL_BLEND);
}
