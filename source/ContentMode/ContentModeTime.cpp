#include "ContentMode/ContentModeTime.h"
#include "fmt/core.h"
#include "savedata.h"

using namespace PhalanxTray;

ContentModeTime::ContentModeTime(serial::Serial* serialConnPtr) : 
    ContentModeBase(EContentModeId::Time, "Time", false, serialConnPtr)
{

}

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
    
    if (now->tm_sec >= 28 && now->tm_sec <= 32 && sav->timeSettings.showDate)
    {
        switch (sav->model)
        {
            case EModel::Phalanx:
                currentDisplayText = fmt::format("{:02}  {:02}", now->tm_mday, now->tm_mon);
                break;
            case EModel::Ameise:
                currentDisplayText = fmt::format("{:02}   {}", now->tm_mday, monthMap[now->tm_mon]);
                break;
            case EModel::Noctiluca:
                currentDisplayText = fmt::format("{:02}{:02}", now->tm_mday, now->tm_mon);
                break;
            default:
                break;
        }
    }
    else
    {
        if (sav->timeSettings.showSeconds)
            currentDisplayText = fmt::format("{:02}{:02}{:02}", now->tm_hour, now->tm_min, now->tm_sec);
        else
            currentDisplayText = fmt::format("{:02}{:02}", now->tm_hour, now->tm_min);

        currentDisplayText = fmt::format("{:^{}}", currentDisplayText, GetAmountTubes(sav->model));
    }
}