#include "tray.hpp"
#include "serial/serial.h"
#include "asio.hpp"
#include "loguru.hpp"
#include <iostream>
#include <thread>

#include "savedata.h"
#include "ContentMode/ContentModeTime.h"
#include "ContentMode/ContentModeFFXIV.h"

using namespace Tray;
using namespace PhalanxTray;

TrayIcon tr = { "icon.png", "icon.ico", "Phalanx Tray App" };
TrayMenu status = { "Disconnected", false };
TrayMenu comPorts = { "Serial Port", true, false, false, nullptr };
TrayMenu model;
TrayMenu settingsMenu;
TrayMenu contentModeMenu = { "Content mode", true, false, false, nullptr };

serial::Serial serialConn;
serial::Timeout timeout(std::numeric_limits<uint32_t>::max(), 1000, 0, 1000, 0);
std::string lastKnownDisplayText = "";

std::thread* tickThread = nullptr;
std::atomic<bool> ticking = false;

struct ContentSwitchInformation { EContentModeId newContentMode = EContentModeId::Time; bool wantsSwitch = false; };
std::atomic<ContentSwitchInformation> contentSwitchInformation;
std::shared_ptr<ContentModeBase> currentContentMode = nullptr;
std::unordered_map<EContentModeId, std::shared_ptr<ContentModeBase>> contentModes;

asio::io_context io_context;
asio::ip::udp::endpoint udpReceiver(asio::ip::address_v4::any(), 11001);
asio::ip::udp::endpoint udpSender(asio::ip::address_v4::loopback(), 11001);
asio::ip::udp::socket udpSocket(io_context, udpReceiver);

void buildContentModeMenu();	// TODO restructure main so we dont have to do forward declarations.

void createContentMode(EContentModeId contentModeId)
{
	// check if content mode already exists & block
	if (contentModes.find(contentModeId) != contentModes.end())
		return;

	std::shared_ptr<ContentModeBase> newContentMode;
	switch(contentModeId) 
	{
		case EContentModeId::Time:
			newContentMode = std::make_shared<ContentModeTime>(&serialConn);
			break;
		case EContentModeId::FinalFantasyXIV:
			newContentMode = std::make_shared<ContentModeFFXIV>(&serialConn);
			break;
		default:
			LOG_F(ERROR, "Could not create content mode");
			break;
	}

	if (currentContentMode != nullptr && serialConn.isOpen())
		currentContentMode->OnDeactivate();

	contentModes.insert({contentModeId, newContentMode});
	currentContentMode = newContentMode;

	if (serialConn.isOpen())
		currentContentMode->OnActivate();

	buildContentModeMenu();
	trayMaker.Update();
}

void killContentMode(EContentModeId contentModeId)
{
	// Destroy content mode.
	if (currentContentMode->contentModeId == contentModeId) 
	{
		if (serialConn.isOpen())
			currentContentMode->OnDeactivate();
		
		currentContentMode = contentModes[EContentModeId::Time];

		if (serialConn.isOpen())
			currentContentMode->OnActivate();
	}

	contentModes.erase(contentModeId);
	buildContentModeMenu();
	trayMaker.Update();
}

