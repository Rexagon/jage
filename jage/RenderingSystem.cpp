#include "RenderingSystem.h"

#include "RenderStateManager.h"
#include "ResourceManager.h"
#include "ShaderFactory.h"
#include "MeshComponent.h"
#include "Core.h"

void RenderingSystem::init()
{
	m_manager->subscribe<Events::OnWindowResized>(this);

	m_quad = std::make_shared<Mesh>();
	m_quad->init(MeshGeometry::createQuad(vec2(1.0f), MeshGeometry::TEXTURED_VERTEX));

	m_geometryBuffer = std::make_unique<FrameBuffer>(1024, 768, GL_UNSIGNED_BYTE, 3, true);
	m_lightBuffer = std::make_unique<FrameBuffer>(1024, 768, GL_UNSIGNED_BYTE, 1, false);
	m_mainBuffer = std::make_unique<FrameBuffer>(1024, 768, GL_UNSIGNED_BYTE, 1, true);

	for (size_t i = 0; i < m_postProcessBuffers.size(); ++i) {
		m_postProcessBuffers[i] = std::make_unique<FrameBuffer>(1024, 768, GL_UNSIGNED_BYTE, 1, false);
	}

	m_commandBuffer = std::make_unique<RenderCommandBuffer>(this);
}

void RenderingSystem::close()
{
	m_manager->unsubscribe<Events::OnWindowResized>(this);

	m_quad.reset();
	m_geometryBuffer.reset();
	m_lightBuffer.reset();
	m_mainBuffer.reset();
	for (size_t i = 0; i < m_postProcessBuffers.size(); ++i) {
		m_postProcessBuffers[i].reset();
	}

	m_postProcessCommands.clear();
}

