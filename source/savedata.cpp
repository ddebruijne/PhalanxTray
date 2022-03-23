#include "savedata.h"
#include "platform.h"
#include <fstream>
#include "loguru.hpp"

using namespace PhalanxTray;

bool SaveHandler::CreateAndLoadSave()
{
	if(!std::filesystem::exists(Platform::GetSavePath()))
	{
		LOG_F(INFO, "Creating new save file at location '%s'", Platform::GetSavePath().generic_string().c_str());
		return SaveCurrentData();
	}

	LOG_F(INFO, "Attempting to read save file from '%s'", Platform::GetSavePath().generic_string().c_str());
	std::fstream saveFileHandle;
	saveFileHandle.open(Platform::GetSavePath(), std::ios::in | std::ios::binary);
	saveFileHandle.seekg(0);
	saveFileHandle.read((char*)&currentSaveData, sizeof(currentSaveData));
	saveFileHandle.close();

	return true;
}

bool SaveHandler::SaveCurrentData()
{
	LOG_F(INFO, "Saving data at location '%s'", Platform::GetSavePath().generic_string().c_str());
	std::fstream saveFileHandle;
	saveFileHandle.open(Platform::GetSavePath(), std::ios::out | std::ios::binary);
	saveFileHandle.write((char*)&currentSaveData, sizeof(currentSaveData));
	saveFileHandle.close();

	return true;
}