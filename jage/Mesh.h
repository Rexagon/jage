#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>

#include "Math.h"

class Mesh
{
public:
	Mesh();
	Mesh(const std::string& name);
	~Mesh();

	void init(const std::vector<vec3>& positions, const std::vector<unsigned int>& indices);

	void init(const std::vector<vec3>& positions, const std::vector<vec2>& textureCoords,
		const std::vector<unsigned int>& indices);

	void init(const std::vector<vec3>& positions, const std::vector<vec2>& textureCoords,
		const std::vector<vec3>& normals, const std::vector<unsigned int>& indices);

	void draw();

	std::string getName() const;

	GLuint getVAO() const;

	GLuint getPositionsBuffer() const;
	unsigned int getPositionsNum() const;

	unsigned int getIndicesNum() const;

private:
	template<glm::length_t L, class T>
	void initAttribute(int number, const std::vector<glm::vec<L, T>>& data) {
		GLuint vbo;
		glGenBuffers(1, &vbo);
		m_buffers.push_back(vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T) * L, &data[0][0], GL_STATIC_DRAW);

		glVertexAttribPointer(number, L, GL_FLOAT, false, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void initIndices(const std::vector<unsigned int>& indices);

	std::string m_name;

	GLuint m_VAO;
	std::vector<GLuint> m_buffers;

	unsigned int m_indicesNum;
	unsigned int m_positionsNum;
};