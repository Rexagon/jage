#include "Time.h"

time_t getTime()
{
	return time(nullptr);
}

std::string getDate()
{
	time_t now = getTime();
	struct tm* tstruct;
	char buf[11];
	tstruct = localtime(&now);
	strftime(buf, sizeof(buf), "%d.%m.%Y", tstruct);

	return buf;
}

std::string getDateTime()
{
	time_t now = getTime();
	struct tm* tstruct;
	char buf[20];
	tstruct = localtime(&now);
	strftime(buf, sizeof(buf), "%d.%m.%Y %X", tstruct);

	return buf;
}
