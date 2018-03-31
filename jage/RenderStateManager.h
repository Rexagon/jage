#pragma once

#include "Shader.h"

class RenderStateManager
{
public:
	static void init();
	static void forceApply();

	static void setViewport(const ivec2& size);
	static void setViewport(const ivec2& offset, const ivec2& size);
	static void setViewport(GLint x, GLint y, GLsizei width, GLsizei height);
	static ivec2 getViewportOffset();
	static ivec2 getViewportSize();

	static void setClearColor(const vec4& color);
	static void setClearColor(const vec3& color);
	static void setClearColor(float r, float g, float b, float a = 1.0f);
	static vec4 getClearColor();

	static void setClearDepth(float depth);
	static float getClearDepth();

	static void setDepthTestEnabled(bool enabled);
	static bool isDepthTestEnabled();

	static void setDepthWriteEnabled(bool enabled);
	static bool isDepthWriteEnabled();

	static void setDepthTestFunction(GLenum depthTestFunction);
	static GLenum getDepthTestFunction();

	static void setClipControl(GLenum origin, GLenum depth);
	static GLenum getClipControlOrigin();
	static GLenum getClipControlDepth();

	static void setBlendingEnabled(bool enabled);
	static bool isBlendingEnabled();

	static void setBlendingFunction(GLenum src, GLenum dst);
	static GLenum getBlendingFunctionSrc();
	static GLenum getBlendingFunctionDst();
	
	static void setFaceCullingEnabled(bool enabled);
	static bool isFaceCullingEnabled();

	static void setFaceCullingSide(GLenum side);
	static GLenum getFaceCullingSide();

	static void setPolygonMode(GLenum mode);
	static GLenum getPolygonMode();

	static void setCurrentShader(const Shader* shader);
	static const Shader* getCurrentShader();

private:
	static GLint m_viewport[4];

	static GLclampf m_clearColor[4];

	static float m_clearDepth;

	static bool m_depthTestEnabled;
	static bool m_depthWriteEnabled;
	static GLenum m_depthTestFunction;

	static GLenum m_clipControlOrigin;
	static GLenum m_clipControlDepth;

	static bool m_blendingEnabled;
	static GLenum m_blendingFunctionSrc;
	static GLenum m_blendingFunctionDst;

	static bool m_faceCullingEnabled;
	static GLenum m_faceCullingSide;

	static GLenum m_polygonMode;

	static const Shader* m_currentShader;
};