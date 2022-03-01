#include "../defines.h"
#include "serial/serial.h"

namespace PhalanxTray 
{
    class ContentModeBase
    {
    protected:
        serial::Serial* serialConn;

    public:
        const std::string contentModeName = "Unknown";
        long updateFrequency = 1000; // in ms

        ContentModeBase(serial::Serial* inRef) { serialConn = inRef; }
        virtual void OnActivate() = 0;
        virtual void OnDeactivate() = 0;
        virtual void OnTick() = 0;
    };
}