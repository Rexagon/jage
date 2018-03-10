#pragma once

#include "Math.h"

class CameraComponent
{
public:
	enum ProjectionType
	{
		PERSPECTIVE,
		ISOMETRIC
	};

	CameraComponent();
	CameraComponent(float minDepth, float maxDepth);
	CameraComponent(const vec2& zRange);

	void updateView(const mat4& globalTransformation) const;
	void updateProjection() const;

	void setProjectionType(ProjectionType type);
	ProjectionType getProjectionType() const;

	// perspective projection functions:
	void setFieldOfView(float fov);
	float getFieldOfView() const;

	void setAspect(float aspect);
	float getAspect() const;
	//

	// isometric projection functions:
	void setDimensions(float leftAndRight, float bottomAndTop);
	void setDimensions(float left, float right, float bottom, float top);

	float getLeftDimension() const;
	float getRightDimension() const;
	float getBottomDimension() const;
	float getTopDimension() const;
	//

	mat4 getViewProjectionMatrix() const;

	mat4 getViewMatrix() const;
	mat4 getProjectionMatrix() const;

	void setMinDepth(float nearZ);
	float getMinDepth() const;

	void setMaxDepth(float farZ);
	float getMaxDepth() const;

	void setDepthRange(const vec2& zRange);
	vec2 getDepthRange();

protected:
	vec2 m_depthRange;

	ProjectionType m_projectionType;

	float m_fov;
	float m_aspect;

	float m_left;
	float m_right;
	float m_bottom;
	float m_top;

	mutable mat4 m_viewMatrix;
	mutable mat4 m_globalTransformation;

	mutable mat4 m_viewProjectionMatrix;
	mutable bool m_viewProjectionChanged;

	mutable mat4 m_projectionMatrix;
	mutable bool m_projectionChanged;
};
