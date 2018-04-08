#pragma once

#include "Material.h"

class LightMaterial : public Material
{
public:
	LightMaterial();
	~LightMaterial();

	void bind() override;

	int getAlbedoTextureUnit() const;
	int getNormalsTextureUnit() const;
	int getSpecularTextureUnit() const;

	int getDepthTextureUnit() const;
	int getLightDepthTextureUnit() const;


	void setShadowsEnabled(bool enabled);
	bool areShadowsEnabled() const;

	void setColor(const vec3& color);
	vec3 getColor() const;


	void setDirection(const vec3& direction);
	vec3 getDirection() const;
	
	void setInversedViewProjection(const mat4& inversedViewProjection);
	mat4 getInversedViewProjection() const;

	void setLightViewProjection(const mat4& lightViewProjection);
	mat4 getLightViewProjection() const;

private:
	int m_albedoTextureUnit;
	int m_normalsTextureUnit;
	int m_specularTextureUnit;

	int m_depthTextureUnit;
	int m_lightDepthTextureUnit;

	bool m_areShadowsEnabled;
	vec3 m_color;

	vec3 m_direction;
	mat4 m_inversedViewProjection;
	mat4 m_lightViewProjection;
};