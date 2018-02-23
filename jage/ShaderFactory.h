#pragma once

#include <memory>

#include "AbstractFactory.h"

#include "Shader.h"

class ShaderFactory : public AbstractFactory
{
private:
	struct ShaderSource
	{
		enum Type { FILE, STRING, NONE };

		ShaderSource() :
			type(Type::NONE)
		{}

		ShaderSource(Type type, const std::string& source) : 
			type(type), source(source) 
		{}

		Type type;
		std::string source;
	};
public:
	struct FromFile : ShaderSource
	{
		FromFile(const std::string& source) : ShaderSource(ShaderSource::FILE, source) {}
	};
	
	struct FromString : ShaderSource
	{
		FromString(const std::string& source) : ShaderSource(ShaderSource::STRING, source) {}
	};

	// Loads shader from vertex
	ShaderFactory(const ShaderSource& vertexShaderSource);

	// Loads shader from vertex and fragment
	ShaderFactory(const ShaderSource& vertexShaderSource, const ShaderSource& fragmentShaderSource);

	// Loads shader from vertex, geometry and fragment
	ShaderFactory(const ShaderSource& vertexShaderSource, const ShaderSource& geometryShaderSource, 
		const ShaderSource& fragmentShaderSource);

	void* load() override;
	void clear() override;

private:
	ShaderSource m_vertexShaderSource;
	ShaderSource m_geometryShaderSource;
	ShaderSource m_fragmentShaderSource;

	std::unique_ptr<Shader> m_data;
};
