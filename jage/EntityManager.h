#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <bitset>

#include "Pool.h"

#define MAX_COMPONENTS 64

class GameObject;
class EntityManager;


class EntityId
{
public:
	static const EntityId INVALID;

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


namespace detail
{
	class BaseComponent
	{
	public:
		virtual ~BaseComponent() {}

		void operator delete(void *p) { fail(); }
		void operator delete[](void *p) { fail(); }
	protected:
		static void fail()
		{
			throw std::bad_alloc();
		}

		static size_t m_familyCounter;
	};

	class BaseComponentHelper
	{
	public:
		virtual ~BaseComponentHelper() {}
		virtual void removeComponent(EntityManager* manager, EntityId id) = 0;
		virtual void copyComponentTo(EntityManager* manager, EntityId source, EntityId target) = 0;
	};

	template<typename T>
	class ComponentHelper : public BaseComponentHelper
	{
	public:
		void removeComponent(EntityManager* manager, EntityId id) override;
		void copyComponentTo(EntityManager* manager, EntityId source, EntityId target) override;
	};
}


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

	ComponentHandle(EntityManager* manager, EntityId entityId) :
		m_manager(manager), m_entityId(entityId)
	{}
		
	EntityManager* m_manager;
	EntityId m_entityId;
};


namespace detail
{
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
}


class EntityManager
{
public:
	typedef std::bitset<MAX_COMPONENTS> ComponentMask;

	EntityManager();

	EntityId createId(uint32_t index) const;
	bool isValid(EntityId id) const;

	std::shared_ptr<GameObject> create();
	void destroy(EntityId id);

	std::shared_ptr<GameObject> get(EntityId id);

	template<typename T, typename... Args>
	ComponentHandle<T> assign(EntityId id, Args&&... args)
	{
		size_t family = getComponentFamily<T>();
		
		// accomodate component
		while (m_componentPools.size() <= family) {
			m_componentPools.emplace_back(nullptr);
		}

		while (m_componentHelpers.size() <= family) {
			m_componentHelpers.emplace_back(nullptr);
		}

		auto& pool = m_componentPools[family];
		if (pool == nullptr) {
			pool = std::move(std::make_unique<Pool<T>>());
			pool->expand(m_currentIndex);
		}

		auto& helper = m_componentHelpers[family];
		if (helper == nullptr) {
			helper = std::move(std::make_unique<detail::ComponentHelper<T>>());
		}
		//

		::new(pool->get(id.getIndex())) T(std::forward<Args>(args)...);
		
		m_entityComponentMasks[id.getIndex()].set(family);
		ComponentHandle<T> component(this, id);
		// emit component assignment event
		return component;
	}

	template<typename T>
	void remove(EntityId id)
	{
		size_t family = getComponentFamily<T>();
		uint32_t index = id.getIndex();

		// create and emit component removement event

		m_entityComponentMasks[index].reset(family);

		auto& pool = m_componentPools[family];
		pool->destroy(index);
	}

	template<typename T>
	bool hasComponent(EntityId id) const 
	{
		size_t family = getComponentFamily<T>();

		if (family >= m_componentPools.size()) {
			return false;
		}

		auto& pool = m_componentPools[family];
		if (pool == nullptr || !m_entityComponentMasks[id.getIndex()][family]) {
			return false;
		}

		return true;
	}

	template<typename T, typename = typename std::enable_if<!std::is_const<T>::value>::type>
	ComponentHandle<T> getComponent(EntityId id)
	{
		size_t family = getComponentFamily<T>();

		if (family >= m_componentPools.size()) {
			return ComponentHandle<T>();
		}

		auto& pool = m_componentPools[family];
		if (pool == nullptr || !m_entityComponentMasks[id.getIndex()][family]) {
			return ComponentHandle<T>();
		}

		return ComponentHandle<T>(this, id);
	}

	template<typename T, typename = typename std::enable_if<std::is_const<T>::value>::type>
	const ComponentHandle<T> getComponent(EntityId id) const
	{
		size_t family = getComponentFamily<T>();

		if (family >= m_componentPools.size()) {
			return ComponentHandle<T>();
		}

		auto& pool = m_componentPools[family];
		if (pool == nullptr || !m_entityComponentMasks[id.getIndex()][family]) {
			return ComponentHandle<T>();
		}

		return ComponentHandle<T>(this, id);
	}

