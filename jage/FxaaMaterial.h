#pragma once

#include "Material.h"

class FxaaMaterial : public Material
{
public:
	FxaaMaterial();

	void bind() override;

	//TODO: add fxaa properties

private:

};