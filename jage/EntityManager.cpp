#include "EntityManager.h"

// Entity //
///////////
Entity::Entity()
{
}

Entity::Entity(EntityManager * manager, Entity::Id id)
{
}

void Entity::destroy()
{
}

void Entity::invalidate()
{
}

std::bitset<MAX_COMPONENTS> Entity::getComponentMask() const
{
	return std::bitset<MAX_COMPONENTS>();
}


// EntityManager //
//////////////////
EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
}

Entity EntityManager::create()
{
	uint32_t index, version;
	if (m_freeEntitiesList.empty()) {
		index = m_indexCounter++;
		accomodateEntity(index);
		version = m_entityVersions[index] = 1;
	}
	else {
		index = m_freeEntitiesList.back();
		m_freeEntitiesList.pop_back();
		version = m_entityVersions[index];
	}

	Entity entity(this, Entity::Id(index, version));
	// TODO: emit entity created event
	return entity;
}

Entity EntityManager::createFromCopy(Entity original)
{
	assert(original.isValid());
	Entity clone = create();
	ComponentMask mask = original.getComponentMask();
	for (size_t i = 0; i < m_componentHelpers.size(); ++i) {
		BaseComponentHelper* helper = m_componentHelpers[i];
		if (helper != nullptr && mask.test(i)) {
			helper->copyComponentTo(original, clone);
		}
	}
}

void EntityManager::destroy(Entity::Id entityId)
{
	assertValid(entityId);

	uint32_t index = entityId.getIndex();
	ComponentMask mask = m_entityComponentMasks[index];

	for (size_t i = 0; i < m_componentHelpers.size(); ++i) {
		BaseComponentHelper* helper = m_componentHelpers[i];
		if (helper != nullptr && mask.test(i)) {
			helper->removeComponent(Entity(this, entityId));
		}
	}

	// TODO: emit entity destroyed event
	m_entityComponentMasks[index].reset();
	m_entityVersions[index]++;
	m_freeEntitiesList.push_back(index);
}

Entity EntityManager::get(Entity::Id id)
{
	assertValid(id);
	return Entity(this, id);
}

size_t EntityManager::getSize() const
{
	return m_entityComponentMasks.size() - m_freeEntitiesList.size();
}

size_t EntityManager::getCapacity() const
{
	return m_entityComponentMasks.size();
}

void EntityManager::assertValid(Entity::Id id) const
{
	assert(id.getIndex() < m_entityComponentMasks.size() && "Entity::Id ID outside entity vector range");
	assert(m_entityVersions[id.getIndex()] == id.getVersion() && "Attempt to access Entity via a stale Entity::Id");
}

EntityManager::ComponentMask EntityManager::getComponentMask(Entity::Id id)
{
	assertValid(id);
	return m_entityComponentMasks[id.getIndex()];
}

void EntityManager::accomodateEntity(uint32_t index)
{
	if (m_entityComponentMasks.size() <= index) {
		m_entityComponentMasks.resize(index + 1);
		m_entityVersions.resize(index + 1);
		for (BasePool* pool : m_componentPools) {
			if (pool) {
				pool->expand(index + 1);
			}
		}
	}
}

Entity::Id EntityManager::createId(uint32_t index) const
{
	return Entity::Id(index, m_entityVersions[index]);
}

bool EntityManager::isValid(Entity::Id id) const
{
	return id.getIndex() < m_entityVersions.size() && 
		m_entityVersions[id.getIndex()] == id.getVersion();
}