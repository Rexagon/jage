#pragma once

#include "Material.h"

class AbberationMaterial : public Material
{
public:
	AbberationMaterial();
	~AbberationMaterial();

	void bind() override;

	void setChromaticAberration(float chromaticAberration);
	float getChromaticAberration() const;

private:
	float m_chromaticAberration;
};