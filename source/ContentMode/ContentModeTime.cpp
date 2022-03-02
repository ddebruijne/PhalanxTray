#include "ContentMode/ContentModeTime.h"
#include "fmt/core.h"
#include "savedata.h"

using namespace PhalanxTray;

void ContentModeTime::OnActivate()
{
}

void ContentModeTime::OnDeactivate()
{
}

void ContentModeTime::OnTick()
{
    SaveData* sav = SaveHandler::GetInstance().GetCurrentSaveData();

    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
    std::string toWrite;
    
    if (sav->timeSettings.showSeconds)
        toWrite = fmt::format("{:02}{:02}{:02}\n", now->tm_hour, now->tm_min, now->tm_sec);
    else
        toWrite = fmt::format("{:02}{:02}\n", now->tm_hour, now->tm_min);

	serialConn->write(toWrite);
}