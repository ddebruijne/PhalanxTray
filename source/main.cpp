#include "tray.hpp"
#include "serial/serial.h"
#include <iostream>

#include "savedata.h"

using namespace Tray;
using namespace PhalanxTray;

std::vector<TrayMenu*> menu;
TrayIcon tr = { "icon.png", "icon.ico", "Phalanx Tray App", menu };
TrayMenu comPorts = { "Serial Port", true, false, false, nullptr };
TrayMenu status = { "Disconnected", false };

void buildComPortMenu()
{
	for (TrayMenu* t : comPorts.subMenu)
		delete t;
	comPorts.subMenu.clear();

	SaveData* sav = saveHandler.GetCurrentSaveData();
	std::vector<serial::PortInfo> devices_found = serial::list_ports();
	for (serial::PortInfo device : devices_found) 
	{
		comPorts.subMenu.push_back(new TrayMenu { device.port + " ("+ device.description + ")", true, strcmp(sav->serialport, device.port.c_str()) == 0, false, [=](TrayMenu* tm){ 
			for (TrayMenu* t : comPorts.subMenu)
				t->isChecked = false;

			tm->isChecked = true;
			strcpy(saveHandler.GetCurrentSaveData()->serialport, device.port.c_str());
			saveHandler.SaveCurrentData();

			buildComPortMenu();
			trayMaker.Update();
		} });
	}
}

int main()
{
	saveHandler.CreateAndLoadSave();
	SaveData* sav = saveHandler.GetCurrentSaveData();

	buildComPortMenu();
	TrayMenu model = { "Model", true, false, false, nullptr, { 
		new TrayMenu { "Phalanx", true, sav->model == EModel::Phalanx, false, [&](TrayMenu* tm){
			for (TrayMenu* t : model.subMenu)
				t->isChecked = false;
			
			tm->isChecked = true;
			sav->model = EModel::Phalanx;
			saveHandler.SaveCurrentData();
			trayMaker.Update();
		}},
		new TrayMenu { "Ameise", true, sav->model == EModel::Ameise, false, [&](TrayMenu* tm){
			for (TrayMenu* t : model.subMenu)
				t->isChecked = false;
			
			tm->isChecked = true;
			sav->model = EModel::Ameise;
			saveHandler.SaveCurrentData();
			trayMaker.Update();
		}},
		new TrayMenu { "Noctiluca", true, sav->model == EModel::Noctiluca, false, [&](TrayMenu* tm){
			for (TrayMenu* t : model.subMenu)
				t->isChecked = false;
			
			tm->isChecked = true;
			sav->model = EModel::Noctiluca;
			saveHandler.SaveCurrentData();
			trayMaker.Update();
		}},
	}};

	tr.menu.push_back(&status);
	tr.menu.push_back(new TrayMenu { "-" });
	tr.menu.push_back(&comPorts);
	tr.menu.push_back(&model);
	tr.menu.push_back(new TrayMenu { "Exit", true, false, false, [&](TrayMenu* tm){ 
		trayMaker.Exit();
	} });

	if (trayMaker.Initialize(&tr))
		while(trayMaker.Loop(1)) {	}
	else
		std::cout << "initialization failed" << std::endl;

	return 0;
}