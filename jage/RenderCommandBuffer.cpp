#include "RenderCommandBuffer.h"

RenderCommandBuffer::RenderCommandBuffer(RenderingSystem * renderingSystem) :
	m_renderingSystem(renderingSystem)
{
}

RenderCommandBuffer::~RenderCommandBuffer()
{
	clear();
}

void RenderCommandBuffer::push(Mesh * mesh, const mat4 & transform, Material * material, FrameBuffer * target)
{
	if (mesh == nullptr || material == nullptr) return;

	RenderCommand command{ mesh, transform, material };

	if (material->isBlendingEnabled()) {
		material->setType(Material::CUSTOM);
		m_alphaRenderCommands.push_back(command);
	}
	else {
		switch (material->getType())
		{
		case Material::DEFAULT:
			m_deferredRenderCommands.push_back(command);
			break;

		case Material::CUSTOM:
		{
			auto it = m_customRenderCommands.find(target);
			if (it != m_customRenderCommands.end()) {
				it->second.push_back(command);
			}
			else {
				m_customRenderCommands.insert({ target, std::vector<RenderCommand>{ command } });
			}
			break;
		}

		case Material::POST_PROCESS:
			m_postProcessingRenderCommands.push_back(command);
			break;
		}
	}
}

void RenderCommandBuffer::clear()
{
	m_deferredRenderCommands.clear();
	m_alphaRenderCommands.clear();
	m_postProcessingRenderCommands.clear();
	m_customRenderCommands.clear();
}

void RenderCommandBuffer::sort()
{
	std::sort(m_deferredRenderCommands.begin(), m_deferredRenderCommands.end(), deferredSortPredicate);
	for (auto it = m_customRenderCommands.begin(); it != m_customRenderCommands.end(); it++) {
		std::sort(it->second.begin(), it->second.end(), customSortPredicate);
	}
}

std::vector<RenderCommand> RenderCommandBuffer::getDeferredRenderCommands(bool cull)
{
	//TODO: make frustrum culling
	return m_deferredRenderCommands;
}

std::vector<RenderCommand> RenderCommandBuffer::getAlphaRenderCommands(bool cull)
{
	//TODO: make frustrum culling
	return m_alphaRenderCommands;
}

std::vector<RenderCommand> RenderCommandBuffer::getCustomRenderCommands(FrameBuffer * target, bool cull)
{
	//TODO: make frustrum culling
	return m_customRenderCommands[target];
}

std::vector<RenderCommand> RenderCommandBuffer::getPostProcessingRenderCommands()
{
	return m_postProcessingRenderCommands;
}

std::vector<RenderCommand> RenderCommandBuffer::getShadowCastRenderCommands()
{
	std::vector<RenderCommand> commands;

	for (auto it = m_deferredRenderCommands.begin(); it != m_deferredRenderCommands.end(); ++it) {
		if (it->material->isShadowCastingEnabled()) {
			commands.push_back(*it);
		}
	}

	auto screenTarget = m_customRenderCommands.find(nullptr);
	if (screenTarget != m_customRenderCommands.end()) {
		for (auto it = screenTarget->second.begin(); it != screenTarget->second.end(); ++it) {
			if (it->material->isShadowCastingEnabled()) {
				commands.push_back(*it);
			}
		}
	}

	return commands;
}

bool RenderCommandBuffer::deferredSortPredicate(const RenderCommand & a, const RenderCommand & b)
{
	return a.material->getShader()->getHandle() < b.material->getShader()->getHandle();
}

bool RenderCommandBuffer::customSortPredicate(const RenderCommand & a, const RenderCommand & b)
{
	return 
		std::make_tuple(a.material->isBlendingEnabled(), a.material->getShader()->getHandle()) <
		std::make_tuple(b.material->isBlendingEnabled(), b.material->getShader()->getHandle());
}
