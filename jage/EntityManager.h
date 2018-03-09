#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <bitset>

#include "Pool.h"

#define MAX_COMPONENTS 64

class GameObject;
class EntityManager;

namespace detail
{	
	class BaseComponent
	{
	public:
		virtual ~BaseComponent() {}

	private:
		static size_t m_familyCounter;
	};
}


class EntityId
{
public:
	EntityId() : m_id(0) {}
	EntityId(uint64_t id) : m_id(id) {}

	EntityId(uint32_t index, uint32_t version) :
		m_id(uint64_t(index) | uint64_t(version) << 32UL)
	{}

	uint64_t getId() const { return m_id; }
	uint32_t getIndex() const { return m_id & 0xffffffffUL; }
	uint32_t getVersion() const { return m_id >> 32; }

	bool operator==(const EntityId &other) const
	{
		return m_id == other.m_id;
	}

	bool operator!=(const EntityId &other) const
	{
		return m_id != other.m_id;
	}

	bool operator<(const EntityId &other) const
	{
		return m_id < other.m_id;
	}

private:
	friend class EntityManager;

	uint64_t m_id;
};


template<typename T>
class ComponentHandle
{
public:
	typedef T ComponentType;

	ComponentHandle() : m_manager(nullptr) {}

	void remove();
	std::shared_ptr<GameObject> getEntity();

	bool isValid() const;

	T* operator->();
	const T* operator->() const;

	T* get();
	const T* get() const;

	operator bool() const
	{
		return isValid();
	}

	bool operator==(const ComponentHandle<T> &other) const 
	{
		return m_manager == other.m_manager && m_id == other.m_id;
	}

	bool operator!=(const ComponentHandle<T> &other) const 
	{
		return m_manager != other.m_manager || m_id != other.m_id;
	}
	
private:
	friend class EntityManager;

	ComponentHandle(const EntityManager* manager, EntityId entityId) :
		m_manager(manager), m_entityId(entityId)
	{}
		
	const EntityManager* m_manager;
	EntityId m_entityId;
};


template<typename T>
class Component : public detail::BaseComponent
{
public:
	typedef ComponentHandle<T> Handle;
	typedef ComponentHandle<const T> ConstHandle;

	static size_t getFamily()
	{
		static size_t family = m_familyCounter++;
		return family;
	}
};


class EntityManager
{
public:
	typedef std::bitset<MAX_COMPONENTS> ComponentMask;

	bool isValid(EntityId id) const;

	std::shared_ptr<GameObject> create();

	std::shared_ptr<GameObject> get(EntityId id);

	template<typename T, typename... Args>
	ComponentHandle<T> assign(EntityId id, Args&&... args)
	{
		size_t family = getComponentFamily<T>();
		
		// accomodate component
		if (m_componentPools.size() <= family) {
			m_componentPools.resize(family + 1, nullptr);
		}

		auto& pool = m_componentPools[family];
		if (pool == nullptr) {
			pool = std::make_unique<Pool<T>>();
			pool->expand(m_currentIndex);
		}
		//

		::new(pool->get(id.getIndex())) T(std::forward<Args>(args)...);
		
		m_entityComponentMasks[id.getIndex()].set(family);
		ComponentHandle<T> component(this, id);
		// emite component assignment event
		return component;
	}

	template<typename T>
	bool hasComponent(EntityId id) const 
	{
		size_t family = getComponentFamily<T>();

		if (family >= m_componentPools.size()) {
			return false;
		}

		auto& pool = m_componentPools[family];
		if (pool == nullptr || !m_entityComponentMasks[id.index()][family]) {
			return false;
		}

		return true;
	}

	template<typename T>
	ComponentHandle<T> getComponent(EntityId id)
	{
		size_t family = getComponentFamily<T>();

		if (family >= m_componentPools.size()) {
			return ComponentHandle<T>();
		}

		auto& pool = m_componentPools[family];
		if (pool == nullptr || !m_entityComponentMasks[id.index()][family]) {
			return ComponentHandle<T>();
		}

		return ComponentHandle<T>(this, id);
	}

	template<typename T>
	static size_t getComponentFamily()
	{
		return Component<typename std::remove_const<T>::type>::getFamily();
	}

private:
	template<typename T>
	friend class ComponentHandle;

	template<typename T>
	T* getComponentPtr(EntityId id)
	{
		auto& pool = m_componentPools[getComponentFamily<T>()];
		return reinterpret_cast<T*>(pool->get(id.getIndex()));
	}

	template<typename T>
	const T* getComponentPtr(EntityId id) const
	{
		auto& pool = m_componentPools[getComponentFamily<T>()];
		return reinterpret_cast<const T*>(pool->get(id.getIndex()));
	}

	uint32_t m_currentIndex;

	std::vector<std::unique_ptr<BasePool>> m_componentPools;
	std::vector<ComponentMask> m_entityComponentMasks;
	std::vector<uint32_t> m_entityVersions;

	std::vector<std::shared_ptr<GameObject>> m_gameObjects;

	std::vector<uint32_t> m_availableIndices;
};

template<typename T>
inline void ComponentHandle<T>::remove()
{
	//TODO: mark as pending remove
}

template<typename T>
inline std::shared_ptr<GameObject> ComponentHandle<T>::getEntity()
{
	return m_manager->get(m_entityId);
}

template<typename T>
inline bool ComponentHandle<T>::isValid() const
{
	return m_manager != nullptr && m_manager->isValid(id) &&
		m_manager->hasComponent<T>(m_entityId);
}

template<typename T>
inline T * ComponentHandle<T>::operator->()
{
	if (isValid()) {
		return m_manager->getComponentPtr(m_entityId);
	}
	else {
		return nullptr;
	}
}

template<typename T>
inline const T * ComponentHandle<T>::operator->() const
{
	if (isValid()) {
		return m_manager->getComponentPtr(m_entityId);
	}
	else {
		return nullptr;
	}
}

template<typename T>
inline T * ComponentHandle<T>::get()
{
	if (isValid()) {
		return m_manager->getComponentPtr(m_entityId);
	}
	else {
		return nullptr;
	}
}

template<typename T>
inline const T * ComponentHandle<T>::get() const
{
	if (isValid()) {
		return m_manager->getComponentPtr(m_entityId);
	}
	else {
		return nullptr;
	}
}
