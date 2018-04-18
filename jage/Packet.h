#pragma once

#include <vector>

class Packet
{
	typedef bool (Packet::*BoolType)(std::size_t);

public:
	Packet();
	Packet(const void* data, size_t size);

	void append(const void* data, size_t size);
	void clear();

	const void* getData() const;
	size_t getDataSize() const;

	bool endOfPacket() const;

	Packet& operator<<(const bool& data);
	Packet& operator<<(const int8_t& data);
	Packet& operator<<(const uint8_t& data);
	Packet& operator<<(const int16_t& data);
	Packet& operator<<(const uint16_t& data);
	Packet& operator<<(const int32_t& data);
	Packet& operator<<(const uint32_t& data);
	Packet& operator<<(const int64_t& data);
	Packet& operator<<(const uint64_t& data);
	Packet& operator<<(const float& data);
	Packet& operator<<(const double& data);
	Packet& operator<<(const char* data);
	Packet& operator<<(const std::string& data);
	Packet& operator<<(const wchar_t* data);
	Packet& operator<<(const std::wstring& data);

	Packet& operator>>(bool& data);
	Packet& operator>>(int8_t& data);
	Packet& operator>>(uint8_t& data);
	Packet& operator>>(int16_t& data);
	Packet& operator>>(uint16_t& data);
	Packet& operator>>(int32_t& data);
	Packet& operator>>(uint32_t& data);
	Packet& operator>>(int64_t& data);
	Packet& operator>>(uint64_t& data);
	Packet& operator>>(float& data);
	Packet& operator>>(double& data);
	Packet& operator>>(char* data);
	Packet& operator>>(std::string& data);
	Packet& operator>>(wchar_t* data);
	Packet& operator>>(std::wstring& data);

	operator BoolType();

protected:
	virtual const void* onSend(size_t& size);
	virtual void onReceive(const void* data, size_t size);

private:
	bool checkSize(size_t size);

	std::vector<char> m_data;

	size_t m_readPosition;
	size_t m_sendPosition;

	bool m_isValid;
};