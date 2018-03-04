#include "Camera.h"

#include "Log.h"

Camera::Camera(const std::string& name) :
	GameObject(name),
	m_depthRange(0.1f, 100.0f), m_viewMatrix(1.0f), m_globalTransformation(1.0f), 
	m_viewProjectionChanged(true), m_projectionChanged(true)
{
}

Camera::Camera(const std::string& name, float minDepth, float maxDepth) :
	GameObject(name),
	m_depthRange(minDepth, maxDepth), m_projectionChanged(true)
{
}

Camera::Camera(const std::string& name, const vec2& zRange) :
	GameObject(name),
	m_depthRange(zRange)
{
}

mat4 Camera::getViewProjectionMatrix() const
{
	updateView();
	updateProjection();

	if (m_viewProjectionChanged) {
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
		m_viewProjectionChanged = false;
	}

	return m_viewProjectionMatrix;
}

mat4 Camera::getViewMatrix() const
{
	updateView();

	return m_viewMatrix;
}

mat4 Camera::getProjectionMatrix() const
{
	updateProjection();

	return m_projectionMatrix;
}

void Camera::setMinDepth(float minDepth)
{
	m_depthRange.x = minDepth;
	m_projectionChanged = true;
}

float Camera::getMinDepth() const
{
	return m_depthRange.x;
}

void Camera::setMaxDepth(float maxDepth)
{
	m_depthRange.y = maxDepth;
	m_projectionChanged = true;
}

float Camera::getMaxDepth() const
{
	return m_depthRange.y;
}

void Camera::setDepthRange(const vec2 & depthRange)
{
	m_depthRange = depthRange;
	m_projectionChanged = true;
}

vec2 Camera::getDepthRange()
{
	return m_depthRange;
}

void Camera::updateView() const
{
	mat4 globalTransfromation = getGlobalTransformation();
	if (globalTransfromation != m_globalTransformation) {
		m_viewMatrix = glm::inverse(globalTransfromation);

		m_globalTransformation = globalTransfromation;
		m_viewProjectionChanged = true;
	}
}


//
PerspectiveCamera::PerspectiveCamera(const std::string& name, float fov, float aspect) :
	Camera(name),
	m_fov(fov), m_aspect(aspect)
{
}

PerspectiveCamera::PerspectiveCamera(const std::string& name, float fov, float aspect, float minDepth, float maxDepth) :
	Camera(name, minDepth, maxDepth),
	m_fov(fov), m_aspect(aspect)
{
}

void PerspectiveCamera::setFieldOfView(float fov)
{
	m_fov = fov;
	m_projectionChanged = true;
}

float PerspectiveCamera::getFieldOfView() const
{
	return m_fov;
}

void PerspectiveCamera::setAspect(float aspect)
{
	m_aspect = aspect;
	m_projectionChanged = true;
}

float PerspectiveCamera::getAspect() const
{
	return m_aspect;
}

void PerspectiveCamera::updateProjection() const
{
	if (m_projectionChanged) {
		float f = 1.0f / tan(m_fov / 2.0f);
		
		m_projectionMatrix = mat4(
			f / m_aspect, 0.0f, 0.0f, 0.0f,
			0.0f, f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, m_depthRange.x, 0.0f);
		//m_projectionMatrix = glm::perspective(m_fov, m_aspect, m_depthRange.x, m_depthRange.y);

		m_projectionChanged = false;
		m_viewProjectionChanged = true;
	}

}


//
IsometricCamera::IsometricCamera(const std::string& name, float leftAndRight, float bottomAndTop) :
	Camera(name),
	m_left(-leftAndRight), m_right(leftAndRight), m_bottom(-bottomAndTop), m_top(bottomAndTop)
{
}

IsometricCamera::IsometricCamera(const std::string& name, float left, float right, float bottom, float top) :
	Camera(name),
	m_left(left), m_right(right), m_bottom(bottom), m_top(top)
{
}

IsometricCamera::IsometricCamera(const std::string& name, float left, float right, float bottom, float top, float minDepth, float maxDepth) :
	Camera(name, minDepth, maxDepth),
	m_left(left), m_right(right), m_bottom(bottom), m_top(top)
{
}

void IsometricCamera::setDimensions(float leftAndRight, float bottomAndTop)
{
	m_left = -leftAndRight;
	m_right = leftAndRight;
	m_bottom = -bottomAndTop;
	m_top = bottomAndTop;
	m_projectionChanged = true;
}

void IsometricCamera::setDimensions(float left, float right, float bottom, float top)
{
	m_left = left;
	m_right = right;
	m_bottom = bottom;
	m_top = top;
	m_projectionChanged = true;
}

float IsometricCamera::getLeftDimension() const
{
	return m_left;
}

float IsometricCamera::getRightDimension() const
{
	return m_right;
}

float IsometricCamera::getBottomDimension() const
{
	return m_bottom;
}

float IsometricCamera::getTopDimension() const
{
	return m_top;
}

void IsometricCamera::updateProjection() const
{
	if (m_projectionChanged) {
		m_projectionMatrix = glm::ortho(m_left, m_right, m_bottom, m_top, m_depthRange.x, m_depthRange.y);

		m_projectionChanged = false;
		m_viewProjectionChanged = true;
	}
}