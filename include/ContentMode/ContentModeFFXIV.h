#ifndef CONTENTMODEFFXIV_H
#define CONTENTMODEFFXIV_H

#include "ContentModeBase.h"

namespace PhalanxTray
{
    class ContentModeFFXIV : public ContentModeBase
    {
    public:
        ContentModeFFXIV(serial::Serial* serialConnPtr);
        void OnActivate();
        void OnDeactivate();
        void OnTick();
        virtual void OnDataReceived(std::vector<std::string>& data);
    };
}

#endif //CONTENTMODEFFXIV_H