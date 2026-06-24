#pragma once

#include "helper.hpp"
#include "settings.hpp"

namespace PoseFixes {

    inline bool IsSittingFurnitureName(const char* a_name)
    {
        if (!a_name || a_name[0] == '\0') {
            return false;
        }

        const std::string_view name(a_name);
        return VCD::ContainsInsensitive(name, "bench") ||
            VCD::ContainsInsensitive(name, "chair") ||
            VCD::ContainsInsensitive(name, "stool") ||
            VCD::ContainsInsensitive(name, "throne");
    }

    inline const RE::TESIdleForm* GetSittingIdle(const RE::Actor* a_actor)
    {
        if (!a_actor) {
            return nullptr;
        }

        const auto* highProcess = a_actor->GetHighProcess();
        if (highProcess && highProcess->currentProcessIdle) {
            return highProcess->currentProcessIdle;
        }

        const auto* process = a_actor->GetActorRuntimeData().currentProcess;
        const auto* middleHigh = process ? process->middleHigh : nullptr;
        return middleHigh ? middleHigh->furnitureIdle : nullptr;
    }

    inline bool IsChildSittingOnKnees(const RE::Actor* a_actor)
    {
        const auto* idle = GetSittingIdle(a_actor);
        if (!idle) {
            return false;
        }

        const auto* editorID = idle->GetFormEditorID();
        const auto* eventName = idle->animEventName.c_str();
        return (editorID && VCD::ContainsInsensitive(editorID, "ChildSitOnKnees")) ||
            (eventName && VCD::ContainsInsensitive(eventName, "ChildSitOnKnees"));
    }

    inline VCD::SittingFlags GetSittingFlags(const RE::Actor* a_actor)
    {
        const auto* actorState = a_actor ? a_actor->AsActorState() : nullptr;
        if (!actorState || !actorState->IsSitting()) {
            return {};
        }

        if (IsSittingFurnitureName(VCD::GetOccupiedFurnitureName(a_actor))) {
            return { true, false };
        }

        const auto isChildSittingOnKnees = IsChildSittingOnKnees(a_actor);
        return { isChildSittingOnKnees, isChildSittingOnKnees };
    }

    inline bool IsReallySitting(const RE::Actor* a_actor)
    {
        return GetSittingFlags(a_actor).isSitting;
    }

	inline VCD::SittingFlags PlayerSitting(const RE::Actor* a_actor)
	{
		const auto& settings = Settings::GetSettings();
		return settings.enablePoseFixes && settings.fixPlayerSitting ? GetSittingFlags(a_actor) : VCD::SittingFlags{};
	}

	inline VCD::SittingFlags NPCSitting(const RE::Actor* a_actor)
	{
		const auto& settings = Settings::GetSettings();
		return settings.enablePoseFixes && settings.fixNPCSitting ? GetSittingFlags(a_actor) : VCD::SittingFlags{};
	}

}
