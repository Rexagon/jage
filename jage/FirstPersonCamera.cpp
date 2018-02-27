#include "FirstPersonCamera.h"

#include "Core.h"

FirstPersonCamera::FirstPersonCamera(const std::string & name) :
	PerspectiveCamera(name, glm::pi<float>() / 2.0f, 1.0f)
{
}

void FirstPersonCamera::onUpdate(const float dt)
{
	vec3 direction(0.0f, 0.0f, 0.0f);
	if (Input::getKey(Key::W)) {
		direction += getDirectionFront();
	}
	else if (Input::getKey(Key::S)) {
		direction -= getDirectionFront();
	}

	if (Input::getKey(Key::A)) {
		direction -= getDirectionRight();
	}
	else if (Input::getKey(Key::D)) {
		direction += getDirectionRight();
	}

	if (Input::getKey(Key::Space)) {
		direction += vec3(0.0f, 1.0f, 0.0f);
	}
	else if (Input::getKey(Key::LControl)) {
		direction -= vec3(0.0f, 1.0f, 0.0f);
	}

	if (direction != vec3(0.0f, 0.0f, 0.0f)) {
		move(glm::normalize(direction) * dt * 10.0f);
	}

	if (Input::getMouse(MouseButton::Right)) {
		m_rotations.x -= Input::getMouseDeltaPosition().y * dt;
		m_rotations.y -= Input::getMouseDeltaPosition().x * dt;

		setRotation(m_rotations.x, 0.0f, 0.0f);
		rotate(0.0f, m_rotations.y, 0.0f);
	}
}
