#pragma once

#include <set>
#include <map>

#include "Mesh.h"
#include "Material.h"
#include "FrameBuffer.h"

class RenderingSystem;

struct RenderCommand
{
	RenderCommand() :
		mesh(nullptr), transform(1.0f), material(nullptr)
	{}

	RenderCommand(Mesh* mesh, const mat4& transform, Material* material) :
		mesh(mesh), transform(transform), material(material)
	{}

	Mesh* mesh;
	mat4 transform;
	Material* material;
};

struct PostProcessCommand
{
	PostProcessCommand() :
		order(0), material(nullptr)
	{}

	PostProcessCommand(size_t order, Material* material) :
		order(order), material(material)
	{}

	size_t order;
	Material* material;
};

class RenderCommandBuffer
{
	struct PostProcessCommandsPredicate
	{
		bool operator()(const PostProcessCommand& a, const PostProcessCommand& b) {
			return a.order < b.order;
		}
	};

public:
	RenderCommandBuffer(RenderingSystem* renderingSystem);
	~RenderCommandBuffer();

	void push(Mesh* mesh, const mat4& transform, Material* material, FrameBuffer* target = nullptr);
	void clear();

	void sort();

	std::vector<RenderCommand> getDeferredRenderCommands(bool cull = false);
	std::vector<RenderCommand> getAlphaRenderCommands(bool cull = false);
	std::vector<RenderCommand> getCustomRenderCommands(FrameBuffer* target, bool cull = false);
	std::vector<RenderCommand> getShadowCastRenderCommands();

private:
	static bool deferredSortPredicate(const RenderCommand& a, const RenderCommand& b);
	static bool customSortPredicate(const RenderCommand& a, const RenderCommand& b);

	RenderingSystem* m_renderingSystem;

	std::vector<RenderCommand> m_deferredRenderCommands;
	std::vector<RenderCommand> m_alphaRenderCommands;
	std::map<FrameBuffer*, std::vector<RenderCommand>> m_customRenderCommands;
};