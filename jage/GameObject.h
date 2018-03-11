#pragma once

#include <typeindex>
#include <vector>
#include <memory>
#include <string>
#include <map>

#include "EntityManager.h"
#include "Transformable.h"

class GameObject : public Transformable
{
public:
	GameObject(EntityManager* manager, EntityId id);
	virtual ~GameObject();

	virtual void setActive(bool active);
	bool isActive() const;

	void setName(const std::string& name);
	std::string getName() const;

	void setTag(const std::string& tag);
	std::string getTag() const;

	mat4 getGlobalTransformation() const;

	const EntityManager* getEntityManager() const;

	EntityId getId() const;

	bool isPendingDestroy() const;

	// Tree structure functions
	std::shared_ptr<GameObject> clone();

	void setParent(GameObject* parent);
	GameObject* getParent() const;
	
	void addChild(std::shared_ptr<GameObject> node);

	GameObject* getChildByName(const std::string& name, bool deep = false);

	void deleteChildByName(const std::string& name, bool deep = false);

	std::shared_ptr<GameObject> detachChildByName(const std::string& name, bool deep = false);

	std::vector<std::shared_ptr<GameObject>>& getChildren();

	// Component functions
	template<typename T, typename... Args>
	ComponentHandle<T> assign(Args&&... args)
	{
		return m_manager->assign<T>(m_id, std::forward<Args>(args)...);
	}

	template<typename T>
	ComponentHandle<T> assignFromCopy(const T& component)
	{
		return m_manager->assign<T>(m_id, std::forward<const T&>(component));
	}

	template<typename T, typename... Args>
	ComponentHandle<T> replace(Args&&... args)
	{
		ComponentHandle<T> handle = getComponent<T>();
		if (handle) {
			(*handle.get()) = T(std::forward<Args>(args)...);
		}
		else {
			handle = m_manager->assign<T>(m_id, std::forward<Args>(args)...);
		}

		return handle;
	}

	template<typename T>
	void remove()
	{
		m_manager->remove<T>(m_id);
	}
	
	template<typename T>
	ComponentHandle<T> getComponent()
	{
		return m_manager->getComponent<T>(m_id);
	}

	template<typename T>
	const ComponentHandle<T> getComponent() const
	{
		return const_cast<const EntityManager*>(m_manager)->getComponent<const T>(m_id);
	}

	template<typename... Ts>
	std::tuple<ComponentHandle<Ts>...> getComponents()
	{
		return m_manager->getComponents<Ts...>(m_id);
	}

	template<typename... Ts>
	std::tuple<ComponentHandle<const Ts>...> getComponents() const
	{
		return const_cast<const EntityManager*>(m_manager)->getComponent<const Ts...>(m_id);
	}

	template<typename T>
	bool hasComponent() const
	{
		return m_manager->hasComponent<T>(m_id);
	}

	template<typename T, typename... Ts>
	void unpack(ComponentHandle<T>& c, ComponentHandle<Ts>&... cs)
	{
		return m_manager->unpack(m_id, cs...);
	}

	std::bitset<MAX_COMPONENTS> getComponentMask() const;

	void destroy();
	void invalidate();
	bool isValid() const;

	bool operator==(const GameObject& other) const;
	bool operator!=(const GameObject& other) const;
	bool operator<(const GameObject& other) const;

protected:
	std::string m_name;
	std::string m_tag;

	bool m_isActive;

	GameObject* m_parent;
	std::vector<std::shared_ptr<GameObject>> m_children;

private:
	friend class EntityManager;

	static std::shared_ptr<GameObject> create(EntityManager* manager, EntityId id);

	EntityManager* m_manager;
	EntityId m_id;

	bool m_isPendingDestroy;
};