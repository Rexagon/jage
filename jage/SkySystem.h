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
	void init() override;
	void close() override;

	void update(const float dt) override;

	void setTime(unsigned int hours, unsigned int minutes = 0);
	unsigned int getTime() const;

	void setAzimuth(float azimuth);


private:
	unsigned int m_currentTime;
	ComponentHandle<SunComponent> m_sunComponent;
};