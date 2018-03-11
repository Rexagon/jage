#include "GameObject.h"

#include <stack>

#include "Log.h"


GameObject::GameObject(EntityManager * manager, EntityId id) :
	m_isActive(true), m_parent(nullptr), 
	m_manager(manager), m_id(id), m_isPendingDestroy(false)
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

mat4 GameObject::getGlobalTransformation() const
{
	if (m_parent == nullptr) {
		return getTransformationMatrix();
	}
	else {
		return m_parent->getGlobalTransformation() * getTransformationMatrix();
	}
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
