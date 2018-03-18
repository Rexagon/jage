#include "LightComponent.h"

LightComponent::LightComponent(const vec3 & color, Type type) :
	m_color(color), m_type(type), m_shadowBuffer(nullptr)
{
}

void LightComponent::setColor(const vec3 & color)
{
	m_color = color;
}

vec3 LightComponent::getColor() const
{
	return m_color;
}

void LightComponent::setType(Type type)
{
	if (m_type == type) return;
	m_type = type;

	switch (m_type)
	{
	case DIRECTIONAL:
		setProjectionType(ISOMETRIC);
		setDimensions(20.0f, 20.0f);
		setDepthRange(vec2(-15.0f, 15.0f));
		break;

	case POINT:
		break;

	case SPOT:
		break;
	}
}

LightComponent::Type LightComponent::getType() const
{
	return m_type;
}

void LightComponent::setShadowCastingEnabled(bool enabled)
{
	if (m_shadowBuffer == nullptr && enabled) {
		m_shadowBuffer = std::make_unique<FrameBuffer>(m_shadowBufferSize.x, m_shadowBufferSize.y, GL_UNSIGNED_BYTE, 1, true);
	}
	else if (m_shadowBuffer != nullptr && !enabled) {
		m_shadowBuffer.reset();
	}
}

bool LightComponent::isShadowCastingEnabled() const
{
	return m_shadowBuffer != nullptr;
}

FrameBuffer * LightComponent::getShadowBuffer() const
{
	return m_shadowBuffer.get();
}

void LightComponent::setShadowBufferSize(const uvec2 & shadowBufferSize)
{
	if (m_shadowBufferSize != shadowBufferSize) {
		return;
	}
	m_shadowBufferSize = shadowBufferSize;

	if (m_shadowBuffer != nullptr) {
		m_shadowBuffer->resize(shadowBufferSize);
	}
}

uvec2 LightComponent::getShadowBufferSize() const
{
	if (m_shadowBuffer == nullptr) {
		return m_shadowBufferSize;
	}
	else {
		return m_shadowBuffer->getSize();
	}
}

void LightComponent::updateView(const mat4 & globalTransformation)
{
	CameraComponent::updateView(globalTransformation);
}

void LightComponent::updateProjection()
{
	CameraComponent::updateProjection();
}

mat4 LightComponent::getViewProjectionMatrix() const
{
	return CameraComponent::getViewProjectionMatrix();
}

mat4 LightComponent::getViewMatrix() const
{
	return CameraComponent::getViewMatrix();
}

mat4 LightComponent::getProjectionMatrix() const
{
	return CameraComponent::getProjectionMatrix();
}
