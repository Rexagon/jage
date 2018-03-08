#pragma once

#include <unordered_map>
#include <functional>
#include <typeindex>
#include <algorithm>
#include <memory>
#include <vector>

class Entity;
class EntityManager;

class World;

namespace detail
{
	template<class T>
	struct ComparationPredicate
	{
		ComparationPredicate(T* ptr) :
			ptr(ptr)
		{}

		template<typename P>
		bool operator()(const std::shared_ptr<P>& other) const
		{
			return other.get() == ptr;
		}

		T* ptr;
	};

	template<class T>
	ComparationPredicate<T> pointerComparator(T* ptr)
	{
		return ComparationPredicate<T>(ptr);
	}

	class BaseEventSubscriber
	{
	public:
		virtual ~BaseEventSubscriber() {}
	};
	
	struct BaseComponentContainer
	{
		virtual ~BaseComponentContainer() {}
	};

	template<class T>
	struct ComponentContainer : public BaseComponentContainer
	{
		ComponentContainer() {}
		ComponentContainer(const T& data) : data(data) {}

		T data;
	};

	template<T>
	class Iterator
	{
	public:
		Iterator(const World* world, size_t index, bool isEnd, bool includePendingDestroy) :
			m_world(world), m_index(index), m_isEnd(isEnd), m_includePendingDestroy(includePendingDestroy)
		{}

		Entity* get() const;

		Entity* operator*() const {
			return get();
		}

		const World* getWorld() const { return m_world; }
		size_t getIndex() const { return m_index; }
		bool isEnd() const { return m_isEnd; }
		bool includePendingDestroy() const { return m_includePendingDestroy; }

		bool operator==(const detail::Iterator<Ts...>& other) const {
			if (m_world != other.m_world) return false;
			if (m_isEnd) return other.m_isEnd;

			return m_index == other.m_index;
		}

		bool operator!=(const detail::Iterator<Ts...>& other) const {
			if (m_world != other.m_world) return true;
			if (m_isEnd) return !other.m_isEnd;

			return m_index != other.m_index;
		}

		Iterator<Ts...>& operator++();

	private:
		const World* m_world;
		size_t m_index;
		bool m_isEnd;
		bool m_includePendingDestroy;
	};


	template<class... Ts>
	class EntityComponentView
	{
	public:
		EntityComponentView(const Iterator<Ts...>& begin, const Iterator<Ts...>& end);

		Iterator<Ts...> begin() { return m_begin; }
		Iterator<Ts...> end() { return m_end; }

	private:
		Iterator<Ts...> m_begin;
		Iterator<Ts...> m_end;
	};
}


template<class T>
class ComponentHandle
{
public:
	ComponentHandle() : m_component(nullptr) {}
	ComponentHandle(T* component) : m_component(component) {}

	bool isValid() const { return m_component != nullptr; }

	T& get() { return *m_component; }
	const T& get() const { return *m_component; }

	T* operator->() const { return m_component; }
	const T* operator->() const { return m_component; }

	operator bool() const { return isValid(); }

private:
	T* m_component;
};


class EntitySystem
{
public:
	virtual ~EntitySystem() {}

	virtual void onUpdate(EntityManager* entityManager, const float dt) {}

	virtual void configure(EntityManager* entityManager) {}
	virtual void unconfigure(EntityManager* entityManager) {}

private:

};


template<class T>
class EventSubscriber : public detail::BaseEventSubscriber
{
public:
	virtual ~EventSubscriber() {}

	virtual void onReceive(EntityManager* entityManager, const T& event) = 0;
};


namespace Events
{
	struct OnEntityCreated
	{
		Entity* entity;
	};

	struct OnEntityDestroyed
	{
		Entity* entity;
	};

	template<class T>
	struct OnComponentAssigned
	{
		Entity* entity;
		ComponentHandle<T> component;
	};
}


class EntityManager
{
public:
	EntityManager();
	~EntityManager();

	void onUpdate(const float dt);

	std::shared_ptr<Entity> create();
	void destroy(Entity* entity, bool immediate = false);

	bool cleanup();

	void reset();

	void registerSystem(std::shared_ptr<EntitySystem> system)
	{
		if (system == nullptr) return;

		m_systems.push_back(system);
		system->configure(this);
	}

	void unregisterSystem(EntitySystem* system)
	{
		if (system == nullptr) return;

		m_systems.erase(std::remove_if(m_systems.begin(), m_systems.end(), detail::pointerComparator(system)), m_systems.end());
		system->unconfigure(this);
	}

	template<class T>
	void subscribe(EventSubscriber<T>* subscriber)
	{
		std::type_index index(typeid(T));
		auto it = m_subscribers.find(index);
		if (it == m_subscribers.end()) {
			m_subscribers.emplace(index, std::initializer_list<detail::BaseEventSubscriber*>{subscriber});
		}
		else {
			it->second.push_back(subscriber);
		}
	}

	template<class T>
	void unsubscribe(EventSubscriber<T>* subscriber)
	{
		std::type_index index(typeid(T));
		auto it = m_subscribers.find(index);
		if (it != m_subscribers.end()) {
			it->second.erase(std::remove(it->second.begin(), it->second.end(), subscriber), it->second.end());
			if (it->second.empty()) {
				m_subscribers.erase(it);
			}
		}
	}

