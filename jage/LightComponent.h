#pragma once

#include <memory>

#include "Math.h"
#include "FrameBuffer.h"

class LightComponent
{
public:
	enum Type
	{
		DIRECTIONAL,
		POINT,
		SPOT
	};

	LightComponent(const vec3& color = vec3(1.0f, 1.0f, 1.0f), Type type = POINT);

	void setColor(const vec3& color);
	vec3 getColor() const;

	void setType(Type type);
	Type getType() const;

	void setShadowCastingEnabled(bool enabled);
	bool isShadowCastingEnabled() const;

	FrameBuffer* getShadowBuffer() const;

	mat4 getProjectionMatrix() const;

	void setShadowBufferSize(const uvec2& shadowBufferSize);
	uvec2 getShadowBufferSize() const;

private:
	void updateProjection();

	vec3 m_color;
	Type m_type;

	uvec2 m_shadowBufferSize;
	std::unique_ptr<FrameBuffer> m_shadowBuffer;

	mutable bool m_projectionChanged;
	mutable mat4 m_projection;
};