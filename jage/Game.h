#pragma once

#include "SceneManager.h"

#include "FirstPersonCamera.h"
#include "FrameBuffer.h"
#include "Shader.h"
#include "Model.h"
#include "Grid.h"

class Game : public BaseScene
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
	Shader* m_skyShader;
	Shader* m_abberationShader;

	Model* m_castle;
	Model* m_baracks;
	Model* m_ghost;

	vec3 m_sunDirection;
	float m_turbidity;
	float m_rayleigh;

	std::shared_ptr<MeshObject> m_planeObject;

	std::shared_ptr<Grid> m_grid;
	std::shared_ptr<FirstPersonCamera> m_camera;

	std::shared_ptr<Mesh> m_quad;
	std::shared_ptr<Mesh> m_circle;
	std::shared_ptr<Mesh> m_plane;
	std::unique_ptr<FrameBuffer> m_framebuffer;
	std::unique_ptr<FrameBuffer> m_postEffectFramebuffer;
};