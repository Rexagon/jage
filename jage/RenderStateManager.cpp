#include "RenderStateManager.h"

GLint RenderStateManager::m_viewport[4] = {};

GLclampf RenderStateManager::m_clearColor[4] = {};

bool RenderStateManager::m_depthTestEnabled = false;
bool RenderStateManager::m_depthWriteEnabled = true;
GLenum RenderStateManager::m_depthTestFunction = GL_LESS;

GLenum RenderStateManager::m_clipControlOrigin = GL_LOWER_LEFT;
GLenum RenderStateManager::m_clipControlDepth = GL_NEGATIVE_ONE_TO_ONE;

bool RenderStateManager::m_blendingEnabled = false;
GLenum RenderStateManager::m_blendingFunctionSrc = GL_SRC_ALPHA;
GLenum RenderStateManager::m_blendingFunctionDst = GL_ONE_MINUS_SRC_ALPHA;

bool RenderStateManager::m_faceCullingEnabled = false;
GLenum RenderStateManager::m_faceCullingSide = GL_NONE;

GLenum RenderStateManager::m_polygonMode = GL_FILL;

const Shader* RenderStateManager::m_currentShader = nullptr;

void RenderStateManager::init()
{
	glGetIntegerv(GL_VIEWPORT, m_viewport);

	glGetFloatv(GL_COLOR_CLEAR_VALUE, m_clearColor);

	m_depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
	m_depthWriteEnabled = glIsEnabled(GL_DEPTH_WRITEMASK);
	glGetIntegerv(GL_DEPTH_FUNC, reinterpret_cast<GLint*>(&m_depthTestFunction));

	m_clipControlOrigin = GL_LOWER_LEFT;
	m_clipControlDepth = GL_ZERO_TO_ONE;
	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

	m_blendingEnabled = glIsEnabled(GL_BLEND);
	glGetIntegerv(GL_BLEND_SRC_ALPHA, reinterpret_cast<GLint*>(&m_blendingFunctionSrc));
	glGetIntegerv(GL_BLEND_DST_ALPHA, reinterpret_cast<GLint*>(&m_blendingFunctionDst));

	m_faceCullingEnabled = glIsEnabled(GL_CULL_FACE);
	glGetIntegerv(GL_CULL_FACE_MODE, reinterpret_cast<GLint*>(&m_faceCullingSide));

	glGetIntegerv(GL_POLYGON_MODE, reinterpret_cast<GLint*>(&m_polygonMode));

	m_currentShader = nullptr;
	glUseProgram(0);
}

void RenderStateManager::forceApply()
{
	// vieport
	glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);

	// clear color
	glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);

	// depth test
	if (m_depthTestEnabled) {
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}

	// depth write
	if (m_depthWriteEnabled) {
		glDepthMask(GL_TRUE);
	}
	else {
		glDepthMask(GL_FALSE);
	}

	// depth function
	glDepthFunc(m_depthTestFunction);

	// clip control
	glClipControl(m_clipControlOrigin, m_clipControlDepth);

	// blending
	if (m_blendingEnabled) {
		glEnable(GL_BLEND);
	}
	else {
		glDisable(GL_BLEND);
	}
	glBlendFunc(m_blendingFunctionSrc, m_blendingFunctionDst);

	// face culling
	if (m_faceCullingEnabled) {
		glEnable(GL_CULL_FACE);
	}
	else {
		glDisable(GL_CULL_FACE);
	}
	glCullFace(m_faceCullingSide);

	// polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, m_polygonMode);

	// shader
	if (m_currentShader == nullptr) {
		glUseProgram(0);
	}
	else {
		glUseProgram(m_currentShader->getHandle());
	}
}

void RenderStateManager::setViewport(const ivec2 & size)
{
	if (m_viewport[2] != size.x || m_viewport[3] != size.y) {
		m_viewport[2] = size.x;
		m_viewport[3] = size.y;
		glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
	}
}

void RenderStateManager::setViewport(const ivec2 & offset, const ivec2 & size)
{
	if (m_viewport[0] != offset.x || m_viewport[1] != offset.y || 
		m_viewport[2] != size.x || m_viewport[3] != size.y)
	{
		m_viewport[0] = offset.x;
		m_viewport[1] = offset.y;
		m_viewport[2] = size.x;
		m_viewport[3] = size.y;
		glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
	}
}

void RenderStateManager::setViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	if (m_viewport[0] != x || m_viewport[1] != y ||
		m_viewport[2] != width || m_viewport[3] != height)
	{
		m_viewport[0] = x;
		m_viewport[1] = y;
		m_viewport[2] = width;
		m_viewport[3] = height;
		glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
	}
}

ivec2 RenderStateManager::getViewportOffset()
{
	return ivec2(m_viewport[0], m_viewport[1]);
}

ivec2 RenderStateManager::getViewportSize()
{
	return ivec2(m_viewport[2], m_viewport[3]);
}

