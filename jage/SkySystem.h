#pragma once

#include "EntityManager.h"
#include "SunComponent.h"
#include "GameObject.h"

namespace Events
{
	struct OnDayBegin {};
	struct OnNightBegin {};
}

class SkySystem : public EntitySystem
{
public:
	void update(const float dt) override;

	void setSun(std::shared_ptr<GameObject> sun);

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
};