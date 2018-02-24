#pragma once

#include <SFML/Graphics/Texture.hpp>

class Material
{
public:
	Material(sf::Texture* diffuseTexture = nullptr, sf::Texture* normalTexture = nullptr, 
		sf::Texture* specularTexture = nullptr);

	void bind(size_t id);

	void setDiffuseTexture(sf::Texture* diffuseTexture);
	void setNormalTexture(sf::Texture* normalTexture);
	void setSpecularTexture(sf::Texture* specularTexture);

private:
	sf::Texture* m_diffuseTexture;
	sf::Texture* m_normalTexture;
	sf::Texture* m_specularTexture;
};