#ifndef CONTENTMODETIME_H
#define CONTENTMODETIME_H

#include "ContentModeBase.h"
#include <unordered_map>

namespace PhalanxTray
{
    class ContentModeTime : public ContentModeBase
    {
    private:
        std::unordered_map<EModel, std::vector<std::string>> timeOffsetMap =
        {
            { EModel::Phalanx , {" {:02}{:02} ", "{:02}  {:02}", "{:02}{:02}  ", "  {:02}{:02}"} },
            { EModel::Ameise, {"{:02}{:02}", " {:02}{:02}", " {:02}{:02}", "   {:02}{:02}", "    {:02}{:02}"} },
            { EModel::Noctiluca, {"{:02}{:02}"} },
        };

        std::unordered_map<EModel, std::vector<std::string>> timeOffsetMapSeconds =
        {
            { EModel::Phalanx , {"{:02}{:02}{:02}"} },
            { EModel::Ameise, {"{:02} {:02} {:02}", "{:02}{:02}{:02}  ", "  {:02}{:02}{:02}", "{:02}{:02}  {:02}"} },
            { EModel::Noctiluca, {"{:02}{:02}"} },
        };

        size_t currentOffset;
        uint8_t prevHr;

    public:
        ContentModeTime(serial::Serial* serialConnPtr);
        void OnActivate();
        void OnDeactivate();
        void OnTick();
    };
}

#endif //CONTENTMODETIME_H