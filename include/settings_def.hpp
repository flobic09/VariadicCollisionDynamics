#pragma once

#include "preset.hpp"

#include <array>

#define FOREACH_BOOL_SETTING(S) \
	S(drawCollision, false)

#define FOREACH_PRESET_SETTING(S) \
	S(outdoor, VCD::Preset::kPersonalSpace) \
	S(indoor, VCD::Preset::kCompact) \
	S(combat, VCD::Preset::kBulky) \
	S(neutral, VCD::Preset::kVanillaLike)

#define BOOL2DEF(S, D) bool S = D;
#define PRESET2DEF(S, D) VCD::Preset S = D;

namespace Settings {

	struct VCDSettings
	{
		struct PresetOverride
		{
			bool edited{ false };
			VCD::CollisionData data{};
		};

		FOREACH_BOOL_SETTING(BOOL2DEF);
		FOREACH_PRESET_SETTING(PRESET2DEF);

		std::array<PresetOverride, 4> presets{};
	};

}
