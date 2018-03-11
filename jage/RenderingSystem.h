#pragma once

#include "CameraComponent.h"
#include "EntityManager.h"
#include "FrameBuffer.h"
#include "GameObject.h"
#include "Shader.h"
#include "Mesh.h"

namespace Events
{
	struct OnWindowResized
	{
		vec2 windowSize;
	};
}

class RenderingSystem : public EntitySystem, public EventSubscriber<Events::OnWindowResized>
{
public:
	void init() override;
	void close() override;

	void update(const float dt) override;

	void setMainCamera(std::shared_ptr<GameObject> camera);

	void onReceive(EntityManager* manager, const Events::OnWindowResized& event) override;

private:
	void loadShaders();
	void updateCamera();

	void drawSky();
	void drawScene();

	// shaders
	Shader* m_fxaaShader;
	Shader* m_meshShader;
	Shader* m_gridShader;
	Shader* m_skyShader;
	Shader* m_abberationShader;

	// lighting
	vec3 m_sunDirection;
	float m_turbidity;
	float m_rayleigh;

	// sky
	std::shared_ptr<Mesh> m_sky;

	// post effects
	std::shared_ptr<Mesh> m_quad;
	std::unique_ptr<FrameBuffer> m_framebuffer;
	std::unique_ptr<FrameBuffer> m_postEffectFramebuffer;

	// camera
	ComponentHandle<CameraComponent> m_mainCameraData;
};