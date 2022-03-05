#include "ContentMode/ContentModeFFXIV.h"
#include <iostream>

using namespace PhalanxTray;

ContentModeFFXIV::ContentModeFFXIV(serial::Serial* serialConnPtr) : 
    ContentModeBase(EContentModeId::FinalFantasyXIV, "Final Fantasy XIV", true, serialConnPtr)
{
    updateFrequency = 1;
}

void ContentModeFFXIV::OnActivate()
{
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