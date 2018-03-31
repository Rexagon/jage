#include "Transformable.h"

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/gtx/matrix_decompose.hpp>

#include "Log.h"

Transformable::Transformable(Transformable* parent) :
	m_position(0.0f, 0.0f, 0.0f), m_rotation(1.0f, 0.0f, 0.0f, 0.0f), m_scale(1.0f, 1.0f, 1.0f),
	m_directionFront(0.0f, 0.0f, -1.0f), m_directionRight(1.0f, 0.0f, 0.0f), m_directionUp(0.0f, 1.0f, 0.0f),
	m_transformation(1.0f), 
	m_positionMatrix(1.0f), m_positionMatrixInversed(1.0f), 
	m_rotationMatrix(1.0f), m_rotationMatrixInversed(1.0f), 
	m_scaleMatrix(1.0f), m_scaleMatrixInversed(1.0f),
	m_positionChanged(true), m_rotationChanged(true), m_scaleChanged(true), m_transformationChanged(true)
{
}

void Transformable::setTransformationMatrix(const mat4 & transform)
{
	glm::vec3 skew;
	glm::vec4 perspective;

	glm::decompose(transform, m_scale, m_rotation, m_position, skew, perspective);
}

mat4 Transformable::getTransformationMatrix() const
{
	updatePosition();
	updateRotation();
	updateScale();

	if (m_transformationChanged) {
		m_transformation = getPositionMatrix() * getRotationMatrix() * getScaleMatrix();
		m_transformationChanged = false;
	}

	return m_transformation;
}

mat4 Transformable::getPositionMatrix() const
{
	updatePosition();

	return m_positionMatrix;
}

mat4 Transformable::getPositionMatrixInversed() const
{
	updatePosition();

	return m_positionMatrixInversed;
}

mat4 Transformable::getRotationMatrix() const
{
	updateRotation();

	return m_rotationMatrix;
}

mat4 Transformable::getRotationMatrixInversed() const
{
	updateRotation();

	return m_rotationMatrixInversed;
}

mat4 Transformable::getScaleMatrix() const
{
	updateScale();

	return m_scaleMatrix;
}

mat4 Transformable::getScaleMatrixInversed() const
{
	updateScale();

	return m_scaleMatrixInversed;
}

void Transformable::move(float x, float y, float z)
{
	m_position.x += x;
	m_position.y += y;
	m_position.z += z;
	m_positionChanged = true;
}

void Transformable::move(const vec3 & vector)
{
	m_position += vector;
	m_positionChanged = true;
}

void Transformable::setPosition(float x, float y, float z)
{
	m_position = vec3(x, y, z);
	m_positionChanged = true;
}

void Transformable::setPosition(const vec3 & position)
{
	m_position = position;
	m_positionChanged = true;
}

vec3 Transformable::getPosition() const
{
	return m_position;
}

void Transformable::rotate(float x, float y, float z)
{
	m_rotation = quat(vec3(x, y, z)) * m_rotation;
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

void Transformable::rotate(const vec3 & eulerAngles)
{
	m_rotation = quat(eulerAngles) * m_rotation;
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

void Transformable::rotate(const quat & rotation)
{
	m_rotation = rotation * m_rotation;
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

void Transformable::setRotation(float x, float y, float z)
{
	m_rotation = quat(vec3(x, y, z));
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

void Transformable::setRotation(const vec3 & eulerAngles)
{
	m_rotation = quat(eulerAngles);
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

void Transformable::setRotation(const quat & rotation)
{
	m_rotation = rotation;
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

quat Transformable::getRotation() const
{
	return m_rotation;
}

vec3 Transformable::getEulerRotation() const
{
	return glm::eulerAngles(m_rotation);
}

void Transformable::scale(float s)
{
	m_scale *= s;
	m_scaleChanged = true;
}

void Transformable::scale(float x, float y, float z)
{
	m_scale *= vec3(x, y, z);
	m_scaleChanged = true;
}

void Transformable::scale(const vec3 & s)
{
	m_scale *= s;
	m_scaleChanged = true;
}

void Transformable::setScale(float s)
{
	m_scale = vec3(s, s, s);
	m_scaleChanged = true;
}

void Transformable::setScale(float x, float y, float z)
{
	m_scale = vec3(x, y, z);
	m_scaleChanged = true;
}

void Transformable::setScale(const vec3 & s)
{
	m_scale = s;
	m_scaleChanged = true;
}

vec3 Transformable::getScale() const
{
	return m_scale;
}

vec3 Transformable::getDirectionFront() const
{
	updateRotation();

	return m_directionFront;
}

vec3 Transformable::getDirectionRight() const
{
	updateRotation();

	return m_directionRight;
}

vec3 Transformable::getDirectionUp() const
{
	updateRotation();

	return m_directionUp;
}

void Transformable::updatePosition() const
{
	if (m_positionChanged) {
		m_positionMatrix = glm::translate(mat4(1.0f), m_position);
		m_positionMatrixInversed = glm::translate(mat4(1.0f), -m_position);

		m_positionChanged = false;
		m_transformationChanged = true;
	}
}

void Transformable::updateRotation() const
{
	if (m_rotationChanged) {
		m_rotationMatrix = glm::mat4_cast(m_rotation);
		m_rotationMatrixInversed = glm::inverse(m_rotationMatrix);

		// update direction also
		vec4 temp(0.0f, 0.0f, -1.0f, 1.0f);
		temp = m_rotationMatrix * temp;
		temp /= temp.w;
		vec3 directionFront(temp.x, temp.y, temp.z);

		m_directionFront = glm::normalize(directionFront);
		m_directionRight = glm::cross(m_directionFront, vec3(0.0f, 1.0f, 0.0f));
		m_directionUp = glm::cross(m_directionRight, m_directionFront);

		m_rotationChanged = false;
		m_transformationChanged = true;
	}
}

void Transformable::updateScale() const
{
	if (m_scaleChanged) {
		m_scaleMatrix = glm::scale(mat4(1.0f), m_scale);
		m_scaleMatrixInversed = glm::scale(mat4(1.0f), 
			vec3(1.0f / m_scale.x, 1.0f / m_scale.y, 1.0f / m_scale.z));

		m_scaleChanged = false;
		m_transformationChanged = true;
	}
}