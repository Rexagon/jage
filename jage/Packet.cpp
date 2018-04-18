#include "Packet.h"

#include <UDT/udt.h>

Packet::Packet() :
	m_readPosition(0), m_sendPosition(0), m_isValid(true)
{
}

Packet::Packet(const void * data, size_t size) :
	m_readPosition(0), m_sendPosition(0), m_isValid(true)
{
	append(data, size);
}

void Packet::append(const void * data, size_t size)
{
	if (data != nullptr && size > 0) {
		size_t start = m_data.size();
		m_data.resize(start + size);
		std::memcpy(&m_data[start], data, size);
	}
}

void Packet::clear()
{
	m_data.clear();
	m_readPosition = 0;
	m_isValid = true;
}

const void * Packet::getData() const
{
	return m_data.data();
}

size_t Packet::getDataSize() const
{
	return m_data.size();
}

bool Packet::endOfPacket() const
{
	return m_readPosition >= m_data.size();
}

Packet & Packet::operator<<(const bool & data)
{
	return *this << static_cast<uint8_t>(data);
}

Packet & Packet::operator<<(const int8_t & data)
{
	append(&data, sizeof(data));
	return *this;
}

Packet & Packet::operator<<(const uint8_t & data)
{
	append(&data, sizeof(data));
	return *this;
}

Packet & Packet::operator<<(const int16_t & data)
{
	int16_t reorderedData = htons(data);
	append(&reorderedData, sizeof(data));
	return *this;
}

Packet & Packet::operator<<(const uint16_t & data)
{
	uint16_t reorderedData = htons(data);
	append(&reorderedData, sizeof(data));
	return *this;
}

Packet & Packet::operator<<(const int32_t & data)
{
	int32_t reorderedData = htonl(data);
	append(&reorderedData, sizeof(data));
	return *this;
}

Packet & Packet::operator<<(const uint32_t & data)
{
	uint32_t reorderedData = htonl(data);
	append(&reorderedData, sizeof(data));
	return *this;
}

Packet & Packet::operator<<(const int64_t & data)
{
	int64_t reorderedData = htonll(data);
	append(&reorderedData, sizeof(data));
	return *this;
}

Packet & Packet::operator<<(const uint64_t & data)
{
	uint64_t reorderedData = htonll(data);
	append(&reorderedData, sizeof(data));
	return *this;
}

Packet & Packet::operator<<(const float & data)
{
	uint32_t reorderedData = htonf(data);
	append(&reorderedData, sizeof(data));
	return *this;
}

Packet & Packet::operator<<(const double & data)
{
	uint64_t reorderedData = htond(data);
	append(&reorderedData, sizeof(data));
	return *this;
}

Packet & Packet::operator<<(const char * data)
{
	uint32_t length = static_cast<uint32_t>(std::strlen(data));
	*this << length;

	append(data, length);

	return *this;
}

Packet & Packet::operator<<(const std::string & data)
{
	uint32_t length = static_cast<uint32_t>(data.size());
	*this << length;

	append(data.c_str(), length * sizeof(std::string::value_type));

	return *this;
}

Packet & Packet::operator<<(const wchar_t * data)
{
	uint32_t length = static_cast<uint32_t>(std::wcslen(data));
	*this << length;

	append(data, length * sizeof(wchar_t));

	return *this;
}

Packet & Packet::operator<<(const std::wstring & data)
{
	uint32_t length = static_cast<uint32_t>(data.size());
	*this << length;

	append(data.c_str(), length * sizeof(std::wstring::value_type));

	return *this;
}

Packet & Packet::operator>>(bool & data)
{
	uint8_t value;
	if (*this >> value) {
		data = value != 0;
	}

	return *this;
}

Packet & Packet::operator>>(int8_t & data)
{
	if (checkSize(sizeof(data))) {
		data = *reinterpret_cast<const int8_t*>(&m_data[m_readPosition]);
		m_readPosition += sizeof(data);
	}
	
	return *this;
}

