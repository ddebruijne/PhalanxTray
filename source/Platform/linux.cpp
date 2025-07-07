#include "platform.h"
#include <filesystem>

using namespace PhalanxTray;

std::filesystem::path Platform::GetSavePath()
{
    std::filesystem::path result = "~/.config/PhalanxTray/PhalanxTray.sav";
    return result;
}

std::filesystem::path Platform::GetLogPath()
{
    std::filesystem::path result = "~/.config/PhalanxTray/PhalanxTray.log";
    return result;
}