	template<typename... Ts>
	std::tuple<ComponentHandle<Ts>...> getComponents(EntityId id)
	{
		return std::make_tuple(getComponent<Ts>(id)...);
	}

	template<typename... Ts>
	std::tuple<ComponentHandle<const Ts>...> getComponents(EntityId id) const
	{
		return std::make_tuple(getComponent<const Ts>(id)...);
	}

	template<typename T>
	void unpack(EntityId id, ComponentHandle<T>& c)
	{
		c = getComponent<T>();
	}

	template<typename T, typename... Ts>
	void unpack(EntityId id, ComponentHandle<T>& c, ComponentHandle<Ts>&... cs)
	{
		c = getComponent<T>();
		unpack<Ts...>(id, cs...);
	}

	template<typename T>
	static size_t getComponentFamily()
	{
		return detail::Component<typename std::remove_const<T>::type>::getFamily();
	}

	ComponentMask getComponentMask(EntityId id);

	template<typename T>
	ComponentMask getComponentMask() 
	{
		ComponentMask mask;
		mask.set(getComponentFamily<T>());
		return mask;
	}

	template<typename T1, typename T2, typename... Ts>
	ComponentMask getComponentMask() 
	{
		return getComponentMask<T1>() | getComponentMask<T2, Ts...>();
	}

	template<typename T>
	ComponentMask getComponentMask(const ComponentHandle<T>& c) 
	{
		return getComponentMask<T>();
	}

	template<typename T1, typename... Ts>
	ComponentMask getComponentMask(const ComponentHandle<T1>& c1, const ComponentHandle<Ts>&... args) 
	{
		return getComponentMask<T1, Ts...>();
	}

private:
	template<typename T>
	friend class ComponentHandle;

	template<typename T>
	T* getComponentPtr(EntityId id)
	{
		BasePool* pool = m_componentPools[getComponentFamily<T>()].get();
		return reinterpret_cast<T*>(pool->get(id.getIndex()));
	}

	template<typename T>
	const T* getComponentPtr(EntityId id) const
	{
		BasePool* pool = m_componentPools[getComponentFamily<T>()].get();
		return reinterpret_cast<const T*>(pool->get(id.getIndex()));
	}

	uint32_t m_currentIndex;

	std::vector<std::unique_ptr<BasePool>> m_componentPools;
	std::vector<std::unique_ptr<detail::BaseComponentHelper>> m_componentHelpers;
	std::vector<ComponentMask> m_entityComponentMasks;
	std::vector<uint32_t> m_entityVersions;

	std::vector<std::shared_ptr<GameObject>> m_gameObjects;

	std::vector<uint32_t> m_availableIndices;
};


template<typename T>
inline void detail::ComponentHelper<T>::removeComponent(EntityManager* manager, EntityId id)
{
	manager->remove<T>(id);
}

template<typename T>
inline void detail::ComponentHelper<T>::copyComponentTo(EntityManager* manager, EntityId source, EntityId target)
{
	manager->assign<T>(source, std::forward<const T&>(*(manager->getComponent<T>(source).get())));
}


template<typename T>
inline void ComponentHandle<T>::remove()
{
	m_manager->remove<T>(m_entityId);
}

template<typename T>
inline std::shared_ptr<GameObject> ComponentHandle<T>::getEntity()
{
	return m_manager->get(m_entityId);
}

template<typename T>
inline bool ComponentHandle<T>::isValid() const
{
	return m_manager != nullptr && m_manager->isValid(m_entityId) &&
		m_manager->hasComponent<T>(m_entityId);
}

template<typename T>
inline T * ComponentHandle<T>::operator->()
{
	if (isValid()) {
		return m_manager->getComponentPtr<T>(m_entityId);
	}
	else {
		return nullptr;
	}
}

template<typename T>
inline const T * ComponentHandle<T>::operator->() const
{
	if (isValid()) {
		return m_manager->getComponentPtr<T>(m_entityId);
	}
	else {
		return nullptr;
	}
}

template<typename T>
inline T * ComponentHandle<T>::get()
{
	if (isValid()) {
		return m_manager->getComponentPtr<T>(m_entityId);
	}
	else {
		return nullptr;
	}
}

template<typename T>
inline const T * ComponentHandle<T>::get() const
{
	if (isValid()) {
		return m_manager->getComponentPtr<T>(m_entityId);
	}
	else {
		return nullptr;
	}
}