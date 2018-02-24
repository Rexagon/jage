#include "GameObject.h"

#include <stack>

#include "Log.h"

GameObject::GameObject(const std::string & name) :
	m_name(name), m_isActive(true), m_parent(nullptr)
{
}

GameObject::~GameObject()
{
	m_children.clear();
}

std::shared_ptr<GameObject> GameObject::duplicate()
{	
	std::shared_ptr<GameObject> result = std::make_shared<GameObject>(m_name);
	result->m_tag = m_tag;
	result->m_isActive = m_isActive;
	result->m_transformation = m_transformation;
	result->setParent(nullptr);

	std::stack<GameObject*> currentStructure;
	std::stack<GameObject*> resultStructure;

	currentStructure.push(this);
	resultStructure.push(result.get());

	while (!resultStructure.empty())
	{
		GameObject* resultObject = resultStructure.top();
		resultStructure.pop();

		GameObject* currentObject = currentStructure.top();
		currentStructure.pop();

		for (auto it = currentObject->m_children.begin(); it != currentObject->m_children.end(); ++it) {
			const auto& currentChild = (*it);

			auto resultChild = std::make_shared<GameObject>(currentChild->m_name);
			resultChild->m_tag = currentChild->m_tag;
			resultChild->m_isActive = currentChild->m_isActive;
			resultChild->m_transformation = currentChild->m_transformation;
			resultChild->setParent(resultObject);

			currentStructure.push(currentChild.get());
			resultStructure.push(resultChild.get());

			resultObject->addChild(std::move(resultChild));
		}
	}

	return std::move(result);
}

void GameObject::setActive(bool active)
{
	m_isActive = active;
}

bool GameObject::isActive() const
{
	return m_isActive;
}

void GameObject::setVisible(bool visible)
{
	m_isVisible = visible;
}

bool GameObject::isVisible() const
{
	return m_isVisible;
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

void GameObject::setParent(GameObject * parent)
{
	m_parent = parent;

	if (m_parent == nullptr) {
		m_transformation.setParent(nullptr);
	}
	else {
		m_transformation.setParent(&parent->getTransformation());
	}
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

Transformation & GameObject::getTransformation()
{
	return m_transformation;
}
