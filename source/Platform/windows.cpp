#include "platform.h"
#include <filesystem>
#include <ShlObj_core.h>

using namespace PhalanxTray;

std::filesystem::path Platform::GetSavePath()
{
    TCHAR szPath[MAX_PATH];
    SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath);
	std::filesystem::path result = szPath;
	result /= "PhalanxTray.sav";
	return result;
}

std::filesystem::path Platform::GetLogPath()
{
    TCHAR szPath[MAX_PATH];
    SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath);
	std::filesystem::path result = szPath;
	result /= "PhalanxTray.log";
	return result;
}