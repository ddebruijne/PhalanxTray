#ifndef SAVEDATA_H
#define SAVEDATA_H

#include "defines.h"
#include <filesystem>

namespace PhalanxTray 
{
	struct SaveData 
	{
		char serialport[100];
		EModel model;
	};

	class SaveHandler
	{
	private:
		std::filesystem::path saveFilePath = std::filesystem::current_path() /= saveFileName;;
		SaveData currentSaveData;
	
	public:
		bool CreateAndLoadSave();		// Creates a new save file if none exists, or loads it if exists.
		bool SaveCurrentData();			// Saves the current save data.
		inline SaveData* GetCurrentSaveData() { return &currentSaveData; }
	};

	static SaveHandler saveHandler;
};

#endif // SAVEDATA_H