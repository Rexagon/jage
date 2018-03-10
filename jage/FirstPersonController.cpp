#include "FirstPersonController.h"

#include "Core.h"

FirstPersonController::FirstPersonController() :
	m_rotations(0.0f, 0.0f), m_speed(10.0f)
{
}

void FirstPersonController::update(const float dt, object_ptr gameObject)
{
	vec3 direction(0.0f, 0.0f, 0.0f);
	if (Input::getKey(Key::W)) {
		direction += gameObject->getDirectionFront();
	}
	else if (Input::getKey(Key::S)) {
		direction -= gameObject->getDirectionFront();
	}

	if (Input::getKey(Key::A)) {
		direction -= gameObject->getDirectionRight();
	}
	else if (Input::getKey(Key::D)) {
		direction += gameObject->getDirectionRight();
	}

	if (Input::getKey(Key::Space)) {
		direction += vec3(0.0f, 1.0f, 0.0f);
	}
	else if (Input::getKey(Key::LControl)) {
		direction -= vec3(0.0f, 1.0f, 0.0f);
	}

	if (direction != vec3(0.0f, 0.0f, 0.0f)) {
		float speed = m_speed;
		if (Input::getKey(Key::LShift)) {
			speed *= 2.0f;
		}
		gameObject->move(glm::normalize(direction) * dt * speed);
	}

	if (Input::getMouse(MouseButton::Right)) {
		m_rotations.x -= Input::getMouseDeltaPosition().y * dt;
		m_rotations.y -= Input::getMouseDeltaPosition().x * dt;

		gameObject->setRotation(m_rotations.x, 0.0f, 0.0f);
		gameObject->rotate(0.0f, m_rotations.y, 0.0f);
	}
}

void FirstPersonController::setSpeed(float speed)
{
	m_speed = speed;
}

float FirstPersonController::getSpeed() const
{
	return m_speed;
}