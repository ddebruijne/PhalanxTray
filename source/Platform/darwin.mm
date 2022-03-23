#include "platform.h"
#include <Cocoa/Cocoa.h>
#include <filesystem>

using namespace PhalanxTray;

std::filesystem::path Platform::GetSavePath()
{
	NSString* homeDir = NSHomeDirectory();
	std::filesystem::path result = std::string([homeDir UTF8String]);
	result /= "Library/Preferences/PhalanxTray.sav";
	return result;
}

std::filesystem::path Platform::GetLogPath()
{
	NSString* homeDir = NSHomeDirectory();
	std::filesystem::path result = std::string([homeDir UTF8String]);
	result /= "Library/Logs/PhalanxTray.log";
	return result;
}