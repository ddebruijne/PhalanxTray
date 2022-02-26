#include "savedata.h"
#include <fstream>

using namespace PhalanxTray;

bool SaveHandler::CreateAndLoadSave()
{
	if(!std::filesystem::exists(saveFilePath)) 
		return SaveCurrentData();

	std::fstream saveFileHandle;
	saveFileHandle.open(saveFilePath, std::ios::in | std::ios::binary);
	saveFileHandle.seekg(0);
	saveFileHandle.read((char*)&currentSaveData, sizeof(currentSaveData));
	saveFileHandle.close();

	return true;
}

bool SaveHandler::SaveCurrentData()
{

	std::fstream saveFileHandle;
	saveFileHandle.open(saveFilePath, std::ios::out | std::ios::binary);
	saveFileHandle.write((char*)&currentSaveData, sizeof(currentSaveData));
	saveFileHandle.close();

	return true;
}