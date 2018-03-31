#pragma once

#include <memory>

#include "Material.h"

class MaterialManager
{
public:
	static void init();

	static Material createMeshMaterial(Texture* albedo, Texture* normal);
	static Material* getFxaaMaterial();

	static Shader* getShadowShader();
	static Shader* getLightShader();

private:
	// materials
	static std::unique_ptr<Material> m_defaultMaterial;
	static std::unique_ptr<Material> m_fxaaMaterial;

	// light shaders
	static Shader* m_shadowShader;
	static Shader* m_lightShader;
};