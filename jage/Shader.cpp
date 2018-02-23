#include "Shader.h"

#include "Log.h"

Shader::Shader()
{
	m_id = glCreateProgram();
}

Shader::~Shader()
{
	glDeleteProgram(m_id);

	for (auto& program : m_programs) {
		glDeleteShader(program);
	}
}

bool Shader::attachPart(const std::string & source, GLenum type, std::string& infoLog)
{
	GLint program = glCreateShader(type);

	const GLchar* data = source.data();
	const GLint size = source.size();
	glShaderSource(program, 1, &data, &size);

	glCompileShader(program);

	GLint compilationStatus;
	glGetShaderiv(program, GL_COMPILE_STATUS, &compilationStatus);
	if (compilationStatus == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		infoLog.resize(infoLogLength);
		glGetShaderInfoLog(program, infoLogLength, NULL, &infoLog[0]);

		glDeleteShader(type);

		return false;
	}
	else {
		m_programs.push_back(program);
	}

	glAttachShader(m_id, program);

	return true;
}

bool Shader::link(std::string& infoLog)
{
	glLinkProgram(m_id);

	GLint linkStatus;
	glGetProgramiv(m_id, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE) {
		GLint infoLogLength;
		glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);

		infoLog.resize(infoLogLength);
		glGetProgramInfoLog(m_id, infoLogLength, NULL, &infoLog[0]);

		return false;
	}

	return true;
}

void Shader::bind()
{
	glUseProgram(m_id);
}

void Shader::unbind()
{
	glUseProgram(0);
}

void Shader::setAttribute(unsigned int index, const std::string & name)
{
	glBindAttribLocation(m_id, index, name.c_str());
}

void Shader::setUniform(const std::string & name, int data)
{
	glUniform1i(getUniformLocation(name), data);
}

void Shader::setUniform(const std::string & name, float data)
{
	glUniform1f(getUniformLocation(name), data);
}

void Shader::setUniform(const std::string & name, const vec2 & data)
{
	glUniform2f(getUniformLocation(name), data.x, data.y);
}

void Shader::setUniform(const std::string & name, const ivec2 & data)
{
	glUniform2i(getUniformLocation(name), data.x, data.y);
}

void Shader::setUniform(const std::string & name, const vec3 & data)
{
	glUniform3f(getUniformLocation(name), data.x, data.y, data.z);
}

void Shader::setUniform(const std::string & name, const ivec3 & data)
{
	glUniform3i(getUniformLocation(name), data.x, data.y, data.z);
}

void Shader::setUniform(const std::string & name, const vec4 & data)
{
	glUniform4f(getUniformLocation(name), data.x, data.y, data.z, data.w);
}

void Shader::setUniform(const std::string & name, const ivec4 & data)
{
	glUniform4i(getUniformLocation(name), data.x, data.y, data.z, data.w);
}

void Shader::setUniform(const std::string & name, const mat4 & data)
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &data[0][0]);
}

void Shader::setUniformArray(const std::string & name, int * data, int size)
{
	glUniform1iv(getUniformLocation(name), size, data);
}

void Shader::setUniformArray(const std::string & name, float * data, int size)
{
	glUniform1fv(getUniformLocation(name), size, data);
}

void Shader::setUniformArray(const std::string & name, vec2 * data, int size)
{
	glUniform2fv(getUniformLocation(name), size, &data[0][0]);
}

void Shader::setUniformArray(const std::string & name, ivec2 * data, int size)
{
	glUniform2iv(getUniformLocation(name), size, &data[0][0]);
}

void Shader::setUniformArray(const std::string & name, vec3 * data, int size)
{
	glUniform3fv(getUniformLocation(name), size, &data[0][0]);
}

void Shader::setUniformArray(const std::string & name, ivec3 * data, int size)
{
	glUniform3iv(getUniformLocation(name), size, &data[0][0]);
}

void Shader::setUniformArray(const std::string & name, vec4 * data, int size)
{
	glUniform4fv(getUniformLocation(name), size, &data[0][0]);
}

void Shader::setUniformArray(const std::string & name, ivec4 * data, int size)
{
	glUniform4iv(getUniformLocation(name), size, &data[0][0]);
}

void Shader::setUniformArray(const std::string & name, mat4 * data, int size)
{
	glUniformMatrix4fv(getUniformLocation(name), size, GL_FALSE, &data[0][0][0]);
}

unsigned int Shader::getUniformLocation(const std::string & name)
{
	auto it = m_uniformLocations.find(name);
	if (it == m_uniformLocations.end()) {
		GLuint location = glGetUniformLocation(m_id, name.c_str());
		m_uniformLocations[name] = location;
		return location;
	}
	else {
		return it->second;
	}
}
