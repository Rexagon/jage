#pragma once

#include "Material.h"

class MeshMaterial : public Material
{
public:
	MeshMaterial(Texture* albedoTexture = nullptr, Texture* normalsTexture = nullptr);

	void bind() override;

	void setAlbedoTexture(Texture* texture);
	Texture* getAlbedoTexture() const;

	void setNormalsTexture(Texture* texture);
	Texture* getNormalsTexture() const;

	void setUVScale(float x, float y);
	void setUVScale(const vec2& scale);
	vec2 getUVScale() const;

private:
	vec2 m_uvScale;
};