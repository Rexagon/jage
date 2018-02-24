#include "Game.h"
#include "Core.h"

void Game::onInit()
{
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	ResourceManager::bind<ModelFactory>("ghost", "ghost.fbx");
	m_ghost = ResourceManager::get<Model>("ghost");

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

	ShaderFactory::FromFile fxaaVertexSource("shaders/quad.vert");
	ShaderFactory::FromFile fxaaFragmentShader("shaders/fxaa.frag");
	ResourceManager::bind<ShaderFactory>("fxaa_shader", fxaaVertexSource, fxaaFragmentShader);
	m_fxaaShader = ResourceManager::get<Shader>("fxaa_shader");
	m_fxaaShader->setAttribute(0, "position");
	m_fxaaShader->setAttribute(1, "texCoord");

	ShaderFactory::FromFile meshVertexSource("shaders/mesh.vert");
	ShaderFactory::FromFile meshFragmentSource("shaders/mesh.frag");
	ResourceManager::bind<ShaderFactory>("mesh_shader", meshVertexSource, meshFragmentSource);
	m_meshShader = ResourceManager::get<Shader>("mesh_shader");
	m_meshShader->setAttribute(0, "position");
	m_meshShader->setAttribute(1, "texCoord");
	m_meshShader->setAttribute(2, "normal");

	m_camera = std::make_unique<PerspectiveCamera>("main_camera", glm::radians(90.0f), 1.0f);
	m_camera->getTransformation().setPosition(0.0f, 1.0f, 2.0f);

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

	vec3 direction(0.0f, 0.0f, 0.0f);
	if (Input::getKey(Key::W)) {
		direction += m_camera->getTransformation().getDirectionFront();
	}
	else if(Input::getKey(Key::S)) {
		direction -= m_camera->getTransformation().getDirectionFront();
	}

	if (Input::getKey(Key::A)) {
		direction -= m_camera->getTransformation().getDirectionRight();
	}
	else if (Input::getKey(Key::D)) {
		direction += m_camera->getTransformation().getDirectionRight();
	}

	if (direction != vec3(0.0f, 0.0f, 0.0f)) {
		m_camera->getTransformation().move(glm::normalize(direction) * dt * 10.0f);
	}

	if (Input::getMouse(MouseButton::Right)) {
		m_camera->getTransformation().rotate(0.0f, Input::getMouseDeltaPosition().x * dt, 0.0f);
	}
	//m_camera->getTransformation().rotate(Input::getMouseDeltaPosition().y * dt, 0.0f, 0.0f);
}

void Game::onDraw(const float dt)
{
	m_framebuffer->bind();
	
	glViewport(0, 0, 
		static_cast<GLsizei>(Core::getWindow().getSize().x), 
		static_cast<GLsizei>(Core::getWindow().getSize().y));

	glClearColor(0.2f, 0.2f, 0.2f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_meshShader->bind();
	m_meshShader->setUniform("u_cameraViewProjection", m_camera->getViewProjection());

	std::stack<GameObject*> gameObjects;
	gameObjects.push(m_ghost->getRootObject().get());
	while (!gameObjects.empty()) {
		GameObject* gameObject = gameObjects.top();
		gameObjects.pop();

		m_meshShader->setUniform("u_transformation", gameObject->getTransformation().getGlobalTransform());
		gameObject->onDraw();

		for (auto it = gameObject->getChildren().rbegin(); it != gameObject->getChildren().rend(); ++it) {
			gameObjects.push(it->get());
		}
	}

	m_meshShader->unbind();

	m_framebuffer->unbind();


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_framebuffer->getColorTexture().getNativeHandle());

	glDisable(GL_DEPTH_TEST);
	m_fxaaShader->bind();
	m_quad->draw();
	m_fxaaShader->unbind();
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


	m_fxaaShader->bind();
	m_fxaaShader->setUniform("u_colorTexture", 0);
	m_fxaaShader->setUniform("u_resolution", vec2(1.0f / windowSize.x , 1.0f / windowSize.y));
	m_fxaaShader->unbind();
}