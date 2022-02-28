#include "tray.hpp"
#include "serial/serial.h"
#include <iostream>
#include <thread>
#include <ctime>
#include <string>

#include "savedata.h"

using namespace Tray;
using namespace PhalanxTray;

std::vector<TrayMenu*> menu;
TrayIcon tr = { "icon.png", "icon.ico", "Phalanx Tray App", menu };
TrayMenu status = { "Disconnected", false };
TrayMenu comPorts = { "Serial Port", true, false, false, nullptr };
TrayMenu model;

serial::Serial serialConn;
std::thread* tickThread = nullptr;
std::atomic<bool> ticking = false;

void onTick()
{
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
	std::string toWrite = std::to_string(now->tm_hour) + std::to_string(now->tm_min) + std::to_string(now->tm_sec) + "\n";

	serialConn.write(toWrite);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void attemptConnect()
{
	if (serialConn.isOpen())
		return;

	SaveData* sav = saveHandler.GetCurrentSaveData();

	serialConn.setPort(std::string(sav->serialport));
	serialConn.setBaudrate(115200);
	serialConn.setTimeout(serial::Timeout::simpleTimeout(1000));
	try
	{
		serialConn.open();
	}
	catch (serial::SerialException e) 
	{
		status.text = "SerialException";
	}
	catch (serial::IOException e)
	{
		status.text = "IOException";
	}

	if (!serialConn.isOpen())
		return;

	status.text = "Connected to ";
	status.text += std::string(sav->serialport);
	serialConn.write("[MODE]serial\n");
	ticking = true;
	tickThread = new std::thread([](){ while(ticking) onTick(); });
}

void attemptDisconnect()
{
	if (!serialConn.isOpen())
		return;

	ticking = false;
	tickThread->join();
	delete tickThread;
	serialConn.write("[MODE]normal\n");
	serialConn.close();
	status.text = "Disconnected";	
}

void buildComPortMenu()
{
	for (TrayMenu* t : comPorts.subMenu)
		delete t;

	comPorts.subMenu.clear();

	SaveData* sav = saveHandler.GetCurrentSaveData();
	std::vector<serial::PortInfo> devices_found = serial::list_ports();
	for (serial::PortInfo device : devices_found) 
	{
		comPorts.subMenu.push_back(new TrayMenu { device.port + " ("+ device.description + ")", true, strcmp(sav->serialport, device.port.c_str()) == 0, false, [&, device](TrayMenu* tm){ 
			for (TrayMenu* t : comPorts.subMenu)
				t->isChecked = false;

			attemptDisconnect();

			tm->isChecked = true;
			strcpy(saveHandler.GetCurrentSaveData()->serialport, device.port.c_str());
			saveHandler.SaveCurrentData();

			attemptConnect();
			buildComPortMenu();
			trayMaker.Update();
		} });
	}
}

void buildModelMenu()
{
	SaveData* sav = saveHandler.GetCurrentSaveData();

	model = { "Model", true, false, false, nullptr, { 
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
}

int main()
{
	saveHandler.CreateAndLoadSave();
	SaveData* sav = saveHandler.GetCurrentSaveData();

	buildComPortMenu();
	buildModelMenu();

	tr.menu.push_back(&status);
	tr.menu.push_back(new TrayMenu { "-" });
	tr.menu.push_back(&comPorts);
	tr.menu.push_back(&model);
	tr.menu.push_back(new TrayMenu { "Exit", true, false, false, [&](TrayMenu* tm){ 
		trayMaker.Exit();
	} });

	if (trayMaker.Initialize(&tr))
	{
		if(strlen(sav->serialport) > 0)
			attemptConnect();

		trayMaker.Update();

		while(trayMaker.Loop(1));

		attemptDisconnect();
	}
	else
		std::cout << "initialization failed" << std::endl;

	return 0;
}