#include "SkySystem.h"

#include "ShaderFactory.h"
#include "MeshComponent.h"
#include "LightComponent.h"
#include "ResourceManager.h"

SkySystem::SkySystem() :
	m_sun(nullptr), m_cube(nullptr)
{
	m_cube = std::make_unique<Mesh>();
	m_cube->init(MeshGeometry::createCube());
}

void SkySystem::update(const float dt)
{
	updateSun();

	m_delay += dt;
	if (m_delay >= 60.0f) {
		float seconds;
		m_delay = std::modf(m_delay, &seconds);
		m_currentTime += static_cast<unsigned int>(seconds);
		m_inclination = m_currentTime / 43200.0f * 180.0f - 90.00f;
		m_sunChanged = true;
	}
}

void SkySystem::setSun(std::shared_ptr<GameObject> sun)
{
	m_sun = sun;
	m_skyRenderingData = sun->getComponent<MeshComponent>();
	m_sunChanged = true;
}

std::shared_ptr<GameObject> SkySystem::createSun()
{
	auto sun = m_manager->create();
	m_skyRenderingData = sun->assign<MeshComponent>(m_cube.get(), std::move(std::make_unique<SkyMaterial>()));

	auto directionalLight = sun->assign<LightComponent>();
	directionalLight->setType(LightComponent::DIRECTIONAL);
	directionalLight->setShadowCastingEnabled(true);

	return std::move(sun);
}

void SkySystem::setTime(unsigned int hours, unsigned int minutes)
{
	m_currentTime = hours * 60 * 60 + minutes * 60;
	m_inclination = m_currentTime / 43200.0f * 180.0f - 90.0f;
	m_sunChanged = true;
}

void SkySystem::setAzimuth(float azimuth)
{
	m_azimuth = azimuth;
	m_sunChanged = true;
}

float SkySystem::getAzimuth() const
{
	return m_azimuth;
}

void SkySystem::updateSun()
{
	if (m_sunChanged && m_sun != nullptr && m_skyRenderingData.isValid()) {
		Log::write(m_currentTime, m_inclination);
		m_sun->setRotation(m_inclination, 0.0f, 0.0f);
		//m_sun->rotate(0.0f, m_azimuth, 0.0f);

		m_skyRenderingData->getMaterial()->as<SkyMaterial>()->setSunDirection(m_sun->getDirectionFront());

		m_sunChanged = false;
	}
}
