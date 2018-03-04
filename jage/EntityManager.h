#pragma once

#include <cstdint>
#include <cassert>
#include <bitset>
#include <vector>

#include "Math.h"
#include "Pool.h"

#define MAX_COMPONENTS 64

class EntityManager;

template<class C, class EM = EntityManager>
class ComponentHandle;


class Entity
{
public:
	struct Id
	{
		Id() : m_id(0) {}
		Id(uint64_t id) : m_id(id) {}
		Id(uint32_t index, uint32_t version) : m_id(uint64_t(index) | (uint64_t(version) << 32UL)) {}

		uint64_t getId() const { return m_id; }
		uint32_t getIndex() const { return m_id & 0xffffffffUL; }
		uint32_t getVersion() const { return m_id >> 32; }

		bool operator==(const Id& other) const { return m_id == other.m_id; }
		bool operator!=(const Id& other) const { return m_id != other.m_id; }
		bool operator<(const Id& other) const { return m_id < other.m_id; }
		bool operator>(const Id& other) const { return m_id > other.m_id; }
	private:
		friend class EntityManager;
		uint64_t m_id;
	};

	static const Id INVALID;

	Entity() = default;
	Entity(EntityManager* manager, Entity::Id id);

	void destroy();
	void invalidate();

	template<class C, class... Args>
	ComponentHandle<C> assign(Args&&... args);

	template<class C>
	ComponentHandle<C> assignFromCopy(const C& component);

	template<class C, class... Args>
	ComponentHandle<C> replace(Args&&... args);

	template<class C>
	void remove();

	template<class C, class = typename std::enable_if<!std::is_const<C>::value>::type>
	ComponentHandle<C> getComponent();

	template<class C, class = typename std::enable_if<std::is_const<C>::value>::type>
	const ComponentHandle<C, const EntityManager> getComponent() const;

	template<class... Cs>
	std::tuple<ComponentHandle<Cs>...> getComponents();

	template<class... Cs>
	std::tuple<ComponentHandle<const Cs, const EntityManager>...> getComponents() const;

	template<class C>
	bool hasComponent() const;

	template<class C, class... Cs>
	void unpack(ComponentHandle<C>& c, ComponentHandle<Cs>&... cs);

	bool isValid() const;
	Id getId() const;

	std::bitset<MAX_COMPONENTS> getComponentMask() const;

	bool operator==(const Entity& other) const;
	bool operator!=(const Entity& other) const;
	bool operator<(const Entity& other) const;

private:
	EntityManager * m_manager = nullptr;
	Id m_id;
};


template<class C, class EM>
class ComponentHandle
{
public:
	typedef C ComponentType;

	ComponentHandle() : m_manager(nullptr) {}

	void remove();
	Entity getEntity() const;

	bool isValid() const;

	C* operator->();
	const C* operator->() const;

	C& operator*();
	const C& operator*() const;

	C* get();
	const C* get() const;

	bool operator==(const ComponentHandle<C, EM> &other) const
	{
		return m_manager == other.m_manager && m_id == other.m_id;
	}

	bool operator!=(const ComponentHandle<C, EM> &other) const
	{
		return m_manager != other.m_manager || m_id != other.m_id;
	}

private:
	friend class EntityManager;

	ComponentHandle(EM* manager, Entity::Id id) :
		m_manager(manager), m_id(id)
	{}

	EM* m_manager;
	Entity::Id m_id;
};


class BaseComponent
{
public:
	typedef size_t Family;

	void operator delete(void *p) { fail(); }
	void operator delete[](void *p) { fail(); }

protected:
	static void fail() { throw std::bad_alloc(); }

	static Family m_familyCounter;
};


template<class C>
struct Component : public BaseComponent
{
public:
	typedef ComponentHandle<C> Handle;
	typedef ComponentHandle<const C, const EntityManager> ConstHandle;

private:
	friend class EntityManager;

	static Family getFamily();
};


class BaseComponentHelper
{
public:
	virtual ~BaseComponentHelper() {}
	virtual void removeComponent(Entity e) = 0;
	virtual void copyComponentTo(Entity source, Entity target) = 0;
};


template<class C>
class ComponentHelper
{
public:
	void removeComponent(Entity e) override
	{
		e.remove<C>();
	}

	void copyComponentTo(Entity source, Entity target) override
	{
		target.assignFromCopy(*source.getComponent<C>().get());
	}
};


class EntityManager
{
public:
	typedef std::bitset<MAX_COMPONENTS> ComponentMask;

	EntityManager();
	virtual ~EntityManager();

	Entity create();
	Entity createFromCopy(Entity original);
	void destroy(Entity::Id entityId);
	Entity get(Entity::Id id);

	Entity::Id createId(uint32_t index) const;
	bool isValid(Entity::Id id) const;

	size_t getSize() const;
	size_t getCapacity() const;

	template<class C>
	static BaseComponent::Family getComponentFamily() {
		return Component<typename std::remove_const<C>::type>::getFamily();
	}
private:
	void assertValid(Entity::Id id) const;

	template<class C>
	C* getComponentPtr(Entity::Id id)
	{
		assertValid(id);
		BasePool* pool = m_componentPools[getComponentFamily<C>()];
		assert(pool);
		return static_cast<C*>(pool->get(id.getIndex()));
	}

	template<class C>
	const C* getComponentPtr(Entity::Id id)
	{
		assertValid(id);
		BasePool* pool = m_componentPools[getComponentFamily<C>()];
		assert(pool);
		return static_cast<const C*>(pool->get(id.getIndex()));
	}

	template<class C>
	ComponentMask getComponentMask()
	{
		ComponentMask mask;
		mask.set(getComponentFamily<C>());
		return mask;
	}

	template<class C1, class C2, class... Cs>
	ComponentMask getComponentMask()
	{
		return getComponentMask<C1>() | getComponentMask<C2, Cs>();
	}

	template<class C>
	ComponentMask getComponentMask(const ComponentHandle<C>& c)
	{
		return getComponentMask<C>();
	}

	template<class C, class... Cs>
	ComponentMask getComponentMask(const ComponentHandle<C>& c, const ComponentHandle<Cs>&... cs)
	{
		return getComponentMask<C, Cs...>();
	}

	ComponentMask getComponentMask(Entity::Id id);

	void accomodateEntity(uint32_t index);
	
	template<class C>
	Pool<C>* accomodateComponent()
	{
		BaseComponent::Familt family = getComponentFamilt<C>();
		if (m_componentPools.size() <= family) {
			m_componentPools.resize(family + 1, nullptr);
		}

		if (m_componentPools[family] == nullptr) {
			Pool<C>* pool = new Pool<C>();
			pool->expand(m_indexCounter);
			m_componentPools[family] = pool;
		}

		if (m_componentHelpers[family].size() <= family) {
			m_componentHelpers.resize(family + 1, nullptr);
		}

		if (m_componentHelpers[family] == nullptr) {
			ComponentHelper<C>* helper = new ComponentHelper<C>();
			m_componentHelpers[family] = helper;
		}
		return static_cast<Pool<C>*>(m_componentPools[family]);
	}

	uint32_t m_indexCounter;

	std::vector<BasePool*> m_componentPools;
	std::vector<BaseComponentHelper*> m_componentHelpers;
	std::vector<ComponentMask> m_entityComponentMasks;
	std::vector<uint32_t> m_entityVersions;
	std::vector<uint32_t> m_freeEntitiesList;
};

template<class C>
BaseComponent::Family Component<C>::getFamily()
{
	Family family = m_familyCounter++;
	assert(family < EntityManager::MAX_COMPONENTS);
	return family;
}
