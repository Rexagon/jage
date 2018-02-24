#pragma once

#include "SceneManager.h"

#include "Transformation.h"
#include "FrameBuffer.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"

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

	Model* m_ghost;

	std::shared_ptr<PerspectiveCamera> m_camera;

	std::unique_ptr<Mesh> m_quad;
	std::unique_ptr<FrameBuffer> m_framebuffer;
};