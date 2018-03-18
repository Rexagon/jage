#pragma once

#include "RenderCommandBuffer.h"
#include "EntityManager.h"
#include "FrameBuffer.h"
#include "GameObject.h"

#include "CameraComponent.h"
#include "LightComponent.h"
#include "MeshComponent.h"

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
	void renderCustomCommand(RenderCommand* command, CameraComponent* cameraData, bool affectRenderState = true);
	void renderShadowCastCommand(RenderCommand* command, LightComponent* lightData);

	void renderDeferredAmbient();
	void renderDeferredDirectional(LightComponent* component);
	void renderDeferredPoint(LightComponent* command);
	void renderDeferredSpot(LightComponent* spot);

	ivec2 m_renderSize;

	ComponentHandle<CameraComponent> m_mainCameraData;

	std::shared_ptr<Mesh> m_quad;
	std::unique_ptr<FrameBuffer> m_geometryBuffer;
	std::unique_ptr<FrameBuffer> m_lightBuffer;
	std::unique_ptr<FrameBuffer> m_mainBuffer;

	std::unique_ptr<RenderCommandBuffer> m_commandBuffer;

	Shader* m_mainShader;
};