#include "Game.h"
#include "Core.h"

void Game::onInit()
{
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);

	m_mesh = std::make_unique<Mesh>();
	m_mesh->init({
		vec3(-0.5f, -0.5f, 0.0f),
		vec3(0.5f, -0.5f, 0.0f),
		vec3(0.0f, 0.5f, 0.0f)
	},
	{
		vec2(0.0f, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(0.0f, 0.0f)
	},
	{
		0, 1, 2
	});

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

	ShaderFactory::FromFile vertexSource("shaders/quad.vert");

	ShaderFactory::FromFile fxaaFragmentShader("shaders/fxaa.frag");
	ShaderFactory::FromFile solidFragmentShader("shaders/solid.frag");

	ResourceManager::bind<ShaderFactory>("fxaa_shader", vertexSource, fxaaFragmentShader);
	m_fxaaShader = ResourceManager::get<Shader>("fxaa_shader");
	m_fxaaShader->setAttribute(0, "position");
	m_fxaaShader->setAttribute(1, "texCoord");

	ResourceManager::bind<ShaderFactory>("solid_shader", vertexSource, solidFragmentShader);
	m_solidShader = ResourceManager::get<Shader>("solid_shader");
	m_solidShader->setAttribute(0, "position");
	m_solidShader->setAttribute(1, "texCoord");

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
}

void Game::onDraw(const float dt)
{
	m_framebuffer->bind();
	glViewport(0.0f, 0.0f, Core::getWindow().getSize().x, Core::getWindow().getSize().y);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_solidShader->bind();
	m_solidShader->setUniform("u_color", vec4(1.0f, 0.5f, 0.2f, 1.0f));
	m_mesh->draw();
	m_solidShader->unbind();

	m_framebuffer->unbind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_framebuffer->getColorTexture().getNativeHandle());

	m_fxaaShader->bind();
	m_quad->draw();
	m_fxaaShader->unbind();
}

void Game::onResize(const vec2& windowSize)
{
	glViewport(0.0f, 0.0f, windowSize.x, windowSize.y);
	m_framebuffer = std::make_unique<FrameBuffer>(
		static_cast<unsigned int>(windowSize.x),
		static_cast<unsigned int>(windowSize.y),
		true);


	m_fxaaShader->bind();
	m_fxaaShader->setUniform("u_resolution", windowSize);
	m_fxaaShader->setUniform("u_colorTexture", 0);
	m_fxaaShader->unbind();
}