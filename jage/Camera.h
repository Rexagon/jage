#pragma once

#include "GameObject.h"

class Camera : public GameObject
{
public:
	Camera(const std::string& name);
	Camera(const std::string& name, float minDepth, float maxDepth);
	Camera(const std::string& name, const vec2& zRange);
	virtual ~Camera() {}

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
	void updateView() const;
	virtual void updateProjection() const = 0;
	
	vec2 m_depthRange;

	mutable mat4 m_viewMatrix;
	mutable mat4 m_globalTransformation;

	mutable mat4 m_viewProjectionMatrix;
	mutable bool m_viewProjectionChanged;

	mutable mat4 m_projectionMatrix;
	mutable bool m_projectionChanged;
};


class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(const std::string& name, float fov, float aspect);
	PerspectiveCamera(const std::string& name, float fov, float aspect, float minDepth, float maxDepth);

	void setFieldOfView(float fov);
	float getFieldOfView() const;

	void setAspect(float aspect);
	float getAspect() const;

private:
	void updateProjection() const override;

	float m_aspect;
	float m_fov;
};


class IsometricCamera : public Camera
{
public:
	IsometricCamera(const std::string& name, float leftAndRight, float bottomAndTop);
	IsometricCamera(const std::string& name, float left, float right, float bottom, float top);
	IsometricCamera(const std::string& name, float left, float right, float bottom, float top, float minDepth, float maxDepth);

	void setDimensions(float leftAndRight, float bottomAndTop);
	void setDimensions(float left, float right, float bottom, float top);

	float getLeftDimension() const;
	float getRightDimension() const;
	float getBottomDimension() const;
	float getTopDimension() const;

private:
	void updateProjection() const override;

	float m_left;
	float m_right;
	float m_bottom;
	float m_top;
};