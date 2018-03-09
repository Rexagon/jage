#include "EntityManager.h"

#include "GameObject.h"

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
	// emit creation event
	return gameObject;
}

std::shared_ptr<GameObject> EntityManager::get(EntityId id)
{
	if (m_gameObjects.size() < id.getIndex()) {
		return m_gameObjects[id.getIndex()];
	}
	else {
		return nullptr;
	}
}
