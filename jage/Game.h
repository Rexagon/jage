#pragma once

#include "SceneManager.h"

#include "FirstPersonCamera.h"
#include "FrameBuffer.h"
#include "Shader.h"
#include "Model.h"
#include "Grid.h"

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
	Shader* m_abberationShader;

	Model* m_castle;
	Model* m_baracks;
	Model* m_ghost;

	std::shared_ptr<Grid> m_grid;
	std::shared_ptr<FirstPersonCamera> m_camera;

	std::unique_ptr<Mesh> m_quad;
	std::unique_ptr<FrameBuffer> m_framebuffer;
	std::unique_ptr<FrameBuffer> m_postEffectFramebuffer;
};