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


	// Loading models
	ResourceManager::bind<ModelFactory>("castle", "castle.fbx");
	m_castle = ResourceManager::get<Model>("castle");
	m_castle->getRootObject()->setPosition(15.0f, 0.0f, -15.0f);
	m_castle->getRootObject()->setRotation(0.0f, glm::pi<float>() / 4.0f, 0.0f);

	ResourceManager::bind<ModelFactory>("baracks", "baracks.fbx");
	m_baracks = ResourceManager::get<Model>("baracks");

	ResourceManager::bind<ModelFactory>("ghost", "ghost.fbx");
	m_ghost = ResourceManager::get<Model>("ghost");

	m_baracks->getRootObject()->setPosition(-15.0f, 0.0f, 15.0f);

	m_quad = std::make_unique<Mesh>();
	m_quad->init({
		vec3(-1.0f, -1.0f, 0.0f),
		vec3(1.0f, -1.0f, 0.0f),
		vec3(1.0f, 1.0f, 0.0f),
		vec3(-1.0f, 1.0f, 0.0f)
	},
	{
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
		vec2(0.0f, 1.0f)
	},
	{
		0, 1, 2,
		0, 2, 3
	});

	m_grid = std::make_shared<Grid>("grid");

	m_camera = std::make_unique<FirstPersonCamera>("main_camera");
	m_camera->setPosition(0.0f, 1.0f, 10.0f);

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

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

	m_camera->onUpdate(dt);
}

void Game::onDraw(const float dt)
{
	m_framebuffer->bind();
	

	glClearColor(0.2f, 0.2f, 0.2f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_meshShader->bind();
	m_meshShader->setUniform("u_cameraViewProjection", m_camera->getViewProjection());

	std::stack<GameObject*> gameObjects;
	gameObjects.push(m_castle->getRootObject().get());
	gameObjects.push(m_baracks->getRootObject().get());
	gameObjects.push(m_ghost->getRootObject().get());

	while (!gameObjects.empty()) {
		GameObject* gameObject = gameObjects.top();
		gameObjects.pop();

		m_meshShader->setUniform("u_transformation", gameObject->getGlobalTransformation());
		gameObject->onDraw();

		for (auto it = gameObject->getChildren().rbegin(); it != gameObject->getChildren().rend(); ++it) {
			gameObjects.push(it->get());
		}
	}

	m_meshShader->unbind();

	m_gridShader->bind();
	m_gridShader->setUniform("u_cameraViewProjection", m_camera->getViewProjection());
	m_gridShader->setUniform("u_cameraPosition", m_camera->getPosition());
	m_gridShader->setUniform("u_transformation", m_grid->getGlobalTransformation());
	m_grid->onDraw();
	m_gridShader->unbind();

	m_framebuffer->unbind();


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_framebuffer->getColorTexture().getNativeHandle());

	glDisable(GL_DEPTH_TEST);

	m_postEffectFramebuffer->bind();
	m_fxaaShader->bind();
	m_quad->draw();
	m_fxaaShader->unbind();
	m_postEffectFramebuffer->unbind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_postEffectFramebuffer->getColorTexture().getNativeHandle());

	m_abberationShader->bind();
	m_quad->draw();
	m_abberationShader->unbind();

	glEnable(GL_DEPTH_TEST);
}

void Game::onResize(const vec2& windowSize)
{
	glViewport(0, 0, static_cast<GLsizei>(windowSize.x), static_cast<GLsizei>(windowSize.y));

	m_camera->setAspect(windowSize.x / windowSize.y);

	m_framebuffer = std::make_unique<FrameBuffer>(
		static_cast<unsigned int>(windowSize.x),
		static_cast<unsigned int>(windowSize.y),
		true);

	m_postEffectFramebuffer = std::make_unique<FrameBuffer>(
		static_cast<unsigned int>(windowSize.x),
		static_cast<unsigned int>(windowSize.y),
		false);


	m_fxaaShader->bind();
	m_fxaaShader->setUniform("u_colorTexture", 0);
	m_fxaaShader->setUniform("u_inversedResolution", vec2(1.0f / windowSize.x , 1.0f / windowSize.y));
	m_fxaaShader->unbind();

	m_abberationShader->bind();
	m_abberationShader->setUniform("u_colorTexture", 0);
	m_abberationShader->setUniform("u_chromaticAberration", 0.002f);
	m_abberationShader->unbind();
}