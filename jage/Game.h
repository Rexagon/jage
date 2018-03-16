#pragma once

#include "EntityManager.h"
#include "SceneManager.h"

#include "FirstPersonController.h"
#include "RenderingSystem.h"
#include "SkySystem.h"

class Game : public BaseScene
{
public:
	void onInit() override;

	void onClose() override;

	void onUpdate(const float dt) override;

	void onResize(const ivec2& windowSize) override;

private:
	std::shared_ptr<EntityManager> m_entityManager;
	std::shared_ptr<RenderingSystem> m_renderingSystem;
	std::shared_ptr<SkySystem> m_skySystem;

	std::shared_ptr<GameObject> m_camera;
	FirstPersonController m_cameraController;
};