#include "tray.hpp"
#include "serial/serial.h"
#include "asio.hpp"
#include <iostream>
#include <thread>

#include "savedata.h"
#include "ContentMode/ContentModeTime.h"

using namespace Tray;
using namespace PhalanxTray;

TrayIcon tr = { "icon.png", "icon.ico", "Phalanx Tray App" };
TrayMenu status = { "Disconnected", false };
TrayMenu comPorts = { "Serial Port", true, false, false, nullptr };
TrayMenu model;
TrayMenu settingsMenu;

serial::Serial serialConn;
serial::Timeout timeout(std::numeric_limits<uint32_t>::max(), 1000, 0, 1000, 0);

std::thread* tickThread = nullptr;
std::atomic<bool> ticking = false;

std::shared_ptr<ContentModeBase> currentContentMode = nullptr;
std::vector<std::shared_ptr<ContentModeBase>> allContentModes;

asio::io_context io_context;
asio::ip::udp::endpoint udpReceiver(asio::ip::address_v4::any(), 11001);
//asio::ip::udp::endpoint sendr(asio::ip::address_v4::broadcast(), 11001);
asio::ip::udp::socket udpSocket(io_context, udpReceiver);

void onTick()
{
	if (udpSocket.available())
	{
		char buffer[65536];
		asio::ip::udp::endpoint sender;
		std::size_t bytes_transferred = udpSocket.receive_from(asio::buffer(buffer), sender);
		std::string result(buffer, bytes_transferred);
		std::cout << result << std::endl;
	}

	// try
	// {
	// 	udpSocket.send_to(asio::buffer("ayy", 3), sendr);
	// }
	// catch (asio::system_error e)
	// {
	// 	std::cout << e.what() << std::endl;
	// }

	if (currentContentMode.get() != nullptr)
	{
		currentContentMode->OnTick();
		std::this_thread::sleep_for(std::chrono::milliseconds(currentContentMode->updateFrequency));
	}
	else
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
}

void attemptConnect()
{
	if (serialConn.isOpen())
		return;

	SaveData* sav = SaveHandler::GetInstance().GetCurrentSaveData();

	serialConn.setPort(std::string(sav->serialport));
	serialConn.setBaudrate(115200);
	serialConn.setTimeout(timeout);
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
	currentContentMode->OnActivate();
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
	currentContentMode->OnDeactivate();
	serialConn.write("[MODE]normal\n");
	serialConn.close();
	status.text = "Disconnected";	
}

void buildComPortMenu()
{
	for (TrayMenu* t : comPorts.subMenu)
		delete t;

	comPorts.subMenu.clear();

	SaveData* sav = SaveHandler::GetInstance().GetCurrentSaveData();
	std::vector<serial::PortInfo> devices_found = serial::list_ports();
	for (serial::PortInfo device : devices_found) 
	{
		comPorts.subMenu.push_back(new TrayMenu { device.port + " ("+ device.description + ")", true, strcmp(sav->serialport, device.port.c_str()) == 0, false, [=](TrayMenu* tm){ 
			for (TrayMenu* t : comPorts.subMenu)
				t->isChecked = false;

			attemptDisconnect();

			tm->isChecked = true;
			strcpy(sav->serialport, device.port.c_str());
			SaveHandler::GetInstance().SaveCurrentData();

			attemptConnect();
			buildComPortMenu();
			trayMaker.Update();
		} });
	}
}

void buildModelMenu()
{
	SaveData* sav = SaveHandler::GetInstance().GetCurrentSaveData();

	model = { "Model", true, false, false, nullptr, { 
		new TrayMenu { "Phalanx", true, sav->model == EModel::Phalanx, false, [=](TrayMenu* tm){
			for (TrayMenu* t : model.subMenu)
				t->isChecked = false;
			
			tm->isChecked = true;
			sav->model = EModel::Phalanx;
			SaveHandler::GetInstance().SaveCurrentData();
			trayMaker.Update();
		}},
		new TrayMenu { "Ameise", true, sav->model == EModel::Ameise, false, [=](TrayMenu* tm){
			for (TrayMenu* t : model.subMenu)
				t->isChecked = false;
			
			tm->isChecked = true;
			sav->model = EModel::Ameise;
			SaveHandler::GetInstance().SaveCurrentData();
			trayMaker.Update();
		}},
		new TrayMenu { "Noctiluca", true, sav->model == EModel::Noctiluca, false, [=](TrayMenu* tm){
			for (TrayMenu* t : model.subMenu)
				t->isChecked = false;
			
			tm->isChecked = true;
			sav->model = EModel::Noctiluca;
			SaveHandler::GetInstance().SaveCurrentData();
			trayMaker.Update();
		}},
	}};
}

void buildSettingsMenu()
{
	SaveData* sav = SaveHandler::GetInstance().GetCurrentSaveData();

	settingsMenu = { "Settings", true, false, false, nullptr, {
		new TrayMenu { "Show Seconds", true, sav->timeSettings.showSeconds, false, [=](TrayMenu* tm){
			sav->timeSettings.showSeconds = !sav->timeSettings.showSeconds;
			
			tm->isChecked = sav->timeSettings.showSeconds;
			SaveHandler::GetInstance().SaveCurrentData();
			trayMaker.Update();
		}},
	}};
}

int main()
{
	SaveHandler::GetInstance().CreateAndLoadSave();
	SaveData* sav = SaveHandler::GetInstance().GetCurrentSaveData();

	buildComPortMenu();
	buildModelMenu();
	buildSettingsMenu();

	tr.menu.push_back(&status);
	tr.menu.push_back(new TrayMenu { "-" });
	tr.menu.push_back(&comPorts);
	tr.menu.push_back(&model);
	tr.menu.push_back(&settingsMenu);
	tr.menu.push_back(new TrayMenu { "Exit", true, false, false, [&](TrayMenu* tm){ 
		trayMaker.Exit();
	} });

	currentContentMode = std::make_shared<ContentModeTime>(&serialConn);
	allContentModes.push_back(currentContentMode);

	// udpSocket.set_option(asio::ip::udp::socket::reuse_address(true));
  	// udpSocket.set_option(asio::socket_base::broadcast(true));

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