void onTick()
{
	long systemTimeMillis = GetSystemTimeMillis();

	ContentSwitchInformation switchInfo = contentSwitchInformation.load();
	if (switchInfo.wantsSwitch)
	{
		if (contentModes.find(switchInfo.newContentMode) != contentModes.end())
		{
			if (currentContentMode != nullptr && serialConn.isOpen())
				currentContentMode->OnDeactivate();

			currentContentMode = contentModes[switchInfo.newContentMode];

			if (serialConn.isOpen())
				currentContentMode->OnActivate();

			buildContentModeMenu();
			trayMaker.Update();
		}

		switchInfo.wantsSwitch = false;
		contentSwitchInformation = switchInfo;
	}

	// Handle incoming commands
	while (udpSocket.available())
	{
		//TODO handle for invalid data packets.
		char buffer[65536];
		asio::ip::udp::endpoint sender;
		std::size_t bytes_transferred = udpSocket.receive_from(asio::buffer(buffer), sender);
		std::string result(buffer, bytes_transferred);

		std::vector commandArray = SplitString(result, '|');
		ECommand command = commandMap[commandArray[0]];
		commandArray.erase(commandArray.begin());

		EContentModeId contentModeId = contentModeMap[commandArray[0]];

		switch (command)
		{
			//TODO: Create Goodbye command
			case ECommand::Hello:
			{
				createContentMode(contentModeId);
				break;
			}
			case ECommand::Keepalive:
			{
				if (contentModes.find(switchInfo.newContentMode) != contentModes.end())
					contentModes[contentModeId]->lastKeepaliveTimestamp = systemTimeMillis;
				else
					createContentMode(contentModeId);

				break;
			}
			case ECommand::SendData:
			{
				commandArray.erase(commandArray.begin());
				if (contentModes.find(switchInfo.newContentMode) != contentModes.end())
					contentModes[contentModeId]->OnDataReceived(commandArray);

				break;
			}
			case ECommand::Goodbye:
			{
				killContentMode(contentModeId);
				break;
			}
			default:
				LOG_F(ERROR, "unable to determine command");
				break;
		}
	}

	// Check if any keepalives expired
	EContentModeId toRemove = EContentModeId::Unknown;
	for (auto& pair : contentModes)
	{
		if(!pair.second->usesKeepalive)
			continue;

		if ((systemTimeMillis - pair.second->lastKeepaliveTimestamp) > 10000)
		{
			toRemove = pair.first;
			break;
		}
	}

	if(toRemove != EContentModeId::Unknown)
		killContentMode(toRemove);

	// Update the current content mode.
	long updateFrequency = 2000;
	if (currentContentMode != nullptr)
	{
		currentContentMode->OnTick();
		if(lastKnownDisplayText != currentContentMode->currentDisplayText)
		{
			serialConn.write(currentContentMode->currentDisplayText + "\n");
			lastKnownDisplayText = currentContentMode->currentDisplayText;
		}

		updateFrequency = currentContentMode->updateFrequency;
	}
	
	std::this_thread::sleep_for(std::chrono::milliseconds(updateFrequency));
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
	tickThread = new std::thread([](){ 
		loguru::set_thread_name("Update Thread");
		while(ticking) onTick(); 
	});
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
		new TrayMenu { "Show day/month at half minute", true, sav->timeSettings.showDate, false, [=](TrayMenu* tm){
			sav->timeSettings.showDate = !sav->timeSettings.showDate;
			
			tm->isChecked = sav->timeSettings.showDate;
			SaveHandler::GetInstance().SaveCurrentData();
			trayMaker.Update();
		}},
	}};
}

void buildContentModeMenu()
{
	for (TrayMenu* t : contentModeMenu.subMenu)
		delete t;

	contentModeMenu.subMenu.clear();

	for (auto& pair : contentModes)
	{
		EContentModeId contentModeId = pair.second->contentModeId; // redefine for copy
		contentModeMenu.subMenu.push_back(new TrayMenu { pair.second->contentModeName, true, pair.second == currentContentMode, false, [=](TrayMenu* tm){ 
			if(!serialConn.isOpen())
				return;

			ContentSwitchInformation toSet = { contentModeId, true };
			contentSwitchInformation = toSet;
		} });
	}
}

int main(int argc, char *argv[])
{
	loguru::init(argc, argv);
	loguru::add_file("PhalanxTray.log", loguru::Truncate, loguru::Verbosity_MAX);

	SaveHandler::GetInstance().CreateAndLoadSave();
	SaveData* sav = SaveHandler::GetInstance().GetCurrentSaveData();

	currentContentMode = std::make_shared<ContentModeTime>(&serialConn);
	contentModes.insert({EContentModeId::Time, currentContentMode});

	buildComPortMenu();
	buildModelMenu();
	buildSettingsMenu();
	buildContentModeMenu();

	tr.menu.push_back(&status);
	tr.menu.push_back(new TrayMenu { "-" });
	tr.menu.push_back(&comPorts);
	tr.menu.push_back(&model);
	tr.menu.push_back(&settingsMenu);
	tr.menu.push_back(&contentModeMenu);
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
		LOG_F(ERROR, "Failed tray initialization");

	return 0;
}