#pragma once

#include "GameObject.h"

class FirstPersonController
{
public:
	FirstPersonController();

	void update(const float dt, std::shared_ptr<GameObject> gameObject);

	void setSpeed(float speed);
	float getSpeed() const;

private:
	vec2 m_rotations;
	float m_speed;
};