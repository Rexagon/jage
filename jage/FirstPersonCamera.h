#pragma once

#include "Camera.h"

class FirstPersonCamera : public PerspectiveCamera
{
public:
	FirstPersonCamera(const std::string& name);

	void onUpdate(const float dt) override;

private:
	vec2 m_rotations;
};