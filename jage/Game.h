#pragma once

#include "SceneManager.h"

#include "FrameBuffer.h"
#include "Shader.h"
#include "Model.h"
#include "FirstPersonCamera.h"

class Game : public Scene
{
public:
	void onInit() override;

	void onClose() override;

	void onUpdate(const float dt) override;

	void onDraw(const float dt) override;

	void onResize(const vec2& windowSize) override;

private:
	Shader* m_fxaaShader;
	Shader* m_meshShader;
	Shader* m_gridShader;

	Model* m_castle;
	Model* m_baracks;

	std::shared_ptr<FirstPersonCamera> m_camera;

	GLuint m_gridVAO;
	GLuint m_gridVBO;

	float m_rotationX;
	float m_rotationY;

	std::unique_ptr<Mesh> m_quad;
	std::unique_ptr<FrameBuffer> m_framebuffer;
};