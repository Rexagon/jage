#include "FxaaMaterial.h"

#include "ShaderFactory.h"
#include "ResourceManager.h"

FxaaMaterial::FxaaMaterial() :
	Material(POST_PROCESS, typeid(FxaaMaterial))
{
	ShaderFactory::FromFile fxaaVertexSource("shaders/quad.vert");
	ShaderFactory::FromFile fxaaFragmentSource("shaders/fxaa.frag");
	ResourceManager::bind<ShaderFactory>("fxaa_shader", fxaaVertexSource, fxaaFragmentSource);
	m_shader = ResourceManager::get<Shader>("fxaa_shader");
	m_shader->setAttribute(0, "position");
	m_shader->setAttribute(1, "texCoord");
}

void FxaaMaterial::bind()
{
	m_shader->bind();
}
