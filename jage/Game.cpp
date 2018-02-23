#include "Game.h"
#include "Core.h"

void Game::onInit()
{
	glGenBuffers(1, &m_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	ShaderFactory::FromString source(R"(
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)");

	ResourceManager::bind<ShaderFactory>("test_shader", source);
	m_shader = ResourceManager::get<Shader>("test_shader");

	onResize(toGLM((Core::getWindow().getSize())));
}

void Game::onClose()
{
	glDeleteBuffers(1, &m_VBO);
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
	Core::getWindow().clear(sf::Color(60, 60, 60));

	m_shader->bind();

	m_shader->unbind();
}

void Game::onResize(const vec2& windowSize)
{
	// properly resizing
	Core::getWindow().setView(sf::View(sf::FloatRect(0.0f, 0.0f, windowSize.x, windowSize.y)));
}