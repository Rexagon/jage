#include "RenderingSystem.h"

#include "RenderStateManager.h"
#include "ResourceManager.h"
#include "ShaderFactory.h"
#include "MeshComponent.h"

void RenderingSystem::init()
{
	m_manager->subscribe<Events::OnWindowResized>(this);

	loadShaders();

	m_quad = std::make_shared<Mesh>();
	m_quad->init(MeshGeometry::createQuad());

	m_sky = std::make_shared<Mesh>();
	m_sky->init(MeshGeometry::createCube());

	m_sunDirection = vec3(-1.0f, 0.0f, 0.0f);
	m_turbidity = 10.0f;
	m_rayleigh = 2.0f;

	RenderStateManager::setFaceCullingEnabled(true);
	RenderStateManager::setFaceCullingSide(GL_BACK);
}

void RenderingSystem::close()
{
	m_manager->unsubscribe<Events::OnWindowResized>(this);
}

void RenderingSystem::update(const float dt)
{	
	if (!m_mainCameraData.isValid()) {
		return;
	}

	updateCamera();

	m_framebuffer->bind();

	RenderStateManager::setClearColor(0.2f, 0.2f, 0.2f);

	glClearDepth(0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	drawSky();
	drawScene();

	m_framebuffer->unbind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_framebuffer->getColorTexture().getNativeHandle());

	RenderStateManager::setDepthTestEnabled(false);

	m_postEffectFramebuffer->bind();
	RenderStateManager::setCurrentShader(m_fxaaShader);
	m_quad->draw();
	m_postEffectFramebuffer->unbind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_postEffectFramebuffer->getColorTexture().getNativeHandle());

	RenderStateManager::setCurrentShader(m_abberationShader);
	m_abberationShader->setUniform("u_chromaticAberration", 0.001f);
	m_quad->draw();
}

void RenderingSystem::setMainCamera(std::shared_ptr<GameObject> camera)
{
	if (camera != nullptr) {
		m_mainCameraData = camera->getComponent<CameraComponent>();
	}
}

void RenderingSystem::onReceive(EntityManager * manager, const Events::OnWindowResized & event)
{
	RenderStateManager::setViewport(ivec2(event.windowSize));

	m_mainCameraData->setAspect(event.windowSize.x / event.windowSize.y);

	m_framebuffer = std::make_unique<FrameBuffer>(
		static_cast<unsigned int>(event.windowSize.x),
		static_cast<unsigned int>(event.windowSize.y),
		true);

	m_postEffectFramebuffer = std::make_unique<FrameBuffer>(
		static_cast<unsigned int>(event.windowSize.x),
		static_cast<unsigned int>(event.windowSize.y),
		false);


	RenderStateManager::setCurrentShader(m_fxaaShader);
	m_fxaaShader->setUniform("u_colorTexture", 0);
	m_fxaaShader->setUniform("u_inversedResolution", vec2(1.0f / event.windowSize.x, 1.0f / event.windowSize.y));

	RenderStateManager::setCurrentShader(m_abberationShader);
	m_abberationShader->setUniform("u_colorTexture", 0);
}

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

void RenderingSystem::updateCamera()
{
	m_manager->each<CameraComponent>([this](EntityId id, CameraComponent& component) {
		std::shared_ptr<GameObject> camera = m_manager->get(id);

		if (camera != nullptr) {
			component.updateView(camera->getGlobalTransformation());
			component.updateProjection();
		}
	});
}

void RenderingSystem::drawSky()
{
	RenderStateManager::setCurrentShader(m_skyShader);
	glDepthMask(GL_FALSE);
	RenderStateManager::setFaceCullingEnabled(false);
	m_skyShader->setUniform("u_cameraViewProjection", m_mainCameraData->getProjectionMatrix() * 
		glm::inverse(m_mainCameraData.getEntity()->getRotationMatrix()));

	m_skyShader->setUniform("u_sunDirection", glm::normalize(quat(m_sunDirection) * vec3(0.0f, 0.0f, 1.0f)));
	m_skyShader->setUniform("u_rayleigh", m_rayleigh);
	m_skyShader->setUniform("u_turbidity", m_turbidity);
	m_skyShader->setUniform("u_mieCoefficient", 0.005f);
	m_skyShader->setUniform("u_mieDirectionalG", 0.8f);
	m_skyShader->setUniform("u_luminance", 1.0f);
	m_sky->draw();
	glDepthMask(GL_TRUE);
	RenderStateManager::setFaceCullingEnabled(true);
}

void RenderingSystem::drawScene()
{
	RenderStateManager::setDepthTestEnabled(true);
	RenderStateManager::setDepthTestFunction(GL_GEQUAL);

	RenderStateManager::setCurrentShader(m_meshShader);
	m_meshShader->setUniform("u_cameraViewProjection", m_mainCameraData->getViewProjectionMatrix());
	m_meshShader->setUniform("u_sunDirection", glm::normalize(quat(m_sunDirection) * vec3(0.0f, 0.0f, 1.0f)));

	m_manager->each<MeshComponent>([this](EntityId id, MeshComponent& component) {
		std::shared_ptr<GameObject> object = m_manager->get(id);

		m_meshShader->setUniform("u_transformation", object->getGlobalTransformation());
		if (component.getMesh() != nullptr) {
			component.getMesh()->draw();
		}
	});
}
