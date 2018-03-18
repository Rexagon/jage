#pragma once

#include <memory>

#include "Material.h"

class MaterialManager
{
public:
	static void init();

	static Shader* getMeshShader();
	static Material createMeshMaterial(Texture* albedo, Texture* normal);

	static Shader* getShadowShader();

	static Shader* getLightShader();

private:
	// mesh materials
	static Shader* m_meshShader;
	static std::unique_ptr<Material> m_defaultMaterial;

	// light shaders
	static Shader* m_shadowShader;
	static Shader* m_lightShader;
};