#include "ContentMode/ContentModeTime.h"

using namespace PhalanxTray;

void ContentModeTime::OnActivate()
{
}

void ContentModeTime::OnDeactivate()
{
}

void ContentModeTime::OnTick()
{
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
	std::string toWrite = std::to_string(now->tm_hour) + std::to_string(now->tm_min) + std::to_string(now->tm_sec) + "\n";

	serialConn->write(toWrite);
}