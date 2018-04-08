#include "GameObject.h"

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <stack>

#include <glm/gtx/matrix_decompose.hpp>

#include "Log.h"

GameObject::GameObject(EntityManager * manager, EntityId id) :
	m_isActive(true), m_parent(nullptr), 
	m_manager(manager), m_id(id), m_isPendingDestroy(false),
	m_position(0.0f, 0.0f, 0.0f), m_rotation(1.0f, 0.0f, 0.0f, 0.0f), m_scale(1.0f, 1.0f, 1.0f),
	m_directionFront(0.0f, 0.0f, -1.0f), m_directionRight(1.0f, 0.0f, 0.0f), m_directionUp(0.0f, 1.0f, 0.0f),
	m_transformation(1.0f),
	m_positionMatrix(1.0f), m_positionMatrixInversed(1.0f),
	m_rotationMatrix(1.0f), m_rotationMatrixInversed(1.0f),
	m_scaleMatrix(1.0f), m_scaleMatrixInversed(1.0f),
	m_positionChanged(true), m_rotationChanged(true), m_scaleChanged(true), m_transformationChanged(true)
{
}

GameObject::~GameObject()
{
	m_children.clear();
}

void GameObject::setActive(bool active)
{
	m_isActive = active;
}

bool GameObject::isActive() const
{
	return m_isActive;
}

void GameObject::setName(const std::string & name)
{
	m_name = name;
}

std::string GameObject::getName() const
{
	return m_name;
}

void GameObject::setTag(const std::string & tag)
{
	m_tag = tag;
}

std::string GameObject::getTag() const
{
	return m_tag;
}

const EntityManager * GameObject::getEntityManager() const
{
	return m_manager;
}

EntityId GameObject::getId() const
{
	return m_id;
}

bool GameObject::isPendingDestroy() const
{
	return m_isPendingDestroy;
}

std::shared_ptr<GameObject> GameObject::clone()
{
	//TODO: implement
	return nullptr;
}

void GameObject::setParent(GameObject * parent)
{
	m_parent = parent;
}

GameObject * GameObject::getParent() const
{
	return m_parent;
}

void GameObject::addChild(std::shared_ptr<GameObject> object)
{
	if (object.get() == nullptr || object.get() == this) {
		return;
	}

	if (object->m_parent != nullptr) {
		object = object->m_parent->detachChildByName(object->m_name);
	}

	object->setParent(this);
	m_children.push_back(std::move(object));
}

GameObject * GameObject::getChildByName(const std::string & name, bool deep)
{
	std::stack<GameObject*> objects;
	objects.push(this);

	while (!objects.empty()) {
		GameObject* object = objects.top();
		objects.pop();

		for (auto it = object->m_children.begin(); it != object->m_children.end(); ++it) {
			auto& child = (*it);

			if (child->m_name == name) {
				return child.get();
			}
			else {
				objects.push(child.get());
			}
		}

		if (!deep) break;
	}

	return nullptr;
}

void GameObject::deleteChildByName(const std::string & name, bool deep)
{
	std::stack<GameObject*> objects;
	objects.push(this);

	while (!objects.empty()) {
		GameObject* object = objects.top();
		objects.pop();

		for (auto it = object->m_children.begin(); it != object->m_children.end(); ++it) {
			auto& child = (*it);

			if (child->m_name == name) {
				child->m_children.erase(it);
				return;
			}
			else {
				objects.push(child.get());
			}
		}

		if (!deep) break;
	}
}

std::shared_ptr<GameObject> GameObject::detachChildByName(const std::string & name, bool deep)
{
	std::stack<GameObject*> objects;
	objects.push(this);

	while (!objects.empty()) {
		GameObject* object = objects.top();
		objects.pop();

		for (auto it = object->m_children.begin(); it != object->m_children.end(); ++it) {
			auto& child = (*it);

			if (child->m_name == name) {
				child->setParent(nullptr);

				auto result = std::move(child);
				child->m_children.erase(it);
				return result;
			}
			else {
				objects.push(child.get());
			}
		}

		if (!deep) break;
	}

	return std::shared_ptr<GameObject>();
}

std::vector<std::shared_ptr<GameObject>>& GameObject::getChildren()
{
	return m_children;
}

std::bitset<MAX_COMPONENTS> GameObject::getComponentMask() const
{
	return m_manager->getComponentMask(m_id);
}

void GameObject::destroy()
{
	m_manager->destroy(m_id);
	invalidate();
}

void GameObject::invalidate()
{
	m_id = EntityId::INVALID;
	m_manager = nullptr;
}

bool GameObject::isValid() const
{
	return m_manager != nullptr && m_manager->isValid(m_id);
}

bool GameObject::operator==(const GameObject & other) const
{
	return m_manager == other.m_manager && m_id == other.m_id;
}

bool GameObject::operator!=(const GameObject & other) const
{
	return m_manager != other.m_manager || m_id != other.m_id;
}

