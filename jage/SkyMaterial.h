#pragma once

#include "Material.h"

class SkyMaterial : public Material
{
public:
	SkyMaterial();
	~SkyMaterial();

	void bind() override;


	void setRayleigh(float rayleigh);
	float getRayleigh() const;

	void setTurbidity(float turbidity);
	float getTurbidity() const;

	void setMieCoefficient(float mieCoefficient);
	float getMieCoefficient() const;

	void setMieDirectionalG(float mieDirectionalG);
	float getMieDirectionalG() const;

	void setLuminance(float luminance);
	float getLuminance() const;


	void setSunDirection(const vec3& direction);
	vec3 getSunDirection() const;

private:
	float m_rayleigh;
	float m_turbidity;
	float m_mieCoefficient;
	float m_mieDirectionalG;
	float m_luminance;

	vec3 m_sunDirection;
};