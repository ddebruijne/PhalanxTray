#ifndef PLATFORM_H
#define PLATFORM_H

#include "defines.h"
#include <filesystem>

namespace PhalanxTray 
{
	class Platform
	{
	public:
		static std::filesystem::path GetSavePath();
		static std::filesystem::path GetLogPath();
	};
};

#endif // PLATFORM_H