#include "EntityManager.h"

EntityManager::EntityManager() :
	m_currentEntityId(0)
{
}

EntityManager::~EntityManager()
{
	for (auto entity : m_entities)
	{
		if (!entity->isPendingDestroy())
		{
			entity->m_pendingDestroy = true;
			emit<Events::OnEntityDestroyed>({ entity.get() });
		}
	}

	for (auto system : m_systems)
	{
		system->unconfigure(this);
	}
}

void EntityManager::onUpdate(const float dt)
{
	cleanup();

	for (auto& system : m_systems) {
		system->onUpdate(this, dt);
	}
}

std::shared_ptr<Entity> EntityManager::create()
{
	++m_currentEntityId;
	std::shared_ptr<Entity> entity = std::make_shared<Entity>(this, m_currentEntityId);
	m_entities.push_back(entity);

	emit<Events::OnEntityCreated>({ entity.get() });

	return entity;
}

void EntityManager::destroy(Entity * entity, bool immediate)
{
	if (entity == nullptr) return;

	if (!entity->isPendingDestroy()) {
		entity->m_pendingDestroy = true;
		emit<Events::OnEntityDestroyed>({ entity });
	}

	if (immediate) {
		m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), detail::pointerComparator(entity)), m_entities.end());
	}
}

bool EntityManager::cleanup()
{
	size_t count = 0;
	m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(), [&, this](Entity* entity) {
		if (entity->isPendingDestroy()) {
			++count;
			return true;
		}

		return false;
	}), m_entities.end());

	return count > 0;
}

void EntityManager::reset()
{
	for (auto entity : m_entities)
	{
		if (!entity->isPendingDestroy())
		{
			entity->m_pendingDestroy = true;
			emit<Events::OnEntityDestroyed>({ entity.get() });
		}
	}

	m_entities.clear();
	m_currentEntityId = 0;
}

void EntityManager::all(std::function<void(Entity*)> func, bool includePendingDestroy)
{
}

size_t EntityManager::getEntityCount() const
{
	return m_entities.size();
}

Entity * EntityManager::getByIndex(size_t index) const
{
	if (index < m_entities.size()) {
		return m_entities[index].get();
	}
	else {
		return nullptr;
	}
}



Entity::Entity(const EntityManager * entityManager, size_t id) :
	m_manager(entityManager), m_id(id), m_pendingDestroy(false)
{
}

Entity::~Entity()
{
}

const EntityManager * Entity::getEntityManager() const
{
	return m_manager;
}

size_t Entity::getId() const
{
	return m_id;
}

bool Entity::isPendingDestroy() const
{
	return m_pendingDestroy;
}
