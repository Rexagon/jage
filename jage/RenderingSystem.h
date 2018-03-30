#pragma once

#include <array>

#include "RenderCommandBuffer.h"
#include "EntityManager.h"
#include "FrameBuffer.h"
#include "GameObject.h"

#include "CameraComponent.h"
#include "LightComponent.h"
#include "MeshComponent.h"

namespace detail
{
	struct PostProcessCommandsPredicate
	{
		bool operator()(const PostProcessCommand& a, const PostProcessCommand& b) {
			return a.order < b.order;
		}
	};
}

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

	void addPostProcess(size_t order, Material* material);

private:
	void renderCustomCommand(const RenderCommand* command, CameraComponent* cameraData, bool affectRenderState = true);
	void renderShadowCastCommand(const RenderCommand* command, LightComponent* lightData);
	void renderPostProcessingCommand(const PostProcessCommand* command);

	ivec2 m_renderSize;

	ComponentHandle<CameraComponent> m_mainCameraData;

	std::shared_ptr<Mesh> m_quad;
	std::unique_ptr<FrameBuffer> m_geometryBuffer;
	std::unique_ptr<FrameBuffer> m_lightBuffer;
	std::unique_ptr<FrameBuffer> m_mainBuffer;

	std::array<std::unique_ptr<FrameBuffer>, 2> m_postProcessBuffers;

	std::unique_ptr<RenderCommandBuffer> m_commandBuffer;
	std::multiset<PostProcessCommand, 
		detail::PostProcessCommandsPredicate> m_postProcessCommands;
};