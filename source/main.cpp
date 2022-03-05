#include "tray.hpp"
#include "serial/serial.h"
#include "asio.hpp"
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
std::vector<std::shared_ptr<ContentModeBase>> allContentModes;	//TODO rework this to map, then we don't have to loop for getting the content mode, and only one of each can be active anyways.

asio::io_context io_context;
asio::ip::udp::endpoint udpReceiver(asio::ip::address_v4::any(), 11001);
asio::ip::udp::endpoint udpSender(asio::ip::address_v4::loopback(), 11001);
asio::ip::udp::socket udpSocket(io_context, udpReceiver);

void buildContentModeMenu();	// TODO restructure main so we dont have to do forward declarations.

void createContentMode(EContentModeId contentModeId)
{
	// check if content mode already exists & block
	for (std::shared_ptr<ContentModeBase> contentModePtr : allContentModes)
		if (contentModePtr->contentModeId == contentModeId)
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
			std::cout << "Could not create content mode for " << contentModeId << std::endl;
			break;
	}

	if (currentContentMode != nullptr && serialConn.isOpen())
		currentContentMode->OnDeactivate();

	allContentModes.push_back(newContentMode);
	currentContentMode = newContentMode;

	if (serialConn.isOpen())
		currentContentMode->OnActivate();

	buildContentModeMenu();
	trayMaker.Update();
}

void onTick()
{
	long systemTimeMillis = GetSystemTimeMillis();

	ContentSwitchInformation switchInfo = contentSwitchInformation.load();
	if (switchInfo.wantsSwitch)
	{
		for (std::shared_ptr<ContentModeBase> contentModePtr : allContentModes)
			if (switchInfo.newContentMode == contentModePtr->contentModeId)
			{
				if (currentContentMode != nullptr && serialConn.isOpen())
					currentContentMode->OnDeactivate();

				currentContentMode = contentModePtr;

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

		switch (command)
		{
			//TODO: Create Goodbye command
			case ECommand::Hello:
			{
				EContentModeId contentModeId = contentModeMap[commandArray[0]];
				createContentMode(contentModeId);
				break;
			}
			case ECommand::Keepalive:
			{
				EContentModeId contentModeId = contentModeMap[commandArray[0]];
				
				bool found = false;
				for (std::shared_ptr<ContentModeBase> contentModePtr : allContentModes)
					if (contentModePtr->contentModeId == contentModeId)
					{
						contentModePtr->lastKeepaliveTimestamp = systemTimeMillis;
						found = true;
						break;
					}

				if (!found)
					createContentMode(contentModeId);

				break;
			}
			case ECommand::SendData:
			{
				EContentModeId contentModeId = contentModeMap[commandArray[0]];
				commandArray.erase(commandArray.begin());

				for (std::shared_ptr<ContentModeBase> contentModePtr : allContentModes)
					if (contentModePtr->contentModeId == contentModeId)
					{
						contentModePtr->OnDataReceived(commandArray);
						break;
					}

				break;
			}
			default:
				std::cout << "unable to determine command" << std::endl;
				break;
		}
	}

	// Check if any keepalives expired
	if(!allContentModes.empty())
		for (int i = allContentModes.size()-1; i >= 0; i--)
		{
			std::shared_ptr<ContentModeBase> contentModePtr = allContentModes[i];
			if (!contentModePtr->usesKeepalive)
				continue;

			if ((systemTimeMillis - contentModePtr->lastKeepaliveTimestamp) > 10000)
			{
				// Destroy content mode.
				std::cout << "killing " << contentModePtr->contentModeName << std::endl;
				allContentModes.erase(allContentModes.begin()+i);
				if (contentModePtr == currentContentMode) 
				{
					if (serialConn.isOpen())
						currentContentMode->OnDeactivate();
					
					currentContentMode = allContentModes.front();

					if (serialConn.isOpen())
						currentContentMode->OnActivate();
				}

				buildContentModeMenu();
				trayMaker.Update();
			}
		}

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

void buildContentModeMenu()
{
	for (TrayMenu* t : contentModeMenu.subMenu)
		delete t;

	contentModeMenu.subMenu.clear();

	for (std::shared_ptr<ContentModeBase> contentModePtr : allContentModes)
	{
		EContentModeId contentModeId = contentModePtr->contentModeId; // redefine for copy
		contentModeMenu.subMenu.push_back(new TrayMenu { contentModePtr->contentModeName, true, contentModePtr == currentContentMode, false, [=](TrayMenu* tm){ 
			if(!serialConn.isOpen())
				return;

			ContentSwitchInformation toSet = { contentModeId, true };
			contentSwitchInformation = toSet;
		} });
	}
}

int main()
{
	SaveHandler::GetInstance().CreateAndLoadSave();
	SaveData* sav = SaveHandler::GetInstance().GetCurrentSaveData();

	currentContentMode = std::make_shared<ContentModeTime>(&serialConn);
	allContentModes.push_back(currentContentMode);

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
		std::cout << "initialization failed" << std::endl;

	return 0;
}