#include "ContentMode/ContentModeTime.h"
#include "fmt/core.h"
#include "savedata.h"
#include "loguru.hpp"

using namespace PhalanxTray;

ContentModeTime::ContentModeTime(serial::Serial* serialConnPtr) : 
    ContentModeBase(EContentModeId::Time, "Time", false, serialConnPtr)
{
    currentOffset = 0;
    std::time_t t = std::time(0);   // get current system time
    std::tm* now = std::localtime(&t);
    prevHr = now->tm_hour;
}

void ContentModeTime::OnActivate()
{
    LOG_F(INFO, "Enabled Content Mode: Time");
    std::time_t t = std::time(0);   // get current system time
    std::tm* now = std::localtime(&t);
    prevHr = now->tm_hour;
}

void ContentModeTime::OnDeactivate()
{
}

void ContentModeTime::OnTick()
{
    SaveData* sav = SaveHandler::GetInstance().GetCurrentSaveData();

    std::time_t t = std::time(0);   // get current system time
    std::tm* now = std::localtime(&t);
    
    if (now->tm_sec >= 28 && now->tm_sec <= 32 && sav->timeSettings.showDate)
    {
        switch (sav->model)
        {
            case EModel::Phalanx:
                currentDisplayText = fmt::format("{:02}  {:02}", now->tm_mday, now->tm_mon+1);
                break;
            case EModel::Ameise:
                currentDisplayText = fmt::format("{:02}   {}", now->tm_mday, monthMap[now->tm_mon]);
                break;
            case EModel::Noctiluca:
                currentDisplayText = fmt::format("{:02}{:02}", now->tm_mday, now->tm_mon+1);
                break;
            default:
                break;
        }
    }
    else
    {
        if (prevHr < now->tm_hour)
        {
            prevHr = now->tm_hour;
            currentOffset++;
        }

        //read from map here as we may live switch.
        auto currentOffsetMap = this->timeOffsetMap;
        if (sav->timeSettings.showSeconds)
            currentOffsetMap = this->timeOffsetMapSeconds;

        if (currentOffset >= currentOffsetMap[sav->model].size())
            currentOffset = 0;

        // display data
        currentDisplayText = fmt::format(currentOffsetMap[sav->model][currentOffset], now->tm_hour, now->tm_min, now->tm_sec);
    }
}