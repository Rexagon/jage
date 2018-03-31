#pragma once

#include "Math.h"

class Transformable
{
public:
	Transformable(Transformable* parent = nullptr);
	virtual ~Transformable() {}
	
	void setTransformationMatrix(const mat4& transformation);
	mat4 getTransformationMatrix() const;

	mat4 getPositionMatrix() const;
	mat4 getPositionMatrixInversed() const;

	mat4 getRotationMatrix() const;
	mat4 getRotationMatrixInversed() const;

	mat4 getScaleMatrix() const;
	mat4 getScaleMatrixInversed() const;

	void move(float x, float y, float z);
	void move(const vec3& vector);
	void setPosition(float x, float y, float z);
	void setPosition(const vec3& position);
	vec3 getPosition() const;

	void rotate(float x, float y, float z);
	void rotate(const vec3& eulerAngles);
	void rotate(const quat& rotation);
	void setRotation(float x, float y, float z);
	void setRotation(const vec3& eulerAngles);
	void setRotation(const quat& rotation);
	quat getRotation() const;
	vec3 getEulerRotation() const;

	void scale(float s);
	void scale(float x, float y, float z);
	void scale(const vec3& s);
	void setScale(float s);
	void setScale(float x, float y, float z);
	void setScale(const vec3& s);
	vec3 getScale() const;

	vec3 getDirectionFront() const;
	vec3 getDirectionRight() const;
	vec3 getDirectionUp() const;

protected:
	vec3 m_position;
	quat m_rotation;
	vec3 m_scale;

	mutable vec3 m_directionFront;
	mutable vec3 m_directionRight;
	mutable vec3 m_directionUp;

	mutable mat4 m_transformation;

	mutable mat4 m_positionMatrix;
	mutable mat4 m_positionMatrixInversed;

	mutable mat4 m_rotationMatrix;
	mutable mat4 m_rotationMatrixInversed;

	mutable mat4 m_scaleMatrix;
	mutable mat4 m_scaleMatrixInversed;

private:
	void updatePosition() const;
	void updateRotation() const;
	void updateScale() const;

	mutable bool m_positionChanged;
	mutable bool m_rotationChanged;
	mutable bool m_scaleChanged;

	mutable bool m_transformationChanged;
};