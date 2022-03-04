#ifndef CONTENTMODETIME_H
#define CONTENTMODETIME_H

#include "ContentModeBase.h"

namespace PhalanxTray
{
    class ContentModeTime : public ContentModeBase
    {
    public:
        ContentModeTime(serial::Serial* serialConnPtr);
        void OnActivate();
        void OnDeactivate();
        void OnTick();
    };
}

#endif //CONTENTMODETIME_H