void RenderStateManager::setClearColor(const vec4 & color)
{
	if (m_clearColor[0] != color.r || m_clearColor[1] != color.g || m_clearColor[2] != color.b ||
		m_clearColor[3] != color.a)
	{
		m_clearColor[0] = color.r;
		m_clearColor[1] = color.g;
		m_clearColor[2] = color.b;
		m_clearColor[3] = color.a;
		glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
	}
}

void RenderStateManager::setClearColor(const vec3 & color)
{
	if (m_clearColor[0] != color.r || m_clearColor[1] != color.g || m_clearColor[2] != color.b)
	{
		m_clearColor[0] = color.r;
		m_clearColor[1] = color.g;
		m_clearColor[2] = color.b;
		glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
	}
}

void RenderStateManager::setClearColor(float r, float g, float b, float a)
{
	if (m_clearColor[0] != r || m_clearColor[1] != g || m_clearColor[2] != b || m_clearColor[3] != a)
	{
		m_clearColor[0] = r;
		m_clearColor[1] = g;
		m_clearColor[2] = b;
		m_clearColor[3] = a;
		glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
	}
}

vec4 RenderStateManager::getClearColor()
{
	return vec4(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
}

void RenderStateManager::setDepthTestEnabled(bool enabled)
{
	if (m_depthTestEnabled != enabled) {
		m_depthTestEnabled = enabled;
		if (enabled) {
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}
	}
}

bool RenderStateManager::isDepthTestEnabled()
{
	return m_depthTestEnabled;
}

void RenderStateManager::setDepthWriteEnabled(bool enabled)
{
	if (m_depthWriteEnabled != enabled) {
		m_depthWriteEnabled = enabled;
		if (enabled) {
			glDepthMask(GL_TRUE);
		}
		else {
			glDepthMask(GL_FALSE);
		}
	}
}

bool RenderStateManager::isDepthWriteEnabled()
{
	return m_depthWriteEnabled;
}

void RenderStateManager::setDepthTestFunction(GLenum depthTestFunction)
{
	if (m_depthTestFunction != depthTestFunction) {
		m_depthTestFunction = depthTestFunction;
		glDepthFunc(depthTestFunction);
	}
}

GLenum RenderStateManager::getDepthTestFunction()
{
	return m_depthTestFunction;
}

void RenderStateManager::setClipControl(GLenum origin, GLenum depth)
{
	if (m_clipControlOrigin != origin || m_clipControlDepth != depth) {
		m_clipControlOrigin = origin;
		m_clipControlDepth = depth;
		glClipControl(origin, depth);
	}
}

GLenum RenderStateManager::getClipControlOrigin()
{
	return m_clipControlOrigin;
}

GLenum RenderStateManager::getClipControlDepth()
{
	return m_clipControlDepth;
}

void RenderStateManager::setBlendingEnabled(bool enabled)
{
	if (m_blendingEnabled != enabled) {
		m_blendingEnabled = enabled;
		if (enabled) {
			glEnable(GL_BLEND);
		}
		else {
			glDisable(GL_BLEND);
		}
	}
}

bool RenderStateManager::isBlendingEnabled()
{
	return m_blendingEnabled;
}

void RenderStateManager::setBlendingFunction(GLenum src, GLenum dst)
{
	if (m_blendingFunctionSrc != src || m_blendingFunctionDst != dst) {
		m_blendingFunctionSrc = src;
		m_blendingFunctionDst = dst;
		glBlendFunc(src, dst);
	}
}

GLenum RenderStateManager::getBlendingFunctionSrc()
{
	return m_blendingFunctionSrc;
}

GLenum RenderStateManager::getBlendingFunctionDst()
{
	return m_blendingFunctionDst;
}

void RenderStateManager::setFaceCullingEnabled(bool enabled)
{
	if (m_faceCullingEnabled != enabled) {
		m_faceCullingEnabled = enabled;
		if (enabled) {
			glEnable(GL_CULL_FACE);
		}
		else {
			glDisable(GL_CULL_FACE);
		}
	}
}

bool RenderStateManager::isFaceCullingEnabled()
{
	return m_faceCullingEnabled;
}

void RenderStateManager::setFaceCullingSide(GLenum side)
{
	if (m_faceCullingSide != side) {
		m_faceCullingSide = side;
		glCullFace(side);
	}
}

GLenum RenderStateManager::getFaceCullingSide()
{
	return m_faceCullingSide;
}

void RenderStateManager::setPolygonMode(GLenum mode)
{
	if (m_polygonMode != mode) {
		m_polygonMode = mode;
		glPolygonMode(GL_FRONT_AND_BACK, mode);
	}
}

GLenum RenderStateManager::getPolygonMode()
{
	return m_polygonMode;
}

void RenderStateManager::setCurrentShader(const Shader * shader)
{
	if (m_currentShader != shader) {
		m_currentShader = shader;

		if (shader == nullptr) {
			glUseProgram(0);
		}
		else {
			glUseProgram(shader->getHandle());
		}
	}
}

const Shader* RenderStateManager::getCurrentShader()
{
	return m_currentShader;
}