bool GameObject::operator<(const GameObject & other) const
{
	return other.m_id < m_id;
}

std::shared_ptr<GameObject> GameObject::create(EntityManager * manager, EntityId id)
{
	return std::make_shared<GameObject>(manager, id);
}

mat4 GameObject::getGlobalTransformation() const
{
	if (m_parent == nullptr) {
		return getTransformationMatrix();
	}
	else {
		return m_parent->getGlobalTransformation() * getTransformationMatrix();
	}
}

void GameObject::setTransformationMatrix(const mat4 & transform)
{
	glm::vec3 skew;
	glm::vec4 perspective;

	glm::decompose(transform, m_scale, m_rotation, m_position, skew, perspective);
}

mat4 GameObject::getTransformationMatrix() const
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

mat4 GameObject::getPositionMatrix() const
{
	updatePosition();

	return m_positionMatrix;
}

mat4 GameObject::getPositionMatrixInversed() const
{
	updatePosition();

	return m_positionMatrixInversed;
}

mat4 GameObject::getRotationMatrix() const
{
	updateRotation();

	return m_rotationMatrix;
}

mat4 GameObject::getRotationMatrixInversed() const
{
	updateRotation();

	return m_rotationMatrixInversed;
}

mat4 GameObject::getScaleMatrix() const
{
	updateScale();

	return m_scaleMatrix;
}

mat4 GameObject::getScaleMatrixInversed() const
{
	updateScale();

	return m_scaleMatrixInversed;
}

void GameObject::move(float x, float y, float z)
{
	m_position.x += x;
	m_position.y += y;
	m_position.z += z;
	m_positionChanged = true;
}

void GameObject::move(const vec3 & vector)
{
	m_position += vector;
	m_positionChanged = true;
}

void GameObject::setPosition(float x, float y, float z)
{
	m_position = vec3(x, y, z);
	m_positionChanged = true;
}

void GameObject::setPosition(const vec3 & position)
{
	m_position = position;
	m_positionChanged = true;
}

vec3 GameObject::getPosition() const
{
	return m_position;
}

void GameObject::rotate(float x, float y, float z)
{
	m_rotation = quat(vec3(math::radians(x), math::radians(y), math::radians(z))) * m_rotation;
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

void GameObject::rotate(const vec3 & eulerAngles)
{
	m_rotation = quat(math::radians(eulerAngles)) * m_rotation;
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

void GameObject::rotate(const quat & rotation)
{
	m_rotation = rotation * m_rotation;
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

void GameObject::setRotation(float x, float y, float z)
{
	m_rotation = quat(vec3(math::radians(x), math::radians(y), math::radians(z)));
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

void GameObject::setRotation(const vec3 & eulerAngles)
{
	m_rotation = quat(math::radians(eulerAngles));
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

void GameObject::setRotation(const quat & rotation)
{
	m_rotation = rotation;
	m_rotation = glm::normalize(m_rotation);
	m_rotationChanged = true;
}

quat GameObject::getRotation() const
{
	return m_rotation;
}

vec3 GameObject::getEulerRotation() const
{
	return glm::eulerAngles(m_rotation);
}

void GameObject::scale(float s)
{
	m_scale *= s;
	m_scaleChanged = true;
}

void GameObject::scale(float x, float y, float z)
{
	m_scale *= vec3(x, y, z);
	m_scaleChanged = true;
}

void GameObject::scale(const vec3 & s)
{
	m_scale *= s;
	m_scaleChanged = true;
}

void GameObject::setScale(float s)
{
	m_scale = vec3(s, s, s);
	m_scaleChanged = true;
}

void GameObject::setScale(float x, float y, float z)
{
	m_scale = vec3(x, y, z);
	m_scaleChanged = true;
}

void GameObject::setScale(const vec3 & s)
{
	m_scale = s;
	m_scaleChanged = true;
}

vec3 GameObject::getScale() const
{
	return m_scale;
}

vec3 GameObject::getDirectionFront() const
{
	updateRotation();

	return m_directionFront;
}

vec3 GameObject::getDirectionRight() const
{
	updateRotation();

	return m_directionRight;
}

vec3 GameObject::getDirectionUp() const
{
	updateRotation();

	return m_directionUp;
}

void GameObject::updatePosition() const
{
	if (m_positionChanged) {
		m_positionMatrix = glm::translate(mat4(1.0f), m_position);
		m_positionMatrixInversed = glm::translate(mat4(1.0f), -m_position);

		m_positionChanged = false;
		m_transformationChanged = true;
	}
}

void GameObject::updateRotation() const
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

void GameObject::updateScale() const
{
	if (m_scaleChanged) {
		m_scaleMatrix = glm::scale(mat4(1.0f), m_scale);
		m_scaleMatrixInversed = glm::scale(mat4(1.0f),
			vec3(1.0f / m_scale.x, 1.0f / m_scale.y, 1.0f / m_scale.z));

		m_scaleChanged = false;
		m_transformationChanged = true;
	}
}