#pragma once

#include <memory>

#include "Math.h"
#include "FrameBuffer.h"
#include "LightMaterial.h"
#include "CameraComponent.h"

class LightComponent : private CameraComponent
{
public:
	enum Type
	{
		DIRECTIONAL,
		POINT,
		SPOT
	};

	LightComponent(const vec3& color = vec3(1.0f, 1.0f, 1.0f), Type type = POINT);
	LightComponent(const LightComponent& other);

	void setColor(const vec3& color);
	vec3 getColor() const;

	void setType(Type type);
	Type getType() const;

	void setShadowCastingEnabled(bool enabled);
	bool isShadowCastingEnabled() const;

	FrameBuffer* getShadowBuffer() const;

	void setShadowBufferSize(const uvec2& shadowBufferSize);
	uvec2 getShadowBufferSize() const;

	void updateView(const mat4& globalTransformation);
	void updateProjection();

	mat4 getViewProjectionMatrix() const;

	mat4 getViewMatrix() const;
	mat4 getProjectionMatrix() const;

	LightMaterial* getMaterial() const;

private:
	vec3 m_color;
	Type m_type;

	uvec2 m_shadowBufferSize;
	std::unique_ptr<LightMaterial> m_material;
	std::unique_ptr<FrameBuffer> m_shadowBuffer;
};