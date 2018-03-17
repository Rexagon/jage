#pragma once

#include <GL/glew.h>

#include "Shader.h"
#include "Texture.h"

class Material
{
public:
	enum Type
	{
		DEFAULT,
		CUSTOM,
		POST_PROCESS
	};

	Material(Shader* shader);

	Shader* getShader() const;

	void setType(Type type);
	Type getType() const;
	
	void setColor(const vec4& color);
	void setColor(const vec3& color);
	void setColor(float r, float g, float b, float a = 1.0f);
	vec4 getColor() const;

	std::vector<Texture*>& getTextures();

	void setDepthTestEnabled(bool enabled);
	bool isDepthTestEnabled() const;

	void setDepthWriteEnabled(bool enabled);
	bool isDepthWriteEnabled() const;

	void setDepthTestFunction(GLenum depthTestFunction);
	GLenum getDepthTestFunction() const;

	void setFaceCullingEnabled(bool enabled);
	bool isFaceCullingEnabled() const;

	void setFaceCullingSide(GLenum side);
	GLenum getFaceCullingSide() const;

	void setBlendingEnabled(bool enabled);
	bool isBlendingEnabled() const;

	void setBlendingFunction(GLenum src, GLenum dst);
	GLenum getBlendingFunctionSrc() const;
	GLenum getBlendingFunctionDst() const;

	void setShadowCastingEnabled(bool enabled);
	bool isShadowCastingEnabled() const;

	void setShadowReceivingEnabled(bool enabled);
	bool isShadowReceivingEnabled() const;

private:
	Shader* m_shader;

	Type m_type;

	vec4 m_color;

	std::vector<Texture*> m_textures;

	bool m_depthTestEnabled;
	bool m_depthWriteEnabled;
	GLenum m_depthTestFunction;

	bool m_faceCullingEnabled;
	GLenum m_faceCullingSide;

	bool m_blendingEnabled;
	GLenum m_blendingFunctionSrc;
	GLenum m_blendingFunctionDst;

	bool m_shadowCastingEnabled;
	bool m_shadowReceivingEnabled;
};