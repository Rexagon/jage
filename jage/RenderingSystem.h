#pragma once

#include "RenderCommandBuffer.h"
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
		ivec2 windowSize;
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

	vec2 m_renderSize;

	// post effects
	std::shared_ptr<Mesh> m_quad;
	std::unique_ptr<FrameBuffer> m_geometryBuffer;

	// camera
	ComponentHandle<CameraComponent> m_mainCameraData;
	
	std::shared_ptr<RenderCommandBuffer> m_commandBuffer;
};