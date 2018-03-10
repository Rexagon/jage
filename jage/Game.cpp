#include "Game.h"
#include "Core.h"

void Game::onInit()
{
	// Loading shaders
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


	// Loading models
	m_rootObject = m_entityManager.create();
	m_rootObject->setName("root");

	ResourceManager::bind<ModelFactory>("castle", "castle.fbx");
	object_ptr castle = ResourceManager::get<Model>("castle")->createGameObject(&m_entityManager, "castle");
	castle->setScale(0.01f);
	castle->setPosition(15.0f, 0.0f, -15.0f);
	castle->setRotation(0.0f, glm::pi<float>() / 4.0f, 0.0f);
	m_rootObject->addChild(castle);

	ResourceManager::bind<ModelFactory>("baracks", "baracks.fbx");
	object_ptr baracks = ResourceManager::get<Model>("baracks")->createGameObject(&m_entityManager, "baracks");
	baracks->setPosition(-15.0f, 0.0f, 15.0f);
	m_rootObject->addChild(baracks);

	ResourceManager::bind<ModelFactory>("ghost", "ghost.fbx");
	object_ptr ghost = ResourceManager::get<Model>("ghost")->createGameObject(&m_entityManager, "ghost");
	m_rootObject->addChild(ghost);

	ResourceManager::bind<ModelFactory>("terrain", "terrain.fbx");
	object_ptr terrain = ResourceManager::get<Model>("terrain")->createGameObject(&m_entityManager, "terrain");
	terrain->setScale(100.0f, 30.0f, 100.0f);
	m_rootObject->addChild(terrain);


	m_quad = std::make_shared<Mesh>();
	m_quad->init(MeshGeometry::createQuad());

	/*
	m_grid = std::make_shared<Grid>("grid");
	m_grid->setPosition(0.0f, 0.001f, 0.0f);
	*/

	m_sky = std::make_shared<Mesh>();
	m_sky->init(MeshGeometry::createCube());

	m_camera = m_entityManager.create();
	m_camera->setName("main_camera");
	m_cameraData = m_camera->assign<CameraComponent>();
	m_camera->setPosition(0.0f, 1.0f, 10.0f);

	m_sunDirection = vec3(-1.0f, 0.0f, 0.0f);
	m_turbidity = 10.0f;
	m_rayleigh = 2.0f;

	RenderStateManager::setFaceCullingEnabled(true);
	RenderStateManager::setFaceCullingSide(GL_BACK);

	onResize(toGLM((Core::getWindow().getSize())));
}

void Game::onClose()
{
}

void Game::onUpdate(const float dt)
{
	if (Input::getKeyDown(Key::Escape)) {
		SceneManager::deleteScene();
		return;
	}

	if (Input::getKey(Key::Up)) {
		m_sunDirection.x -= dt;
	}
	if (Input::getKey(Key::Down)) {
		m_sunDirection.x += dt;
	}

	if (Input::getKey(Key::Left)) {
		m_sunDirection.y += dt;
	}
	if (Input::getKey(Key::Right)) {
		m_sunDirection.y -= dt;
	}

	if (Input::getKey(Key::O)) {
		m_turbidity += dt;
	}
	if (Input::getKey(Key::L)) {
		m_turbidity -= dt;
	}

	if (Input::getKey(Key::I)) {
		m_rayleigh += dt;
	}
	if (Input::getKey(Key::K)) {
		m_rayleigh -= dt;
	}

	m_cameraController.update(dt, m_camera);

	m_cameraData->updateView(m_camera->getGlobalTransformation());
	m_cameraData->updateProjection();

	//m_camera->onUpdate(dt);
}

void Game::onDraw(const float dt)
{
	m_framebuffer->bind();
	
	RenderStateManager::setClearColor(0.2f, 0.2f, 0.2f);

	glClearDepth(0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	RenderStateManager::setCurrentShader(m_skyShader);
	glDepthMask(GL_FALSE);
	RenderStateManager::setFaceCullingEnabled(false);
	m_skyShader->setUniform("u_cameraViewProjection", m_cameraData->getProjectionMatrix() * glm::inverse(m_camera->getRotationMatrix()));

	m_skyShader->setUniform("u_sunDirection", glm::normalize(quat(m_sunDirection) * vec3(0.0f, 0.0f, 1.0f)));
	m_skyShader->setUniform("u_rayleigh", m_rayleigh);
	m_skyShader->setUniform("u_turbidity", m_turbidity);
	m_skyShader->setUniform("u_mieCoefficient", 0.005f);
	m_skyShader->setUniform("u_mieDirectionalG", 0.8f);
	m_skyShader->setUniform("u_luminance", 1.0f);
	m_sky->draw();
	glDepthMask(GL_TRUE);
	RenderStateManager::setFaceCullingEnabled(true);

	// Scene rendering
	RenderStateManager::setDepthTestEnabled(true);
	RenderStateManager::setDepthTestFunction(GL_GEQUAL);

	RenderStateManager::setCurrentShader(m_meshShader);
	m_meshShader->setUniform("u_cameraViewProjection", m_cameraData->getViewProjectionMatrix());
	m_meshShader->setUniform("u_sunDirection", glm::normalize(quat(m_sunDirection) * vec3(0.0f, 0.0f, 1.0f)));

	std::stack<GameObject*> gameObjects;
	gameObjects.push(m_rootObject.get());

	while (!gameObjects.empty()) {
		GameObject* gameObject = gameObjects.top();
		gameObjects.pop();

		if (gameObject->hasComponent<MeshComponent>()) {
			m_meshShader->setUniform("u_transformation", gameObject->getGlobalTransformation());
			auto handle = gameObject->getComponent<MeshComponent>();
			if (handle && handle->getMesh() != nullptr) {
				handle->getMesh()->draw();
			}
		}

		for (auto it = gameObject->getChildren().rbegin(); it != gameObject->getChildren().rend(); ++it) {
			gameObjects.push(it->get());
		}
	}

	/*
	RenderStateManager::setCurrentShader(m_gridShader);
	m_gridShader->setUniform("u_cameraViewProjection", m_cameraData->getViewProjectionMatrix());
	m_gridShader->setUniform("u_cameraPosition", m_camera->getPosition());
	m_gridShader->setUniform("u_transformation", m_grid->getGlobalTransformation());
	m_grid->onDraw();
	*/

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

void Game::onResize(const vec2& windowSize)
{
	RenderStateManager::setViewport(ivec2(windowSize));

	m_cameraData->setAspect(windowSize.x / windowSize.y);

	m_framebuffer = std::make_unique<FrameBuffer>(
		static_cast<unsigned int>(windowSize.x),
		static_cast<unsigned int>(windowSize.y),
		true);

	m_postEffectFramebuffer = std::make_unique<FrameBuffer>(
		static_cast<unsigned int>(windowSize.x),
		static_cast<unsigned int>(windowSize.y),
		false);


	RenderStateManager::setCurrentShader(m_fxaaShader);
	m_fxaaShader->setUniform("u_colorTexture", 0);
	m_fxaaShader->setUniform("u_inversedResolution", vec2(1.0f / windowSize.x , 1.0f / windowSize.y));

	RenderStateManager::setCurrentShader(m_abberationShader);
	m_abberationShader->setUniform("u_colorTexture", 0);
}