#pragma once

#include <memory>

#include "Material.h"

class MaterialManager
{
public:
	static void init();

	static Material createDefaultMaterial(Texture* albedo, Texture* normal);

	static Material* getAbberationMaterial();
	static Material* getFxaaMaterial();
private:
	// mesh materials
	static std::unique_ptr<Material> m_defaultMaterial;

	// post processing materials
	static std::unique_ptr<Material> m_abberationMaterial;
	static std::unique_ptr<Material> m_fxaaMaterial;
};