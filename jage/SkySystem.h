#pragma once

#include <memory>

#include "Mesh.h"
#include "GameObject.h"
#include "SkyMaterial.h"
#include "MeshComponent.h"
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

	std::shared_ptr<GameObject> m_sun;
	ComponentHandle<MeshComponent> m_skyRenderingData;

	float m_azimuth;
	float m_inclination;
	bool m_sunChanged;

	std::unique_ptr<Mesh> m_cube;
};