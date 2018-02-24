#include "Material.h"

#include <GL/glew.h>

Material::Material(sf::Texture * diffuseTexture, sf::Texture * normalTexture, 
	sf::Texture * specularTexture) :
	m_diffuseTexture(diffuseTexture), m_normalTexture(normalTexture), 
	m_specularTexture(specularTexture)
{
}

void Material::bind(size_t id)
{
	if (m_diffuseTexture != nullptr) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_diffuseTexture->getNativeHandle());
	}

	if (m_normalTexture != nullptr) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_normalTexture->getNativeHandle());
	}

	if (m_specularTexture != nullptr) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_specularTexture->getNativeHandle());
	}
}

void Material::setDiffuseTexture(sf::Texture * diffuseTexture)
{
	m_diffuseTexture = diffuseTexture;
}

void Material::setNormalTexture(sf::Texture * normalTexture)
{
	m_normalTexture = normalTexture;
}

void Material::setSpecularTexture(sf::Texture * specularTexture)
{
	m_specularTexture = specularTexture;
}
