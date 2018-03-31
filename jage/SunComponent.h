#pragma once

#include "Math.h"

class SunComponent
{
public:
	SunComponent();

	void setTurbidity(float turbidity);
	float getTurbidity() const;

	void setRayleigh(float rayleigh);
	float getRayleigh() const;

	void setMieCoefficient(float mieCoefficient);
	float getMieCoefficient() const;

	void setMieDirectionalG(float mieDirectionalG);
	float getMieDirectionalG() const;

private:
	float m_turbidity;
	float m_rayleigh;
	float m_mieCoefficient;
	float m_mieDirectionalG;
};