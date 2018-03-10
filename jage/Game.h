#pragma once

#include "EntityManager.h"
#include "SceneManager.h"

#include "FirstPersonController.h"
#include "CameraComponent.h"
#include "FrameBuffer.h"
#include "Shader.h"
#include "Model.h"
#include "Grid.h"
#include "Mesh.h"

class Game : public BaseScene
{
public:
	void onInit() override;

	void onClose() override;

	void onUpdate(const float dt) override;

	void onDraw(const float dt) override;

	void onResize(const vec2& windowSize) override;

private:
	EntityManager m_entityManager;

	Shader* m_fxaaShader;
	Shader* m_meshShader;
	Shader* m_gridShader;
	Shader* m_skyShader;
	Shader* m_abberationShader;

	object_ptr m_rootObject;

	vec3 m_sunDirection;
	float m_turbidity;
	float m_rayleigh;

	//std::shared_ptr<Grid> m_grid;
	object_ptr m_camera;
	ComponentHandle<CameraComponent> m_cameraData;

	FirstPersonController m_cameraController;

	std::shared_ptr<Mesh> m_quad;
	std::shared_ptr<Mesh> m_sky;
	std::unique_ptr<FrameBuffer> m_framebuffer;
	std::unique_ptr<FrameBuffer> m_postEffectFramebuffer;
};