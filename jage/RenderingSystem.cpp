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
	m_quad->init(MeshGeometry::createQuad());

	m_geometryBuffer = std::make_unique<FrameBuffer>(1024, 768, GL_UNSIGNED_BYTE, 4, true);
	m_lightBuffer = std::make_unique<FrameBuffer>(1024, 768, GL_UNSIGNED_BYTE, 1, false);
	m_mainBuffer = std::make_unique<FrameBuffer>(1024, 768, GL_UNSIGNED_BYTE, 1, true);

	m_commandBuffer = std::make_unique<RenderCommandBuffer>(this);

	ShaderFactory::FromString vertexShaderSource(
		"#version 330\n"
		"layout(location = 0) in vec3 position;\n"
		"layout(location = 1) in vec2 texCoord;\n"
		"out vec2 v_texCoord;\n"
		"void main() {\n"
		"	gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
		"	v_texCoord = texCoord;\n"
		"}"
	);
	ShaderFactory::FromString fragmentShaderSource(
		"#version 330\n"
		"uniform sampler2D diffuseTexture;\n"
		"in vec2 v_texCoord;\n"
		"void main() { gl_FragColor = texture(diffuseTexture, v_texCoord); }"
	);
	ResourceManager::bind<ShaderFactory>("main_shader", vertexShaderSource, fragmentShaderSource);
	m_mainShader = ResourceManager::get<Shader>("main_shader");
	m_mainShader->setAttribute(0, "position");
	m_mainShader->setAttribute(1, "texCoord");
	m_mainShader->setUniform("diffuseTexture", 0);
}

void RenderingSystem::close()
{
	m_manager->unsubscribe<Events::OnWindowResized>(this);
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
	unsigned int attachments[4] = { 
		GL_COLOR_ATTACHMENT0, 
		GL_COLOR_ATTACHMENT1, 
		GL_COLOR_ATTACHMENT2, 
		GL_COLOR_ATTACHMENT3 
	};
	glDrawBuffers(4, attachments);

	std::vector<RenderCommand> deferredRenderCommands = m_commandBuffer->getDeferredRenderCommands(true);
	
	RenderStateManager::setViewport(m_renderSize);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (size_t i = 0; i < deferredRenderCommands.size(); ++i) {
		renderCustomCommand(&deferredRenderCommands[i], m_mainCameraData.get(), false);
	}

	// render all shadow casters
	attachments[1] = GL_NONE;
	attachments[2] = GL_NONE;
	attachments[3] = GL_NONE;
	glDrawBuffers(4, attachments);

	RenderStateManager::setFaceCullingSide(GL_FRONT);
	std::vector<RenderCommand> shadowRenderCommands = m_commandBuffer->getShadowCastRenderCommands();
	m_manager->each<LightComponent>([this, &shadowRenderCommands](EntityId id, LightComponent& component) {
		std::shared_ptr<GameObject> object = m_manager->get(id);

		if (object != nullptr && component.isShadowCastingEnabled()) {
			component.updateView(object->getGlobalTransformation());
			component.updateProjection();

			component.getShadowBuffer()->bind();
			RenderStateManager::setViewport(component.getShadowBufferSize());
			glClear(GL_DEPTH_BUFFER_BIT);
			
			for (size_t i = 0; i < shadowRenderCommands.size(); ++i) {
				renderShadowCastCommand(&shadowRenderCommands[i], &component);
			}
		}
	});
	RenderStateManager::setFaceCullingSide(GL_BACK);

	// do post processing before lighting pass
	//TODO: make preprocessing

	// render lights
	RenderStateManager::setDepthTestEnabled(false);
	RenderStateManager::setBlendingEnabled(true);
	RenderStateManager::setBlendingFunction(GL_ONE, GL_ONE);

	for (unsigned int i = 0; i < 4; ++i) {
		m_geometryBuffer->getColorTexture(i)->bind(i);
	}

	m_manager->each<LightComponent>([this](EntityId id, LightComponent& component) {
		switch (component.getType())
		{
		case LightComponent::DIRECTIONAL:
			renderDeferredDirectional(&component);
			break;

		case LightComponent::POINT:
			renderDeferredPoint(&component);
			break;

		case LightComponent::SPOT:
			renderDeferredSpot(&component);

		default:
			break;
		}
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

	// show to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	RenderStateManager::setDepthTestEnabled(false);
	m_geometryBuffer->getColorTexture(0)->bind(0);

	RenderStateManager::setCurrentShader(m_mainShader);
	m_quad->draw();

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
}

void RenderingSystem::renderCustomCommand(RenderCommand * command, CameraComponent* cameraData, bool affectRenderState)
{
	Mesh* mesh = command->mesh;
	Material* material = command->material;

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

	RenderStateManager::setCurrentShader(material->getShader());
	material->getShader()->setUniform("u_transformation", command->transform);
	material->getShader()->setUniform("u_cameraViewProjection", cameraData->getViewProjectionMatrix());
	material->getShader()->setUniform("u_sunDirection", vec3(1.0f, 1.0f, 1.0f));
	mesh->draw();
}

void RenderingSystem::renderShadowCastCommand(RenderCommand * command, LightComponent* lightData)
{
}

void RenderingSystem::renderDeferredAmbient()
{
}

void RenderingSystem::renderDeferredDirectional(LightComponent * component)
{
}

void RenderingSystem::renderDeferredPoint(LightComponent * command)
{
}

void RenderingSystem::renderDeferredSpot(LightComponent * spot)
{
}