
#include "plugin.hpp"
#include "logger.hpp"
#include "eventSinks.h"

static void MessageHandler(SKSE::MessagingInterface::Message* msg) {
    switch (msg->type) {
    case SKSE::MessagingInterface::kPostLoad:
    {
        break;
    }
    case SKSE::MessagingInterface::kSaveGame:
    {
        break;
    }
    case SKSE::MessagingInterface::kPreLoadGame:
    {
        break;
    }
    case SKSE::MessagingInterface::kPostLoadGame:
    {
        break;
    }
    case SKSE::MessagingInterface::kNewGame:
    {
   
        break;
    }
    case SKSE::MessagingInterface::kDataLoaded:
    {
     
        break;
    }
    default:
        break;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    setupLog(spdlog::level::info);
    logger::info("Variadic Collision Dynamics Plugin is Loaded");
    SKSE::GetMessagingInterface()->RegisterListener(MessageHandler);
    EventSinks::InstallEventSinks(); 
    return true;
}
