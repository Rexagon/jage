#include "ShaderFactory.h"

#include "FileManager.h"

ShaderFactory::ShaderFactory(const ShaderSource & vertexShaderSource) :
	AbstractFactory(tag<Shader>{}),
	m_vertexShaderSource(vertexShaderSource),
	m_data(nullptr)
{
}

ShaderFactory::ShaderFactory(const ShaderSource & vertexShaderSource, const ShaderSource & fragmentShaderSource) :
	AbstractFactory(tag<Shader>{}),
	m_vertexShaderSource(vertexShaderSource),
	m_fragmentShaderSource(fragmentShaderSource),
	m_data(nullptr)
{
}

ShaderFactory::ShaderFactory(const ShaderSource & vertexShaderSource, const ShaderSource & geometryShaderSource, const ShaderSource & fragmentShaderSource) :
	AbstractFactory(tag<Shader>{}),
	m_vertexShaderSource(vertexShaderSource),
	m_geometryShaderSource(geometryShaderSource),
	m_fragmentShaderSource(fragmentShaderSource),
	m_data(nullptr)
{
}

void * ShaderFactory::load()
{
	if (m_data == nullptr) {
		std::unique_ptr<Shader> shader = std::make_unique<Shader>();

		std::string infoLog;
		
		if (m_vertexShaderSource.type != ShaderSource::NONE) {
			std::string shaderSource;
			if (m_vertexShaderSource.type == ShaderSource::FILE) {
				shaderSource = FileManager::open(m_vertexShaderSource.source);
			}
			else {
				shaderSource = m_vertexShaderSource.source;
			}

			if (!shader->attachPart(shaderSource, GL_VERTEX_SHADER, infoLog)) {
				throw std::runtime_error("Unable to load vertex shader: " + m_assignedName + ". " + infoLog);
			}
		}

		if (m_geometryShaderSource.type != ShaderSource::NONE) {
			std::string shaderSource;
			if (m_geometryShaderSource.type == ShaderSource::FILE) {
				shaderSource = FileManager::open(m_geometryShaderSource.source);
			}
			else {
				shaderSource = m_geometryShaderSource.source;
			}

			if (!shader->attachPart(shaderSource, GL_GEOMETRY_SHADER, infoLog)) {
				throw std::runtime_error("Unable to load fragment shader: " + m_assignedName + ". " + infoLog);
			}
		}

		if (m_fragmentShaderSource.type != ShaderSource::NONE) {
			std::string shaderSource;
			if (m_fragmentShaderSource.type == ShaderSource::FILE) {
				shaderSource = FileManager::open(m_fragmentShaderSource.source);
			}
			else {
				shaderSource = m_fragmentShaderSource.source;
			}

			if (!shader->attachPart(shaderSource, GL_FRAGMENT_SHADER, infoLog)) {
				throw std::runtime_error("Unable to load fragment shader: " + m_assignedName + ". " + infoLog);
			}
		}

		if (!shader->link(infoLog)) {
			throw std::runtime_error("Unable to link shader: " + m_assignedName + ". " + infoLog);
		}

		m_data = std::move(shader);
	}

	return m_data.get();
}

void ShaderFactory::clear()
{
	m_data.reset();
}
