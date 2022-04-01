#include "ContentMode/ContentModeFFXIV.h"
#include <iostream>
#include "loguru.hpp"

using namespace PhalanxTray;

ContentModeFFXIV::ContentModeFFXIV(serial::Serial* serialConnPtr) : 
    ContentModeBase(EContentModeId::FinalFantasyXIV, "Final Fantasy XIV", true, serialConnPtr)
{
    updateFrequency = 1;
}

void ContentModeFFXIV::OnActivate()
{
    LOG_F(INFO, "Enabled Content Mode: Final Fantasy XIV");
    currentDisplayText = "ff";
}

void ContentModeFFXIV::OnDeactivate()
{
}

void ContentModeFFXIV::OnTick()
{
}

void ContentModeFFXIV::OnDataReceived(std::vector<std::string>& data)
{
    currentDisplayText = data[0];
}