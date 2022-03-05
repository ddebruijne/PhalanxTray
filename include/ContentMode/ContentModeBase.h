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
        const EContentModeId contentModeId;
        const std::string contentModeName;
        const bool usesKeepalive;

        long updateFrequency; // in ms
        long lastKeepaliveTimestamp;
        std::string currentDisplayText;

    protected:
        ContentModeBase(EContentModeId contentModeId, std::string contentModeName, bool usesKeepAlive, serial::Serial* serialConnPtr);

    public:
        virtual void OnActivate() = 0;
        virtual void OnDeactivate() = 0;
        virtual void OnTick() = 0;
        virtual void OnDataReceived(std::vector<std::string>& data) {};
    };
}

#endif //CONTENTMODEBASE_H