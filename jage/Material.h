#pragma once

#include <typeindex>

#include <GL/glew.h>

#include "Shader.h"
#include "Texture.h"

class Material
{
public:
	enum Type
	{
		DEFERRED,
		FORWARD,
		POST_PROCESS,
	};

	Material(Type type, const std::type_index& classInfo);
	virtual ~Material();

	virtual void bind() = 0;

	Shader* getShader() const;

	Type getType() const;

	std::vector<Texture*>& getTextures();
	const std::vector<Texture*>& getTextures() const;

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

	std::type_index getClassInfo() const;

	template<typename T>
	bool is()
	{
		return std::type_index(typeid(T)) == m_classInfo;
	}

	template<typename T>
	T* as()
	{
		return dynamic_cast<T*>(this);
	}

protected:
	Shader* m_shader;

	Type m_type;

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

	std::type_index m_classInfo;
};