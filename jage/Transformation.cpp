#include "Transformation.h"

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/gtx/matrix_decompose.hpp>

Transformation::Transformation(Transformation* parent) :
	m_parent(parent), m_parentGlobalTransform(1.0f),
	m_position(0.0f, 0.0f, 0.0f), m_rotation(0.0f, 0.0f, 0.0f, 1.0f), m_scale(1.0f, 1.0f, 1.0f),
	m_positionChanged(true), m_rotationChanged(true), m_scaleChanged(true)
{
	if (m_parent != nullptr) {
		m_parentGlobalTransform = m_parent->getGlobalTransform();
	}
}

void Transformation::setLocalTransform(const mat4 & transform)
{
	glm::vec3 skew;
	glm::vec4 perspective;

	glm::decompose(transform, m_scale, m_rotation, m_position, skew, perspective);
	m_rotation = glm::conjugate(m_rotation);
}

mat4 Transformation::getLocalTransform() const
{
	if (wasUpdated()) {
		m_transform = getPositionMatrix() * getRotationMatrix() * getScaleMatrix();
	}

	return m_transform;
}

mat4 Transformation::getGlobalTransform() const
{
	if (m_parent != nullptr && m_parent->wasUpdated()) {
		m_parentGlobalTransform = m_parent->getGlobalTransform();
	}

	return m_parentGlobalTransform * getLocalTransform();
}

mat4 Transformation::getPositionMatrix() const
{
	if (m_positionChanged) {
		m_positionMatrix = glm::translate(mat4(1.0f), m_position);
		m_positionChanged = false;
	}

	return m_positionMatrix;
}

mat4 Transformation::getRotationMatrix() const
{
	if (m_rotationChanged) {
		m_rotationMatrix = glm::mat4_cast(m_rotation);
		m_rotationChanged = false;
	}

	return m_rotationMatrix;
}

mat4 Transformation::getScaleMatrix() const
{
	if (m_scaleChanged) {
		m_scaleMatrix = glm::scale(mat4(1.0f), m_scale);
		m_scaleChanged = false;
	}

	return m_scaleMatrix;
}

void Transformation::move(float x, float y, float z)
{
	m_position.x += x;
	m_position.y += y;
	m_position.z += z;
	m_positionChanged = true;
}

void Transformation::move(const vec3 & vector)
{
	m_position += vector;
	m_positionChanged = true;
}

void Transformation::setPosition(float x, float y, float z)
{
	m_position = vec3(x, y, z);
	m_positionChanged = true;
}

void Transformation::setPosition(const vec3 & position)
{
	m_position = position;
	m_positionChanged = true;
}

vec3 Transformation::getPosition() const
{
	return m_position;
}

void Transformation::rotate(float x, float y, float z)
{
	m_rotation = quat(vec3(x, y, z)) * m_rotation;
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

void Transformation::rotate(const vec3 & eulerAngles)
{
	m_rotation = quat(eulerAngles) * m_rotation;
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

void Transformation::rotate(const quat & rotation)
{
	m_rotation = rotation * m_rotation;
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

void Transformation::setRotation(float x, float y, float z)
{
	m_rotation = quat(vec3(x, y, z));
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

void Transformation::setRotation(const vec3 & eulerAngles)
{
	m_rotation = quat(eulerAngles);
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

void Transformation::setRotation(const quat & rotation)
{
	m_rotation = rotation;
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

quat Transformation::getRotation() const
{
	return m_rotation;
}

void Transformation::scale(float s)
{
	m_scale *= s;
	m_scaleChanged = true;
}

void Transformation::scale(float x, float y, float z)
{
	m_scale *= vec3(x, y, z);
	m_scaleChanged = true;
}

void Transformation::scale(const vec3 & s)
{
	m_scale *= s;
	m_scaleChanged = true;
}

void Transformation::setScale(float s)
{
	m_scale = vec3(s, s, s);
	m_scaleChanged = true;
}

void Transformation::setScale(float x, float y, float z)
{
	m_scale = vec3(x, y, z);
	m_scaleChanged = true;
}

void Transformation::setScale(const vec3 & s)
{
	m_scale = s;
	m_scaleChanged = true;
}

vec3 Transformation::getScale() const
{
	return m_scale;
}

vec3 Transformation::getDirectionFront() const
{
	vec4 temp(0.0f, 0.0f, -1.0f, 1.0f);
	temp = getRotationMatrix() * temp;
	temp /= temp.w;
	vec3 result(temp.x, temp.y, temp.z);
	return glm::normalize(result);
}

vec3 Transformation::getDirectionRight() const
{
	return glm::normalize(glm::cross(vec3(0.0f, 1.0f, 0.0f), getDirectionFront()));
}

vec3 Transformation::getDirectionUp() const
{
	vec3 directionFront = getDirectionFront();

	return glm::normalize(glm::cross(glm::cross(directionFront, vec3(0.0f, 1.0f, 0.0f)), directionFront));
}

void Transformation::setParent(Transformation * parent)
{
	m_parent = parent;
	if (m_parent == nullptr) {
		m_parentGlobalTransform = mat4();
	}
	else {
		m_parentGlobalTransform = parent->getGlobalTransform();
	}
}

Transformation * Transformation::getParent() const
{
	return m_parent;
}

bool Transformation::wasUpdated() const
{
	return m_positionChanged || m_rotationChanged || m_scaleChanged;
}
