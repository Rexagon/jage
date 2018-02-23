#pragma once

#include <string>
#include <ctime>

// Current time in unix format
time_t getTime();

// Current date dd.mm.yyyy
std::string getDate();

// Current date and time
std::string getDateTime();
