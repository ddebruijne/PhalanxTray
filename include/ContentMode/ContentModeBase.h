#ifndef CONTENTMODEBASE_H
#define CONTENTMODEBASE_H

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

        ContentModeBase(serial::Serial* serialConnPtr) { serialConn = serialConnPtr; }
        virtual void OnActivate() = 0;
        virtual void OnDeactivate() = 0;
        virtual void OnTick() = 0;
    };
}

#endif //CONTENTMODEBASE_H