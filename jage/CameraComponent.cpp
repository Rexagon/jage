#include "CameraComponent.h"

CameraComponent::CameraComponent() :
	m_depthRange(0.1f, 100.0f), m_projectionChanged(true), m_projectionType(PERSPECTIVE),
	m_fov(glm::half_pi<float>() * 0.77f), m_aspect(1.0f), m_left(0.0f), m_right(0.0f), m_bottom(0.0f), m_top(0.0f)
{
}

CameraComponent::CameraComponent(float minDepth, float maxDepth) :
	m_depthRange(minDepth, maxDepth), m_projectionChanged(true), m_projectionType(PERSPECTIVE),
	m_fov(glm::half_pi<float>() * 0.77f), m_aspect(1.0f), m_left(0.0f), m_right(0.0f), m_bottom(0.0f), m_top(0.0f)
{
}

CameraComponent::CameraComponent(const vec2& zRange) :
	m_depthRange(zRange), m_projectionChanged(true), m_projectionType(PERSPECTIVE),
	m_fov(glm::half_pi<float>() * 0.77f), m_aspect(1.0f), m_left(0.0f), m_right(0.0f), m_bottom(0.0f), m_top(0.0f)
{
}

mat4 CameraComponent::getViewProjectionMatrix() const
{
	if (m_viewProjectionChanged) {
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
		m_viewProjectionChanged = false;
	}

	return m_viewProjectionMatrix;
}

mat4 CameraComponent::getViewMatrix() const
{
	return m_viewMatrix;
}

mat4 CameraComponent::getProjectionMatrix() const
{
	return m_projectionMatrix;
}

void CameraComponent::setMinDepth(float minDepth)
{
	m_depthRange.x = minDepth;
	m_projectionChanged = true;
}

float CameraComponent::getMinDepth() const
{
	return m_depthRange.x;
}

void CameraComponent::setMaxDepth(float maxDepth)
{
	m_depthRange.y = maxDepth;
	m_projectionChanged = true;
}

float CameraComponent::getMaxDepth() const
{
	return m_depthRange.y;
}

void CameraComponent::setDepthRange(const vec2 & depthRange)
{
	m_depthRange = depthRange;
	m_projectionChanged = true;
}

vec2 CameraComponent::getDepthRange()
{
	return m_depthRange;
}

void CameraComponent::updateView(const mat4& globalTransformation) const
{
	if (globalTransformation != m_globalTransformation) {
		m_viewMatrix = glm::inverse(globalTransformation);

		m_globalTransformation = globalTransformation;
		m_viewProjectionChanged = true;
	}
}

void CameraComponent::updateProjection() const
{
	if (m_projectionChanged) {
		switch (m_projectionType) {
		case PERSPECTIVE:
			{
				float f = 1.0f / tan(m_fov / 2.0f);

				m_projectionMatrix = mat4(
					f / m_aspect, 0.0f, 0.0f, 0.0f,
					0.0f, f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, -1.0f,
					0.0f, 0.0f, m_depthRange.x, 0.0f);

				m_projectionChanged = false;
				m_viewProjectionChanged = true;
				break;
			}

		case ISOMETRIC:
			{
				m_projectionMatrix = glm::ortho(m_left, m_right, 
					m_bottom, m_top, m_depthRange.x, m_depthRange.y);

				m_projectionChanged = false;
				m_viewProjectionChanged = true;
				break;
			}
		}
	}
}

void CameraComponent::setProjectionType(ProjectionType type)
{
	if (type != m_projectionType) {
		m_projectionType = type;
		m_projectionChanged = true;
	}
}

CameraComponent::ProjectionType CameraComponent::getProjectionType() const
{
	return m_projectionType;
}

void CameraComponent::setFieldOfView(float fov)
{
	if (fov != m_fov) {
		m_fov = fov;

		if (m_projectionType == PERSPECTIVE) {
			m_projectionChanged = true;
		}
	}
}

float CameraComponent::getFieldOfView() const
{
	return m_fov;
}

void CameraComponent::setAspect(float aspect)
{
	if (aspect != m_aspect) {
		m_aspect = aspect;

		if (m_projectionType == PERSPECTIVE) {
			m_projectionChanged = true;
		}
	}
}

float CameraComponent::getAspect() const
{
	return m_aspect;
}

void CameraComponent::setDimensions(float leftAndRight, float bottomAndTop)
{
	if (m_right != -m_left || m_right != leftAndRight || m_top != -m_bottom || m_top != bottomAndTop) {
		m_right = leftAndRight;
		m_left = -leftAndRight;
		m_top = bottomAndTop;
		m_bottom = -bottomAndTop;

		if (m_projectionType == ISOMETRIC) {
			m_projectionChanged = true;
		}
	}
}

void CameraComponent::setDimensions(float left, float right, float bottom, float top)
{
	if (m_left != left || m_right != right || m_bottom != bottom || m_top != top) {
		m_left = left;
		m_right = right;
		m_bottom = bottom;
		m_top = top;

		if (m_projectionType == ISOMETRIC) {
			m_projectionChanged = true;
		}
	}
}

float CameraComponent::getLeftDimension() const
{
	return m_left;
}

float CameraComponent::getRightDimension() const
{
	return m_right;
}

float CameraComponent::getBottomDimension() const
{
	return m_bottom;
}

float CameraComponent::getTopDimension() const
{
	return m_top;
}
