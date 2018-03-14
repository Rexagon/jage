#pragma once

#include "Mesh.h"
#include "Material.h"
#include "FrameBuffer.h"

class RenderingSystem;

struct RenderCommand
{
	Mesh* mesh;
	mat4 transform;
	Material* material;
};

class RenderCommandBuffer
{
public:
	RenderCommandBuffer(RenderingSystem* renderingSystem);
	~RenderCommandBuffer();

	void push(Mesh* mesh, const mat4& transform, Material* material, FrameBuffer* target = nullptr);
	void clear();

	void sort();

	std::vector<RenderCommand> getDeferredRenderCommands(bool cull = false);
	std::vector<RenderCommand> getAlphaRenderCommands(bool cull = false);
	std::vector<RenderCommand> getCustomRenderCommands(FrameBuffer* target, bool cull = false);
	std::vector<RenderCommand> getPostProcessingRenderCommands();
	std::vector<RenderCommand> getShadowCastRenderCommands();

private:
	static bool deferredSortPredicate(const RenderCommand& a, const RenderCommand& b);
	static bool customSortPredicate(const RenderCommand& a, const RenderCommand& b);

	RenderingSystem* m_renderingSystem;

	std::vector<RenderCommand> m_deferredRenderCommands;
	std::vector<RenderCommand> m_alphaRenderCommands;
	std::vector<RenderCommand> m_postProcessingRenderCommands;
	std::map<FrameBuffer*, std::vector<RenderCommand>> m_customRenderCommands;
};