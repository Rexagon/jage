#pragma once

#include <GL/glew.h>

#include "Math.h"

struct MeshGeometry
{
public:
	typedef int ComponentsMask;

	enum Components {
		POSITIONS = 1 << 0,
		TEX_COORDS = 1 << 1,
		NORMALS = 1 << 2,
		//TANGENTS = 1 << 3,
		//BITANGENTS = 1 << 4,
	};

	static const int SIMPLE_VERTEX = POSITIONS;
	static const int TEXTURED_VERTEX = POSITIONS | TEX_COORDS;
	static const int MODEL_VERTEX = POSITIONS | TEX_COORDS | NORMALS;

	static MeshGeometry createQuad(const vec2& halfSize = vec2(1.0f), 
		ComponentsMask vertexComponents = MODEL_VERTEX);

	static MeshGeometry createPlane(const vec2& halfSize = vec2(5.0f), unsigned int xSegments = 1, unsigned int ySegments = 1, 
		ComponentsMask vertexComponents = MODEL_VERTEX);

	static MeshGeometry createCube(const vec3& halfSize = vec3(1.0f), 
		ComponentsMask vertexComponents = MODEL_VERTEX);

	static MeshGeometry createSphere(float radius, unsigned int xSegments, unsigned int ySegments, 
		ComponentsMask vertexComponents = MODEL_VERTEX);

	MeshGeometry(ComponentsMask vertexComponents = MODEL_VERTEX, GLenum topology = GL_TRIANGLES);

	std::vector<vec3> positions;
	std::vector<vec2> texCoords;
	std::vector<vec3> normals;
	//std::vector<vec3> tangents;
	//std::vector<vec3> bitangents;
	std::vector<unsigned int> indices;

	ComponentsMask vertexComponents;
	GLenum topology;
};