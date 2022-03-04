#ifndef DEFINES_H
#define DEFINES_H

#include <vector>
#include <string>
#include <unordered_map>

namespace PhalanxTray
{
	static char saveFileName[] = "config.sav";

	enum EModel : uint8_t {
		Phalanx,	// 6 tubes, 8 segments
		Ameise,		// 8 tubes, 20 segments
		Noctiluca	// 4 tubes, 7 segments
	};

	enum ECommand : uint8_t {
		Hello,
		Keepalive,
		SendData,
	};

	static std::unordered_map<std::string, ECommand> commandMap = {
		{ std::string("HELLO"), ECommand::Hello },
		{ std::string("KEEPALIVE"), ECommand::Keepalive },
		{ std::string("SENDDATA"), ECommand::SendData }
	};

	static int GetAmountTubes(EModel model)
	{
		switch (model)
		{
			case EModel::Phalanx: return 6;
			case EModel::Ameise: return 8;
			case EModel::Noctiluca: return 4;
			default: return 0;
		}
	}

	static std::vector<std::string> SplitString(std::string x, char delimiter = '|')
	{
		x += delimiter; //includes a delimiter at the end so last word is also read
		std::vector<std::string> splitted;
		std::string temp = "";
		for (int i = 0; i < x.length(); i++)
		{
			if (x[i] == delimiter)
			{
				splitted.push_back(temp); //store words in "splitted" vector
				temp = "";
				i++;
			}
			temp += x[i];
		}
		return splitted;
	}
};

#endif // DEFINES_H