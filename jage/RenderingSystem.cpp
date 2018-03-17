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

	m_geometryBuffer = std::make_unique<FrameBuffer>(1, 1, GL_HALF_FLOAT, 4, true);
	m_lightBuffer = std::make_unique<FrameBuffer>(1, 1, GL_UNSIGNED_BYTE, 1, false);

	m_commandBuffer = std::make_unique<RenderCommandBuffer>(this);
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
			m_commandBuffer->push(component.getMesh(), object->getGlobalTransformation(), component.getMaterial());
		}
	});

	m_commandBuffer->sort();

	// reset gl state
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
	
	m_geometryBuffer->bind();
	RenderStateManager::setViewport(m_renderSize);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	for (size_t i = 0; i < deferredRenderCommands.size(); ++i) {
		draw(&deferredRenderCommands[i], m_mainCameraData, false);
	}

	// render all shadow casters
	attachments[1] = GL_NONE;
	attachments[2] = GL_NONE;
	attachments[3] = GL_NONE;
	glDrawBuffers(4, attachments);

	std::vector<RenderCommand> shadowRenderCommands = m_commandBuffer->getShadowCastRenderCommands();
	m_manager->each<LightComponent>([this, &shadowRenderCommands](EntityId id, LightComponent& component) {
		std::shared_ptr<GameObject> object = m_manager->get(id);

		if (object != nullptr && component.isShadowCastingEnabled()) {
			component.getShadowBuffer()->bind();
			RenderStateManager::setViewport(component.getShadowBufferSize());
			glClear(GL_DEPTH_BUFFER_BIT);


		}
	});
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

	m_mainCameraData->setAspect(m_renderSize.x / m_renderSize.y);

	m_geometryBuffer->resize(m_renderSize);
	m_lightBuffer->resize(m_renderSize);

	/*RenderStateManager::setCurrentShader(m_fxaaShader);
	m_fxaaShader->setUniform("u_colorTexture", 0);
	m_fxaaShader->setUniform("u_inversedResolution", vec2(1.0f / event.windowSize.x, 1.0f / event.windowSize.y));

	RenderStateManager::setCurrentShader(m_abberationShader);
	m_abberationShader->setUniform("u_colorTexture", 0);*/
}

void RenderingSystem::draw(RenderCommand * command, ComponentHandle<CameraComponent> cameraData, bool affectRenderState)
{
}

/*
void RenderingSystem::loadShaders()
{
	ShaderFactory::FromFile quadVertexSource("shaders/quad.vert");

	ShaderFactory::FromFile fxaaFragmentSource("shaders/fxaa.frag");
	ResourceManager::bind<ShaderFactory>("fxaa_shader", quadVertexSource, fxaaFragmentSource);
	m_fxaaShader = ResourceManager::get<Shader>("fxaa_shader");
	m_fxaaShader->setAttribute(0, "position");
	m_fxaaShader->setAttribute(1, "texCoord");

	ShaderFactory::FromFile abberationFragmentSource("shaders/abberation.frag");
	ResourceManager::bind<ShaderFactory>("abberation_shader", quadVertexSource, abberationFragmentSource);
	m_abberationShader = ResourceManager::get<Shader>("abberation_shader");
	m_abberationShader->setAttribute(0, "position");
	m_abberationShader->setAttribute(1, "texCoord");

	ShaderFactory::FromFile meshVertexSource("shaders/mesh.vert");
	ShaderFactory::FromFile meshFragmentSource("shaders/mesh.frag");
	ResourceManager::bind<ShaderFactory>("mesh_shader", meshVertexSource, meshFragmentSource);
	m_meshShader = ResourceManager::get<Shader>("mesh_shader");
	m_meshShader->setAttribute(0, "position");
	m_meshShader->setAttribute(1, "texCoord");
	m_meshShader->setAttribute(2, "normal");

	ShaderFactory::FromFile gridVertexSource("shaders/grid.vert");
	ShaderFactory::FromFile gridGeometrySource("shaders/grid.geom");
	ShaderFactory::FromFile gridFragmentSource("shaders/grid.frag");
	ResourceManager::bind<ShaderFactory>("grid_shader", gridVertexSource, gridGeometrySource, gridFragmentSource);
	m_gridShader = ResourceManager::get<Shader>("grid_shader");
	m_gridShader->setAttribute(0, "position");
	m_gridShader->setAttribute(1, "chunkId");

	ShaderFactory::FromFile skyVertexSource("shaders/sky.vert");
	ShaderFactory::FromFile skyFragmentSource("shaders/sky.frag");
	ResourceManager::bind<ShaderFactory>("sky_shader", skyVertexSource, skyFragmentSource);
	m_skyShader = ResourceManager::get<Shader>("sky_shader");
	m_skyShader->setAttribute(0, "position");
}
*/