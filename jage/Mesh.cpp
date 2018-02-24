#include "Mesh.h"

#include "Log.h"

Mesh::Mesh()
{
	glGenVertexArrays(1, &m_VAO);
}

Mesh::Mesh(const std::string & name) :
	m_name(name)
{
	glGenVertexArrays(1, &m_VAO);
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(static_cast<int>(m_buffers.size()), &m_buffers[0]);
}

void Mesh::init(const std::vector<vec3>& positions, const std::vector<unsigned int>& indices)
{
	glBindVertexArray(m_VAO);

	m_positionsNum = static_cast<unsigned int>(positions.size());
	initAttribute(0, positions);
	initIndices(indices);

	glBindVertexArray(0);
}

void Mesh::init(const std::vector<vec3>& positions, const std::vector<vec2>& textureCoords,
	const std::vector<unsigned int>& indices)
{
	glBindVertexArray(m_VAO);

	m_positionsNum = static_cast<unsigned int>(positions.size());
	initAttribute(0, positions);
	initAttribute(1, textureCoords);
	initIndices(indices);

	glBindVertexArray(0);
}

void Mesh::init(const std::vector<vec3>& positions, const std::vector<vec2>& textureCoords,
	const std::vector<vec3>& normals, const std::vector<unsigned int>& indices)
{
	glBindVertexArray(m_VAO);

	m_positionsNum = static_cast<unsigned int>(positions.size());
	initAttribute(0, positions);
	initAttribute(1, textureCoords);
	initAttribute(2, normals);
	initIndices(indices);

	glBindVertexArray(0);
}

void Mesh::draw() const
{
	glBindVertexArray(m_VAO);

	for (int i = 0; i < m_buffers.size(); ++i) {
		glEnableVertexAttribArray(i);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.back());
	glDrawElements(GL_TRIANGLES, m_indicesNum, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	for (int i = 0; i < m_buffers.size() - 1; ++i) {
		glDisableVertexAttribArray(i);
	}

	glBindVertexArray(0);
}

void Mesh::setName(const std::string & name)
{
	m_name = name;
}

std::string Mesh::getName() const
{
	return m_name;
}

GLuint Mesh::getVAO() const
{
	return m_VAO;
}

GLuint Mesh::getPositionsBuffer() const
{
	return m_buffers[0];
}

unsigned int Mesh::getPositionsNum() const
{
	return m_positionsNum;
}

unsigned int Mesh::getIndicesNum() const
{
	return m_indicesNum;
}

void Mesh::initIndices(const std::vector<unsigned int>& indices)
{
	GLuint ebo;
	glGenBuffers(1, &ebo);
	m_buffers.push_back(ebo);

	m_indicesNum = static_cast<unsigned int>(indices.size());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indicesNum, &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}