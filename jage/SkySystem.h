#pragma once

#include "Mesh.h"
#include "Material.h"
#include "GameObject.h"
#include "SunComponent.h"
#include "EntityManager.h"

namespace Events
{
	struct OnDayBegin {};
	struct OnNightBegin {};
}

class SkySystem : public EntitySystem
{
public:
	SkySystem();

	void update(const float dt) override;

	void setSun(std::shared_ptr<GameObject> sun);
	std::shared_ptr<GameObject> createSun();

	void setTime(unsigned int hours, unsigned int minutes = 0);

	void setAzimuth(float azimuth);
	float getAzimuth() const;

private:
	void updateSun();

	float m_delay;
	unsigned int m_currentTime;
	ComponentHandle<SunComponent> m_sunComponent;

	float m_azimuth;
	float m_inclination;
	vec3 m_direction;
	bool m_direcitonChanged;

	std::unique_ptr<Mesh> m_cube;
	std::unique_ptr<Material> m_skyMaterial;
};