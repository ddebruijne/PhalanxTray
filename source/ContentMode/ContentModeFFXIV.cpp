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
    serialConn->write(std::string("ff\n"));
}

void ContentModeFFXIV::OnDeactivate()
{
}

void ContentModeFFXIV::OnTick()
{
}

void ContentModeFFXIV::OnDataReceived(std::vector<std::string>& data)
{
    serialConn->write(data[0] + "\n");
}