void RenderingSystem::update(const float dt)
{	
	if (!m_mainCameraData.isValid() || m_geometryBuffer == nullptr) {
		return;
	}

	m_manager->each<CameraComponent>([this](EntityId id, CameraComponent& component) {
		std::shared_ptr<GameObject> object = m_manager->get(id);

		if (object != nullptr) {
			component.updateView(object->getGlobalTransformation());
			component.updateProjection();
		}
	});

	m_manager->each<MeshComponent>([this](EntityId id, MeshComponent& component) {
		std::shared_ptr<GameObject> object = m_manager->get(id);

		if (object != nullptr) {
			m_commandBuffer->push(component.getMesh(), object->getGlobalTransformation(), &component.getMaterial());
		}
	});

	m_commandBuffer->sort();

	// reset gl state
	m_geometryBuffer->bind();

	RenderStateManager::setBlendingEnabled(false);

	RenderStateManager::setFaceCullingEnabled(true);
	RenderStateManager::setFaceCullingSide(GL_BACK);

	RenderStateManager::setClearDepth(0.0f);
	RenderStateManager::setClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

	RenderStateManager::setDepthTestEnabled(true);
	RenderStateManager::setDepthTestFunction(GL_GEQUAL);

	RenderStateManager::setClearColor(0.0f, 0.0f, 0.0f);

	// render to geometry buffer
	unsigned int attachments[3] = { 
		GL_COLOR_ATTACHMENT0, 
		GL_COLOR_ATTACHMENT1, 
		GL_COLOR_ATTACHMENT2
	};
	glDrawBuffers(3, attachments);

	std::vector<RenderCommand> deferredRenderCommands = m_commandBuffer->getDeferredRenderCommands(true);
	
	RenderStateManager::setViewport(m_renderSize);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	for (size_t i = 0; i < deferredRenderCommands.size(); ++i) {
		renderCustomCommand(&deferredRenderCommands[i], m_mainCameraData.get(), false);
	}

	// render all shadow casters
	attachments[1] = GL_NONE;
	attachments[2] = GL_NONE;
	glDrawBuffers(3, attachments);

	RenderStateManager::setFaceCullingSide(GL_FRONT);
	std::vector<RenderCommand> shadowRenderCommands = m_commandBuffer->getShadowCastRenderCommands();
	m_manager->each<LightComponent>([this, &shadowRenderCommands](EntityId id, LightComponent& component) {
		std::shared_ptr<GameObject> object = m_manager->get(id);

		if (object != nullptr && component.isShadowCastingEnabled()) {
			component.updateView(object->getGlobalTransformation());
			component.updateProjection();

			component.getShadowBuffer()->bind();
			RenderStateManager::setViewport(component.getShadowBufferSize());
			glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			
			for (size_t i = 0; i < shadowRenderCommands.size(); ++i) {
				renderShadowCastCommand(&shadowRenderCommands[i], &component);
			}
		}
	});
	RenderStateManager::setFaceCullingSide(GL_BACK);

	// do post processing before lighting pass
	//TODO: make preprocessing

	// render lights
	m_lightBuffer->bind();
	RenderStateManager::setViewport(m_renderSize);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	RenderStateManager::setDepthTestEnabled(false);
	RenderStateManager::setBlendingEnabled(true);
	RenderStateManager::setBlendingFunction(GL_ONE, GL_ONE);

	for (unsigned int i = 0; i < 3; ++i) {
		m_geometryBuffer->getColorTexture(i)->bind(i);
	}
	m_geometryBuffer->getDepthStencilTexture()->bind(3);

	RenderStateManager::setCurrentShader(MaterialManager::getLightShader());
	MaterialManager::getLightShader()->setUniform("u_inversedViewProjection", glm::inverse(m_mainCameraData->getViewProjectionMatrix()));

	m_manager->each<LightComponent>([this](EntityId id, LightComponent& component) {
		std::shared_ptr<GameObject> object = m_manager->get(id);

		Shader* shader = MaterialManager::getLightShader();
		shader->setUniform("u_isShadowsEnabled", static_cast<int>(component.isShadowCastingEnabled()));
		shader->setUniform("u_direction", object->getDirectionFront());
		shader->setUniform("u_color", component.getColor());
		shader->setUniform("u_lightViewProjection", component.getViewProjectionMatrix());

		if (component.isShadowCastingEnabled()) {
			component.getShadowBuffer()->getDepthStencilTexture()->bind(4);
		}

		m_quad->draw();
	});

	// render in forward mode
	RenderStateManager::setDepthTestEnabled(true);
	RenderStateManager::setBlendingEnabled(false);
	RenderStateManager::setBlendingFunction(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_geometryBuffer->getHandle());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_mainBuffer->getHandle());
	glBlitFramebuffer(
		0, 0, m_geometryBuffer->getSize().x, m_geometryBuffer->getSize().y,
		0, 0, m_renderSize.x, m_renderSize.y,
		GL_DEPTH_BUFFER_BIT, GL_NEAREST
	);

	std::vector<RenderCommand> customRenderCommands = m_commandBuffer->getCustomRenderCommands(nullptr, true);

	m_mainBuffer->bind();
	RenderStateManager::setViewport(m_renderSize);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	for (size_t i = 0; i < customRenderCommands.size(); ++i) {
		renderCustomCommand(&customRenderCommands[i], m_mainCameraData.get(), true);
	}

	// render meshes with alpha materials
	std::vector<RenderCommand> alphaRenderCommands = m_commandBuffer->getAlphaRenderCommands(true);

	for (size_t i = 0; i < alphaRenderCommands.size(); ++i) {
		renderCustomCommand(&alphaRenderCommands[i], m_mainCameraData.get(), true);
	}

	// post processing
	m_lightBuffer->bind();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_postProcessBuffers[1]->getHandle());
	glBlitFramebuffer(
		0, 0, m_renderSize.x, m_renderSize.y,
		0, 0, m_renderSize.x, m_renderSize.y,
		GL_COLOR_BUFFER_BIT, GL_LINEAR);

	bool oddCommand = false;
	auto postProcessCommands = m_postProcessCommands;
	for (auto& command : postProcessCommands) {
		auto targetBuffer = m_postProcessBuffers[static_cast<size_t>(oddCommand)].get();
		auto screenBuffer = m_postProcessBuffers[static_cast<size_t>(!oddCommand)].get();

		screenBuffer->getColorTexture(0)->bind(0);

		targetBuffer->bind();
		RenderStateManager::setViewport(m_renderSize);
		glClear(GL_COLOR_BUFFER_BIT);
		renderPostProcessingCommand(&command);

		oddCommand = !oddCommand;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(
		0, 0, m_renderSize.x, m_renderSize.y,
		0, 0, m_renderSize.x, m_renderSize.y,
		GL_COLOR_BUFFER_BIT, GL_LINEAR);

	// finals
	m_commandBuffer->clear();
}