Packet & Packet::operator>>(uint8_t & data)
{
	if (checkSize(sizeof(data))) {
		data = *reinterpret_cast<const uint8_t*>(&m_data[m_readPosition]);
		m_readPosition += sizeof(data);
	}

	return *this;
}

Packet & Packet::operator>>(int16_t & data)
{
	if (checkSize(sizeof(data))) {
		data = ntohs(*reinterpret_cast<const int16_t*>(&m_data[m_readPosition]));
		m_readPosition += sizeof(data);
	}

	return *this;
}

Packet & Packet::operator>>(uint16_t & data)
{
	if (checkSize(sizeof(data))) {
		data = ntohs(*reinterpret_cast<const uint16_t*>(&m_data[m_readPosition]));
		m_readPosition += sizeof(data);
	}

	return *this;
}

Packet & Packet::operator>>(int32_t & data)
{
	if (checkSize(sizeof(data))) {
		data = ntohl(*reinterpret_cast<const int32_t*>(&m_data[m_readPosition]));
		m_readPosition += sizeof(data);
	}

	return *this;
}

Packet & Packet::operator>>(uint32_t & data)
{
	if (checkSize(sizeof(data))) {
		data = ntohl(*reinterpret_cast<const uint32_t*>(&m_data[m_readPosition]));
		m_readPosition += sizeof(data);
	}

	return *this;
}

Packet & Packet::operator>>(int64_t & data)
{
	if (checkSize(sizeof(data))) {
		data = ntohll(*reinterpret_cast<const int64_t*>(&m_data[m_readPosition]));
		m_readPosition += sizeof(data);
	}

	return *this;
}

Packet & Packet::operator>>(uint64_t & data)
{
	if (checkSize(sizeof(data))) {
		data = ntohll(*reinterpret_cast<const uint64_t*>(&m_data[m_readPosition]));
		m_readPosition += sizeof(data);
	}

	return *this;
}

Packet & Packet::operator>>(float & data)
{
	if (checkSize(sizeof(data))) {
		data = ntohf(*reinterpret_cast<const uint32_t*>(&m_data[m_readPosition]));
		m_readPosition += sizeof(data);
	}

	return *this;
}

Packet & Packet::operator>>(double & data)
{
	if (checkSize(sizeof(data))) {
		data = ntohd(*reinterpret_cast<const uint64_t*>(&m_data[m_readPosition]));
		m_readPosition += sizeof(data);
	}

	return *this;
}

Packet & Packet::operator>>(char * data)
{
	uint32_t length = 0;
	*this >> length;

	if (length > 0 && checkSize(length)) {
		std::memcpy(data, &m_data[m_readPosition], length);
		data[length] = '\0';

		m_readPosition += length;
	}

	return *this;
}

Packet & Packet::operator>>(std::string & data)
{
	uint32_t length = 0;
	*this >> length;

	data.clear();
	if (length > 0 && checkSize(length)) {
		data.assign(&m_data[m_readPosition], length);
		m_readPosition += length;
	}

	return *this;
}

Packet & Packet::operator>>(wchar_t * data)
{
	uint32_t length = 0;
	*this >> length;

	if (length > 0 && checkSize(length * sizeof(wchar_t))) {
		std::memcpy(data, &m_data[m_readPosition], length * sizeof(wchar_t));
		data[length] = L'\0';

		m_readPosition += length * sizeof(wchar_t);
	}

	return *this;
}

Packet & Packet::operator>>(std::wstring & data)
{
	uint32_t length = 0;
	*this >> length;

	data.clear();
	if (length > 0 && checkSize(length)) {
		data.assign(reinterpret_cast<const wchar_t*>(&m_data[m_readPosition]), length);
		m_readPosition += length * sizeof(std::wstring::value_type);
	}

	return *this;
}

Packet::operator BoolType()
{
	return m_isValid ? &Packet::checkSize : nullptr;
}

const void * Packet::onSend(size_t & size)
{
	size = getDataSize();
	return getData();
}

void Packet::onReceive(const void * data, size_t size)
{
	append(data, size);
}

bool Packet::checkSize(size_t size)
{
	m_isValid = m_isValid && (m_readPosition + size <= m_data.size());

	return m_isValid;
}
