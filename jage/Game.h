#pragma once

#include "EntityManager.h"
#include "SceneManager.h"

#include "FirstPersonController.h"
#include "RenderingSystem.h"

class Game : public BaseScene
{
public:
	void onInit() override;

	void onClose() override;

	void onUpdate(const float dt) override;

	void onResize(const vec2& windowSize) override;

private:
	EntityManager m_entityManager;
	std::shared_ptr<RenderingSystem> m_renderingSystem;

	std::shared_ptr<GameObject> m_camera;
	FirstPersonController m_cameraController;
};