void RenderingSystem::setMainCamera(std::shared_ptr<GameObject> camera)
{
	if (camera != nullptr) {
		m_mainCameraData = camera->getComponent<CameraComponent>();
	}
}

void RenderingSystem::onReceive(EntityManager * manager, const Events::OnWindowResized & event)
{
	m_renderSize = event.windowSize;

	m_mainCameraData->setAspect(static_cast<float>(m_renderSize.x) / static_cast<float>(m_renderSize.y));

	m_geometryBuffer->resize(m_renderSize);
	m_lightBuffer->resize(m_renderSize);
	m_mainBuffer->resize(m_renderSize);

	for (auto& postProcessBuffer : m_postProcessBuffers) {
		postProcessBuffer->resize(m_renderSize);
	}
}

void RenderingSystem::addPostProcess(size_t order, Material * material)
{
	m_postProcessCommands.emplace(order, material);
}

void RenderingSystem::renderCustomCommand(const RenderCommand * command, CameraComponent* cameraData, bool affectRenderState)
{
	const Mesh* mesh;
	const Material* material;
	Shader* shader;

	if ((mesh = command->mesh) == nullptr ||
		(material = command->material) == nullptr ||
		(shader = material->getShader()) == nullptr)
	{
		return;
	}

	if (affectRenderState) {
		RenderStateManager::setBlendingEnabled(material->isBlendingEnabled());
		RenderStateManager::setBlendingFunction(
			material->getBlendingFunctionSrc(),
			material->getBlendingFunctionDst()
		);
		RenderStateManager::setDepthTestEnabled(material->isDepthTestEnabled());
		RenderStateManager::setDepthTestFunction(material->getDepthTestFunction());
		RenderStateManager::setFaceCullingEnabled(material->isFaceCullingEnabled());
		RenderStateManager::setFaceCullingSide(material->getFaceCullingSide());
	}

	RenderStateManager::setCurrentShader(shader);
	shader->setUniform("u_transformation", command->transform);
	shader->setUniform("u_cameraViewProjection", cameraData->getViewProjectionMatrix());
	shader->setUniform("u_cameraProjection", cameraData->getProjectionMatrix());

	const auto& textures = material->getTextures();
	for (size_t i = 0; i < textures.size(); ++i) {
		textures[i]->bind(static_cast<unsigned int>(i));
	}

	mesh->draw();
}

void RenderingSystem::renderShadowCastCommand(const RenderCommand * command, LightComponent* lightData)
{
	Shader* shader;

	if ((shader = MaterialManager::getShadowShader()) == nullptr) {
		return;
	}

	RenderStateManager::setCurrentShader(shader);
	shader->setUniform("u_transformation", command->transform);
	shader->setUniform("u_cameraViewProjection", lightData->getViewProjectionMatrix());

	command->mesh->draw();
}

void RenderingSystem::renderPostProcessingCommand(const PostProcessCommand * command)
{
	Material* material;
	Shader* shader;

	if ((material = command->material) == nullptr ||
		(shader = material->getShader()) == nullptr)
	{
		return;
	}

	RenderStateManager::setDepthTestEnabled(false);

	RenderStateManager::setCurrentShader(shader);
	shader->setUniform("u_screenSize", m_renderSize);
	shader->setUniform("u_screenSizeInverted", vec2(1.0f / m_renderSize.x, 1.0f / m_renderSize.y));
	shader->setUniform("u_screenTexture", 0);

	m_quad->draw();
}
