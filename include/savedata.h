#ifndef SAVEDATA_H
#define SAVEDATA_H

#include "defines.h"
#include <filesystem>

namespace PhalanxTray 
{
	struct SaveData_TimeSettings
	{
		bool showSeconds = false;
		bool showDate = true;
	};

	struct SaveData 
	{
		char serialport[100];
		EModel model;
		SaveData_TimeSettings timeSettings;
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

		// SINGLETON HANDLING
		static SaveHandler& GetInstance()
		{
			static SaveHandler inst;
			return inst;
		}        
	private:
		SaveHandler() {}
		SaveHandler(SaveHandler const&) = delete;
    	void operator=(SaveHandler const&) = delete;
	};
};

#endif // SAVEDATA_H