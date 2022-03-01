#include "ContentMode/ContentModeTime.h"
#include "fmt/core.h"

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
    std::string toWrite = fmt::format("{:02}{:02}{:02}\n", now->tm_hour, now->tm_min, now->tm_sec);
	serialConn->write(toWrite);
}