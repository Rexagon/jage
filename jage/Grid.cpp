#include "Grid.h"

#include "RenderStateManager.h"

Grid::Grid(unsigned int size) :
	m_size(static_cast<int>(size))
{
	float chunkSize = 20.0f;

	std::vector<vec3> gridVertices;
	std::vector<ivec2> chunkIds;

	for (int i = -m_size; i <= m_size; ++i) {
		for (int j = -m_size; j <= m_size; ++j) {
			chunkIds.push_back(vec2(i, j));
			gridVertices.push_back(chunkSize * vec3(i, 0.0f, j));
		}
	}

	m_vertexCount = static_cast<unsigned int>(gridVertices.size());

	size_t positionsBufferSize = sizeof(vec3) * gridVertices.size();
	size_t chunkIdsBufferSize = sizeof(vec2) * chunkIds.size();

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	std::vector<char> data(positionsBufferSize + chunkIdsBufferSize);	
	std::memcpy(&data[0], gridVertices.data(), positionsBufferSize);
	std::memcpy(&data[0] + positionsBufferSize, chunkIds.data(), chunkIdsBufferSize);

	glBufferData(GL_ARRAY_BUFFER, data.size(), data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glVertexAttribIPointer(1, 2, GL_INT, 0, reinterpret_cast<GLvoid*>(positionsBufferSize));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

Grid::~Grid()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

void Grid::draw()
{
	RenderStateManager::setBlendingEnabled(true);
	RenderStateManager::setBlendingFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray(m_VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glDrawArrays(GL_POINTS, 0, m_vertexCount);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	RenderStateManager::setBlendingEnabled(false);
}
