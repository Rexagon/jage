#include "SceneManager.h"

std::stack<std::unique_ptr<Scene>> SceneManager::m_scenes;

void SceneManager::deleteScene()
{
	if (!m_scenes.empty()) {
		m_scenes.top()->onClose();
		m_scenes.pop();
	}

	if (!m_scenes.empty()) {
		m_scenes.top()->onReturn();
	}
}

Scene * SceneManager::getCurrentScene()
{
	if (m_scenes.empty()) {
		return nullptr;
	}
	else {
		return m_scenes.top().get();
	}
}

void SceneManager::close()
{
	while (!m_scenes.empty()) {
		deleteScene();
	}
}
