#include "EntityManager.h"

#include "GameObject.h"

#include "Log.h"

size_t detail::BaseComponent::m_familyCounter = 0;
const EntityId EntityId::INVALID;

EntityManager::EntityManager() :
	m_currentIndex(0)
{
}

size_t EntityManager::getSize() const
{
	return m_entityComponentMasks.size() - m_availableIndices.size();
}

size_t EntityManager::getCapacity() const
{
	return m_entityComponentMasks.size();
}

EntityId EntityManager::createId(uint32_t index) const
{
	return EntityId(index, m_entityVersions[index]);
}

bool EntityManager::isValid(EntityId id) const
{
	return id.getIndex() < m_entityVersions.size() && 
		m_entityVersions[id.getIndex()] == id.getVersion();
}

std::shared_ptr<GameObject> EntityManager::create()
{
	uint32_t index, version;

	if (m_availableIndices.empty()) {
		index = m_currentIndex++;

		// accomodate entity
		if (m_entityComponentMasks.size() <= index) {
			m_entityComponentMasks.resize(index + 1);
			m_entityVersions.resize(index + 1);

			m_gameObjects.resize(index + 1);

			for (auto& pool : m_componentPools) {
				if (pool) {
					pool->expand(index + 1);
				}
			}
		}
		//

		version = m_entityVersions[index] = 1;
	}
	else {
		index = m_availableIndices.back();
		m_availableIndices.pop_back();
		version = m_entityVersions[index];
	}

	std::shared_ptr<GameObject> gameObject = GameObject::create(this, EntityId(index, version));
	m_gameObjects[index] = gameObject;
	
	emit<Events::OnEntityCreated>({ gameObject });

	return gameObject;
}

void EntityManager::destroy(EntityId id)
{
	emit<Events::OnEntityDestroyed>({ get(id) });

	uint32_t index = id.getIndex();
	ComponentMask mask = m_entityComponentMasks[index];
	for (size_t i = 0; i < m_componentHelpers.size(); ++i) {
		auto& helper = m_componentHelpers[i];
		if (helper != nullptr && mask.test(i)) {
			helper->removeComponent(this, id);
		}
	}

	m_entityComponentMasks[index].reset();
	m_entityVersions[index]++;
	m_availableIndices.push_back(index);
}

std::shared_ptr<GameObject> EntityManager::get(EntityId id)
{
	if (id.getIndex() < m_gameObjects.size()) {
		return m_gameObjects[id.getIndex()];
	}
	else {
		return nullptr;
	}
}

EntityManager::ComponentMask EntityManager::getComponentMask(EntityId id)
{
	return m_entityComponentMasks.at(id.getIndex());
}

void EntityManager::registerSystem(std::shared_ptr<EntitySystem> system)
{
	if (system == nullptr) {
		return;
	}

	m_systems.push_back(system);
	system->m_manager = this;
	system->init();
}

void EntityManager::unregisterSystem(std::shared_ptr<EntitySystem> system)
{
	if (system == nullptr) {
		return;
	}

	system->close();
	system->m_manager = nullptr;
	m_systems.erase(std::remove(m_systems.begin(), m_systems.end(), system), m_systems.end());
}
