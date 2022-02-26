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
};

#endif // DEFINES_H