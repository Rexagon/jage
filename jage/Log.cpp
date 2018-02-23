#include "Log.h"

std::ofstream Log::m_file;

void Log::init(const std::string & path)
{
	m_file.open(path);
	if (!m_file.is_open()) {
		throw std::runtime_error("Unable to create log file: \"" + path + "\"");
	}
}

std::wostream & operator<<(std::wostream & stream, const std::string & string)
{
	stream << string.c_str();
	return stream;
}