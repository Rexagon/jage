#include "LightComponent.h"

LightComponent::LightComponent(const vec3 & color, Type type) :
	m_color(color), m_type(type), 
	m_shadowBufferSize(2048.0f, 2048.0f), m_shadowBuffer(nullptr),
	m_material(nullptr)
{
	m_material = std::make_unique<LightMaterial>();
}

LightComponent::LightComponent(const LightComponent & other)
{
	m_color = other.m_color;
	m_type = other.m_type;
	m_shadowBufferSize = other.m_shadowBufferSize;
	m_shadowBuffer.reset(new FrameBuffer(*other.m_shadowBuffer));
}

void LightComponent::setColor(const vec3 & color)
{
	m_color = color;
	m_material->setColor(color);
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
		setDimensions(50.0f, 50.0f);
		setDepthRange(vec2(-100.0f, 40.0f));
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

	m_material->setShadowCastingEnabled(enabled);
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

LightMaterial * LightComponent::getMaterial() const
{
	return m_material.get();
}
