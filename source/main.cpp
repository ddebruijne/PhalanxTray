#include "tray.hpp"
#include "serial/serial.h"
#include <iostream>

using namespace Tray;

std::vector<TrayMenu*> menu;
TrayIcon tr = { "icon.png", "icon.ico", "Phalanx Tray App", menu };
TrayMenu comPorts = { "Serial Port", true, false, false, nullptr };

std::function changeModel = [](TrayMenu* tm){ };

int main()
{
	std::vector<serial::PortInfo> devices_found = serial::list_ports();
	for (serial::PortInfo device : devices_found) 
	{
		comPorts.subMenu.push_back(new TrayMenu { device.port, true, false, false, [&](TrayMenu* tm){ 
			for (TrayMenu* t : comPorts.subMenu)
			{
				t->isChecked = false;
			}

			tm->isChecked = true;

			trayMaker.Update();
		} });
	}

	TrayMenu model = { "Model", true, false, false, nullptr, { 
		new TrayMenu { "Phalanx", true, false, false, changeModel},
		new TrayMenu { "Ameise", true, false, false, changeModel},
		new TrayMenu { "Noctiluca", true, false, false, changeModel},
	}};

	tr.menu.push_back(new TrayMenu { "Phalanx Tray App", false });
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