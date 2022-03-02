#ifndef DEFINES_H
#define DEFINES_H

namespace PhalanxTray
{
	static char saveFileName[] = "config.sav";

	enum EModel : int {
		Phalanx,	// 6 tubes, 8 segments
		Ameise,		// 8 tubes, 20 segments
		Noctiluca	// 4 tubes, 7 segments
	};

	static int GetAmountTubes(EModel model)
	{
		switch (model)
		{
			case EModel::Phalanx: return 6;
			case EModel::Ameise: return 8;
			case EModel::Noctiluca: return 4;
		}
	}
};

#endif // DEFINES_H