#include "SkySystem.h"

#include "ShaderFactory.h"
#include "MeshComponent.h"
#include "LightComponent.h"
#include "ResourceManager.h"

SkySystem::SkySystem()
{
	m_cube = std::make_unique<Mesh>();
	m_cube->init(MeshGeometry::createCube());

	ShaderFactory::FromFile skyVertexSource("shaders/sky.vert");
	ShaderFactory::FromFile skyFragmentSource("shaders/sky.frag");
	ResourceManager::bind<ShaderFactory>("sky_shader", skyVertexSource, skyFragmentSource);
	Shader* skyShader = ResourceManager::get<Shader>("sky_shader");
	skyShader->setAttribute(0, "position");
	skyShader->setAttribute(1, "texCoords");

	m_skyMaterial = std::make_unique<Material>(skyShader);
	m_skyMaterial->setType(Material::CUSTOM);
	m_skyMaterial->setShadowCastingEnabled(false);
	m_skyMaterial->setShadowReceivingEnabled(false);
	m_skyMaterial->setDepthWriteEnabled(false);
	m_skyMaterial->setFaceCullingEnabled(false);
}

void SkySystem::update(const float dt)
{
	updateSun();

	m_delay += dt;
	if (m_delay >= 60.0f) {
		float seconds;
		m_delay = std::modf(m_delay, &seconds);
		m_currentTime += static_cast<unsigned int>(seconds);
		m_inclination = m_currentTime / 43200.0f * glm::pi<float>();
		m_direcitonChanged = true;
	}
}

void SkySystem::setSun(std::shared_ptr<GameObject> sun)
{
	m_sunComponent = sun->getComponent<SunComponent>();
	m_direcitonChanged = true;
}

std::shared_ptr<GameObject> SkySystem::createSun()
{
	auto sun = m_manager->create();
	sun->assign<SunComponent>();
	sun->assign<MeshComponent>(m_cube.get(), *m_skyMaterial.get());

	auto directionalLight = sun->assign<LightComponent>();
	directionalLight->setType(LightComponent::DIRECTIONAL);
	directionalLight->setShadowCastingEnabled(true);

	return std::move(sun);
}

void SkySystem::setTime(unsigned int hours, unsigned int minutes)
{
	m_currentTime = hours * 24 * 60 * 60 + minutes * 60;
	m_direcitonChanged = true;
}

void SkySystem::setAzimuth(float azimuth)
{
	m_azimuth = azimuth;
	m_direcitonChanged = true;
}

float SkySystem::getAzimuth() const
{
	return m_azimuth;
}

void SkySystem::updateSun()
{
	if (m_direcitonChanged && m_sunComponent.isValid()) {
		m_direction.x = cos(m_inclination) * sin(m_azimuth);
		m_direction.y = sin(m_inclination);
		m_direction.z = cos(m_inclination) * cos(m_azimuth);
		m_direcitonChanged = false;
	}
}
