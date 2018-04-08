#include "Material.h"

#include "RenderStateManager.h"

Material::Material(Type type, const std::type_index& classInfo) :
	m_shader(nullptr), m_type(type),
	m_depthTestEnabled(true), m_depthWriteEnabled(true), m_depthTestFunction(GL_GEQUAL),
	m_faceCullingEnabled(true), m_faceCullingSide(GL_BACK),
	m_blendingEnabled(false), m_blendingFunctionSrc(GL_SRC_ALPHA), m_blendingFunctionDst(GL_ONE_MINUS_SRC_ALPHA),
	m_shadowCastingEnabled(true), m_shadowReceivingEnabled(true),
	m_classInfo(classInfo)
{
}

Material::~Material()
{
}

Shader * Material::getShader() const
{
	return m_shader;
}

Material::Type Material::getType() const
{
	return m_type;
}

std::vector<Texture*>& Material::getTextures()
{
	return m_textures;
}

const std::vector<Texture*>& Material::getTextures() const
{
	return m_textures;
}

void Material::setDepthTestEnabled(bool enabled)
{
	m_depthTestEnabled = enabled;
}

bool Material::isDepthTestEnabled() const
{
	return m_depthTestEnabled;
}

void Material::setDepthWriteEnabled(bool enabled)
{
	m_depthWriteEnabled = enabled;
}

bool Material::isDepthWriteEnabled() const
{
	return m_depthWriteEnabled;
}

void Material::setDepthTestFunction(GLenum depthTestFunction)
{
	m_depthTestFunction = depthTestFunction;
}

GLenum Material::getDepthTestFunction() const
{
	return m_depthTestFunction;
}

void Material::setFaceCullingEnabled(bool enabled)
{
	m_faceCullingEnabled = enabled;
}

bool Material::isFaceCullingEnabled() const
{
	return m_faceCullingEnabled;
}

void Material::setFaceCullingSide(GLenum side)
{
	m_faceCullingSide = side;
}

GLenum Material::getFaceCullingSide() const
{
	return m_faceCullingSide;
}

void Material::setBlendingEnabled(bool enabled)
{
	m_blendingEnabled = enabled;
}

bool Material::isBlendingEnabled() const
{
	return m_blendingEnabled;
}

void Material::setBlendingFunction(GLenum src, GLenum dst)
{
	m_blendingFunctionSrc = src;
	m_blendingFunctionDst = dst;
}

GLenum Material::getBlendingFunctionSrc() const
{
	return m_blendingFunctionSrc;
}

GLenum Material::getBlendingFunctionDst() const
{
	return m_blendingFunctionDst;
}

void Material::setShadowCastingEnabled(bool enabled)
{
	m_shadowCastingEnabled = enabled;
}

bool Material::isShadowCastingEnabled() const
{
	return m_shadowCastingEnabled;
}

void Material::setShadowReceivingEnabled(bool enabled)
{
	m_shadowReceivingEnabled = enabled;
}

bool Material::isShadowReceivingEnabled() const
{
	return m_shadowReceivingEnabled;
}

std::type_index Material::getClassInfo() const
{
	return m_classInfo;
}
