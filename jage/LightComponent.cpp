#include "LightComponent.h"

LightComponent::LightComponent(const vec3 & color, Type type) :
	m_color(color), m_type(type), m_shadowBuffer(nullptr)
{
}

void LightComponent::setType(Type type)
{
	m_type = type;
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

mat4 LightComponent::getProjectionMatrix() const
{
	
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

void LightComponent::setColor(const vec3 & color)
{
	m_color = color;
}

vec3 LightComponent::getColor() const
{
	return m_color;
}
