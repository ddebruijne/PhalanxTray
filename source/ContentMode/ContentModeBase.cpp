#include "ContentMode/ContentModeBase.h"

using namespace PhalanxTray;

ContentModeBase::ContentModeBase(EContentModeId contentModeId, std::string contentModeName, bool usesKeepAlive, serial::Serial* serialConnPtr) :
    contentModeId(contentModeId),
    contentModeName(contentModeName),
    usesKeepalive(usesKeepAlive),
    serialConn(serialConnPtr),
    updateFrequency(1000),
    lastKeepaliveTimestamp(GetSystemTimeMillis())
{
}