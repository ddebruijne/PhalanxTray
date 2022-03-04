#include "ContentMode/ContentModeFFXIV.h"
#include <iostream>

using namespace PhalanxTray;

ContentModeFFXIV::ContentModeFFXIV(serial::Serial* serialConnPtr) : 
    ContentModeBase(EContentModeId::FinalFantasyXIV, "Final Fantasy XIV", true, serialConnPtr)
{

}

void ContentModeFFXIV::OnActivate()
{
}

void ContentModeFFXIV::OnDeactivate()
{
}

void ContentModeFFXIV::OnTick()
{
}

void ContentModeFFXIV::OnDataReceived(std::vector<std::string>& data)
{
    std::cout << "Huzzah!" << std::endl;
}