	void unsubscribeAll(void* subscriber)
	{
		for (auto& it : m_subscribers) {
			it.second.erase(std::remove(it.second.begin(), it.second.end(), subscriber), it.second.end());
			if (it.second.empty()) {
				m_subscribers.erase(it.first);
			}
		}
	}

	template<class T>
	void emit(const T& event)
	{
		auto it = m_subscribers.find(std::type_index(typeid(T)));
		if (it != m_subscribers.end()) {
			for (auto& base: it->second) {
				EventSubscriber<T>* subscriber = reinterpret_cast<EventSubscriber<T>*>(base);
				subscriber->onReceive(this, event);
			}
		}
	}

	template<class... Ts>
	void each(typename std::common_type<std::function<void(Entity*, ComponentHandle<Ts>...)>>::type func,
		bool includePendingDestroy = false);

	template<typename... Ts>
	detail::EntityComponentView<Ts...> each(bool includePendingDestroy = false)
	{
		detail::Iterator<Ts...> first(this, 0, false, includePendingDestroy);
		detail::Iterator<Ts...> last(this, getEntityCount(), true, includePendingDestroy);
		return detail::EntityComponentView<Ts...>(first, last);
	}
	
	void all(std::function<void(Entity*)> func, bool includePendingDestroy);

	size_t getEntityCount() const;
	Entity* getByIndex(size_t index) const;

private:
	std::vector<std::shared_ptr<Entity>> m_entities;
	std::vector<std::shared_ptr<EntitySystem>> m_systems;

	std::unordered_map<std::type_index,
		std::vector<detail::BaseEventSubscriber*>,
		std::hash<std::type_index>,
		std::equal_to<std::type_index>> m_subscribers;

	size_t m_currentEntityId;
};


class Entity
{
public:
	Entity(const EntityManager* entityManager, size_t id);
	virtual ~Entity();

	template<class T>
	ComponentHandle<T> get()
	{
		auto it = m_components.find(std::type_index(typeid(T)));
		if (it == m_components.end()) {
			return ComponentHandle<T>(nullptr);
		}
		else {
			return ComponentHandle<T>(&reinterpret_cast<detail::ComponentContainer<T>*>(it->second.get())->data);
		}
	}

	template<class T>
	bool has() const
	{
		return m_components.find(std::type_index(typeid(T))) != m_components.end();
	}

	template<class T1, class T2, class... Ts>
	bool has() const
	{
		return has<T1>() && has<T2, Ts>();
	}

	template<class T, class... Args>
	ComponentHandle<T> assign(Args&&... args)
	{
		detail::ComponentContainer<T>* container;

		std::type_index index(typeid(T));
		auto it = m_components.find(index);
		if (it == m_components.end()) {
			std::shared_ptr<detail::ComponentContainer<T>> containerPtr = std::make_shared<detail::ComponentContainer<T>>(T(args...));
			container = containerPtr.get();
			m_components.insert({ index, containerPtr });
		}
		else {
			container = reinterpret_cast<detail::ComponentContainer<T>*>(it->second.get());
			container->data = T(args...);
		}

		ComponentHandle<T> handle(&container->data);
		m_manager->emit<Events::OnComponentAssigned<T>>({ this, handle });
		return handle;
	}
	
	template<class... Ts>
	bool with(typename std::common_type<std::function<void(ComponentHandle<Ts>...)>>::type func)
	{
		if (!has<Ts...>()) {
			return false;
		}

		func(get<Ts>()...);
		return true;
	}

	const EntityManager* getEntityManager() const;
	size_t getId() const;

	bool isPendingDestroy() const;

private:
	friend class EntityManager;

	std::unordered_map<std::type_index, std::shared_ptr<detail::BaseComponentContainer>> m_components;

	const EntityManager* m_manager;
	size_t m_id;

	int m_pendingDestroy;
};

template<class... Ts>
inline Entity * detail::Iterator<Ts...>::get() const 
{
	return m_world->getByIndex(m_index);
}

template<class ...Ts>
inline detail::Iterator<Ts...>& detail::Iterator<Ts...>::operator++()
{
	++m_index;
	while (m_index < m_world->getEntityCount() && (get() == nullptr || !get()->has<Ts...>() ||
		(get()->isPendingDestroy() && m_includePendingDestroy)))
	{
		++m_index;
	}

	if (m_index >= m_world->getEntityCount()) {
		m_isEnd = true;
	}

	return *this;
}

template<class ...Ts>
inline detail::EntityComponentView<Ts...>::EntityComponentView(const detail::Iterator<Ts...>& begin, 
	const detail::Iterator<Ts...>& end) :
	m_begin(begin), m_end(end)
{
	if (begin.get() == nullptr || (begin.get()->isPendingDestroy() && begin->includePendingDestroy()) ||
		!begin.get()->has<Ts...>())
	{
		++begin;
	}
}

template<class ...Ts>
inline void EntityManager::each(typename std::common_type<std::function<void(Entity*, ComponentHandle<Ts>...)>>::type func, 
	bool includePendingDestroy)
{
	detail::EntityComponentView<Ts...> entities = each<Ts...>(includePendingDestroy);
	for (auto it = entities.begin(); it != entities.end(); ++it) {
		func(it->get(), it->get()->get<Ts>()...);
	}
}
