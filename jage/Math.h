#pragma once

#include <vector>
#include <cmath>

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Constants.h"

typedef glm::vec2 vec2;
typedef glm::uvec2 uvec2;
typedef glm::ivec2 ivec2;

typedef glm::vec3 vec3;
typedef glm::uvec3 uvec3;
typedef glm::ivec3 ivec3;

typedef glm::vec4 vec4;
typedef glm::uvec4 uvec4;
typedef glm::ivec4 ivec4;

typedef glm::mat4 mat4;

using quat = glm::quat;

// GLM conversions
vec2 toGLM(const sf::Vector2f& v);
uvec2 toGLM(const sf::Vector2u& v);
ivec2 toGLM(const sf::Vector2i& v);
vec3 toGLM(const sf::Vector3f& v);
ivec3 toGLM(const sf::Vector3i& v);

// SFML conversions
sf::Vector2f toSFML(const vec2& v);
sf::Vector2u toSFML(const uvec2& v);
sf::Vector2i toSFML(const ivec2& v);
sf::Vector3f toSFML(const vec3& v);
sf::Vector3i toSFML(const ivec3& v);

namespace math
{
	float interpolate(const std::vector<std::pair<float, float>>& curve, float x);

	sf::Color lerp(const sf::Color& a, const sf::Color& b, float t);
	
	// Checks intersection between two lines
	bool intersect(const vec2& a1, const vec2& a2, const vec2& b1, const vec2& b2);

	sf::Color colorFromHex(const std::string& hex);

	// Decodes string in base64
	std::vector<char> decodeBase64(const std::string& encodedString);

	// Decompresses zlib compressed buffer
	std::vector<char> decompress(const std::vector<char>& buffer);
}