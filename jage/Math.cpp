#include "Math.h"

#include <string>
#include <cctype>

#include <zlib.h>

vec2 toGLM(const sf::Vector2f & v)
{
	return vec2(v.x, v.y);
}

uvec2 toGLM(const sf::Vector2u & v)
{
	return uvec2(v.x, v.y);
}

ivec2 toGLM(const sf::Vector2i & v)
{
	return ivec2(v.x, v.y);
}

vec3 toGLM(const sf::Vector3f & v)
{
	return vec3(v.x, v.y, v.z);
}

ivec3 toGLM(const sf::Vector3i & v)
{
	return ivec3(v.x, v.y, v.z);
}

sf::Vector2f toSFML(const vec2 & v)
{
	return sf::Vector2f(v.x, v.y);
}

sf::Vector2u toSFML(const uvec2 & v)
{
	return sf::Vector2u(v.x, v.y);
}

sf::Vector2i toSFML(const ivec2 & v)
{
	return sf::Vector2i(v.x, v.y);
}

sf::Vector3f toSFML(const vec3 & v)
{
	return sf::Vector3f(v.x, v.y, v.z);
}

sf::Vector3i toSFML(const ivec3 & v)
{
	return sf::Vector3i(v.x, v.y, v.z);
}


float math::interpolate(const std::vector<std::pair<float, float>>& curve, float x)
{
	if (x > curve.back().first) {
		return curve.back().second;
	}
	if (x < curve[0].first) {
		return curve[0].second;
	}

	std::vector<std::pair<float, float>>::const_iterator left, right;

	left = std::lower_bound(curve.begin(), curve.end(), std::make_pair(x, -INFINITY));

	if (left == curve.begin()) {
		return left->second;
	}

	right = left;
	--right;
	return right->second + (left->second - right->second) * (x - right->first) / (left->first - right->first);
}

sf::Color math::lerp(const sf::Color & a, const sf::Color & b, float t)
{
	return sf::Color(
		static_cast<sf::Uint8>(a.r * (1 - t) + b.r * t),
		static_cast<sf::Uint8>(a.g * (1 - t) + b.g * t),
		static_cast<sf::Uint8>(a.b * (1 - t) + b.b * t),
		static_cast<sf::Uint8>(a.a * (1 - t) + b.a * t));
}


float CCW(const vec2& a, const vec2& b, const vec2& c)
{
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool middle(float a, float b, float c)
{
	if (a < b) {
		return a <= c && c <= b;
	}
	else {
		return b <= c && c <= a;
	}
}

bool math::intersect(const vec2 & a1, const vec2 & a2, const vec2 & b1, const vec2 & b2)
{
	if ((CCW(a1, a2, b1) * CCW(a1, a2, b2) < 0) &&
		(CCW(b1, b2, a1) * CCW(b1, b2, a2) < 0))
	{
		return true;
	}

	return 
		(CCW(a1, a2, b1) == 0 && middle(a1.x, a2.x, b1.x) && middle(a1.y, a2.y, b1.y)) ||
		(CCW(a1, a2, b2) == 0 && middle(a1.x, a2.x, b2.x) && middle(a1.y, a2.y, b2.y)) ||
		(CCW(b1, b2, a1) == 0 && middle(b1.x, b2.x, a1.x) && middle(b1.y, b2.y, a1.y)) ||
		(CCW(b1, b2, a2) == 0 && middle(b1.x, b2.x, a2.x) && middle(b1.y, b2.y, a2.y));
}


sf::Color math::colorFromHex(const std::string & hex)
{
	size_t hexSize = hex.size();
	bool hasAlpha = hexSize == 9;
	if (hexSize != 7 && !hasAlpha) {
		return sf::Color::Black;
	}

	sf::Uint8 channels[4] = {};

	signed char currentChannel = 0;
	if (!hasAlpha) {
		channels[0] = 255;
		currentChannel = 1;
	}

	for (signed char i = 1; i < hex.size(); ++i) {
		char symbol = tolower(hex[i]);
		if (isxdigit(symbol)) {
			char number = 0;
			if (isdigit(symbol)) {
				number = symbol - '0';
			}
			else {
				number = 10 + symbol - 'a';
			}

			if (i % 2 == 1) {
				channels[currentChannel] += number * 16;
			}
			else {
				channels[currentChannel++] += number;
			}
		}
		else {
			return sf::Color::Black;
		}
	}

	return sf::Color(channels[1], channels[2], channels[3], channels[0]);
}

static const std::string base64Chars =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

bool isBase64(char c)
{
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::vector<char> math::decodeBase64(const std::string & encodedString)
{
	size_t encodedLength = encodedString.size();
	size_t currentSymbol = 0;
	unsigned char buffer4bit[4], buffer3bit[3];
	std::vector<char> result;

	int i = 0;
	while (--encodedLength && 
		(encodedString[currentSymbol] != '=') && 
		isBase64(encodedString[currentSymbol])) 
	{
		buffer4bit[i++] = encodedString[currentSymbol]; 
		++currentSymbol;

		if (i == 4) {
			for (i = 0; i < 4; ++i) {
				buffer4bit[i] = static_cast<unsigned char>(base64Chars.find(buffer4bit[i]));
			}

			buffer3bit[0] = (buffer4bit[0] << 2) + ((buffer4bit[1] & 0x30) >> 4);
			buffer3bit[1] = ((buffer4bit[1] & 0xf) << 4) + ((buffer4bit[2] & 0x3c) >> 2);
			buffer3bit[2] = ((buffer4bit[2] & 0x3) << 6) + buffer4bit[3];

			for (i = 0; i < 3; ++i) {
				result.push_back(buffer3bit[i]);
			}

			i = 0;
		}
	}

	if (i) {
		for (int j = i; j < 4; ++j) {
			buffer4bit[j] = 0;
		}

		for (int j = 0; j < 4; ++j) {
			buffer4bit[j] = static_cast<unsigned char>(base64Chars.find(buffer4bit[j]));
		}

		buffer3bit[0] = (buffer4bit[0] << 2) + ((buffer4bit[1] & 0x30) >> 4);
		buffer3bit[1] = ((buffer4bit[1] & 0xf) << 4) + ((buffer4bit[2] & 0x3c) >> 2);
		buffer3bit[2] = ((buffer4bit[2] & 0x3) << 6) + buffer4bit[3];

		for (int j = 0; j < i - 1; ++j) {
			result.push_back(buffer3bit[j]);
		}
	}

	return result;
}

std::vector<char> math::decompress(const std::vector<char>& buffer)
{
	z_stream zs;
	memset(&zs, 0, sizeof(zs));

	if (inflateInit(&zs) != Z_OK) {
		throw std::runtime_error("inflateInit failed while decompressing.");
	}

	zs.next_in = (Bytef*)buffer.data();
	zs.avail_in = static_cast<uInt>(buffer.size());

	int resultCode;
	char chunk[32768];
	std::vector<char> result;

	do {
		zs.next_out = reinterpret_cast<Bytef*>(chunk);
		zs.avail_out = sizeof(chunk);

		resultCode = inflate(&zs, 0);

		if (result.size() < zs.total_out) {
			size_t insertionSize = zs.total_out - result.size();

			result.insert(result.end(), std::begin(chunk), std::begin(chunk) + insertionSize);
		}

	} while (resultCode == Z_OK);

	inflateEnd(&zs);

	if (resultCode != Z_STREAM_END && zs.msg != nullptr) {
		throw std::runtime_error("zlib decompression failed: \"" + std::string(zs.msg) + "\"");
	}

	return result;
}