#include "EntityManager.h"

EntityManager::EntityManager() :
	m_currentEntityId(0)
{
}

EntityManager::~EntityManager()
{
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
}

bool EntityManager::cleanup()
{
	return false;
}

void EntityManager::reset()
{
}

void EntityManager::all(std::function<void(Entity*)> func, bool includePendingDestroy)
{
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
