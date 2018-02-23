#pragma once

#include <iostream>
#include <fstream>
#include <time.h>

#include "Time.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

template<class T>
std::ostream& operator<<(std::ostream& stream, const sf::Vector2<T>& v)
{
	stream << "[" << v.x << ", " << v.y << "]";
	return stream;
}

template<class T>
std::ostream& operator<<(std::ostream& stream, const sf::Vector3<T>& v)
{
	stream << "[" << v.x << ", " << v.y << ", " << v.z << "]";
	return stream;
}

class Log
{
public:
	// Creates output file
	static void init(const std::string& path = "log.txt");

	// Prints specified parameters to console and file 
	// Adds spaces between them and adds new line symbol at the end
	template<class Arg, class... Args>
	static void write(Arg&& arg, Args&&... args)
	{
		writeToStream(m_file, arg, args...);
		writeToStream(std::cout, arg, args...);
	}

private:
	// One magic function to 
	template<class Arg, class... Args>
	static void writeToStream(std::ostream& out, Arg&& arg, Args&&... args)
	{
		out << "[" << getDateTime() << "] " << std::forward<Arg>(arg);
		using dummy = int[];
		(void)dummy {
			0, (void(out << ' ' << std::forward<Args>(args)), 0)...
		};

		out << std::endl;
	}

	static std::ofstream m_file;
};
