#pragma once

#include "plugin.hpp"

#include <array>
#include <cstddef>
#include <optional>
#include <string_view>

#define FOREACH_SUPPORTED_NPC_PRESET_RACE(S) \
	S(Giant, "GiantRace", "Giant") \
	S(C00GiantOutsideWhiterun, "C00GiantOutsideWhiterunRace", "Giant") \
	S(Troll, "TrollRace", "Troll") \
	S(TrollFrost, "TrollFrostRace", "Troll") \
	S(Draugr, "DraugrRace", "Draugr") \
	S(DraugrMagic, "DraugrMagicRace", "Draugr") \
	S(Dremora, "DremoraRace", "Dremora") \
	S(Falmer, "FalmerRace", "Falmer") \
	S(Hagraven, "HagravenRace", "Hagraven") \
	S(Spriggan, "SprigganRace", "Spriggan") \
	S(SprigganMatron, "SprigganMatronRace", "Spriggan") \
	S(DragonPriest, "DragonPriestRace", "DragonPriest") \
	S(Skeleton, "skeletonRace", "Skeleton") \
	S(RigidSkeleton, "RigidSkeletonRace", "Skeleton") \
	S(SkeletonNecro, "SkeletonNecroRace", "Skeleton") \
	S(SkeletonNecroPriest, "SkeletonNecroPriestRace", "Skeleton") \
	S(AtronachFlame, "AtronachFlameRace", "FlameAtronach") \
	S(AtronachFrost, "AtronachFrostRace", "FrostAtronach") \
	S(AtronachStorm, "AtronachStormRace", "StormAtronach")

namespace VCD::Race {

	enum class SupportedNPCPresetRace
	{
#define SUPPORTED_NPC_PRESET_RACE_ENUM(S, E, P) k##S,
		FOREACH_SUPPORTED_NPC_PRESET_RACE(SUPPORTED_NPC_PRESET_RACE_ENUM)
		kTotal
	};

	struct SupportedNPCPresetRaceInfo
	{
		SupportedNPCPresetRace race{};
		std::string_view editorID{};
		std::string_view presetKey{};
	};

#define SUPPORTED_NPC_PRESET_RACE_INFO(S, E, P) SupportedNPCPresetRaceInfo{ SupportedNPCPresetRace::k##S, E, P },
	inline constexpr std::array<SupportedNPCPresetRaceInfo, static_cast<size_t>(SupportedNPCPresetRace::kTotal)> kSupportedNPCPresetRaces
	{
		FOREACH_SUPPORTED_NPC_PRESET_RACE(SUPPORTED_NPC_PRESET_RACE_INFO)
	};

	inline std::optional<SupportedNPCPresetRace> SupportedNPCPresetRaceFromEditorID(std::string_view a_editorID)
	{
		for (const auto& raceInfo : kSupportedNPCPresetRaces) {
			if (raceInfo.editorID == a_editorID) {
				return raceInfo.race;
			}
		}

		return std::nullopt;
	}

	inline std::string_view SupportedNPCPresetKey(const SupportedNPCPresetRace& a_race)
	{
		const auto index = static_cast<size_t>(a_race);
		return index < kSupportedNPCPresetRaces.size() ? kSupportedNPCPresetRaces[index].presetKey : std::string_view{};
	}

	inline std::optional<SupportedNPCPresetRace> GetSupportedNPCPresetRace(const RE::Actor* a_actor)
	{
		if (!a_actor) {
			return std::nullopt;
		}

		const auto* race = a_actor->GetRace();
		if (!race) {
			return std::nullopt;
		}

		const auto* editorID = race->GetFormEditorID();
		return editorID ? SupportedNPCPresetRaceFromEditorID(editorID) : std::nullopt;
	}

	inline std::string_view GetSupportedNPCPresetKey(const RE::Actor* a_actor)
	{
		const auto race = GetSupportedNPCPresetRace(a_actor);
		return race ? SupportedNPCPresetKey(*race) : std::string_view{};
	}

	inline bool IsSupportedNPCPresetActor(RE::Actor* a_actor)
	{
		return a_actor && (a_actor->IsGuard() || a_actor->HasKeywordString("ActorTypeNPC") || GetSupportedNPCPresetRace(a_actor).has_value());
	}

}

#undef SUPPORTED_NPC_PRESET_RACE_ENUM
#undef SUPPORTED_NPC_PRESET_RACE_INFO
#undef FOREACH_SUPPORTED_NPC_PRESET_RACE
