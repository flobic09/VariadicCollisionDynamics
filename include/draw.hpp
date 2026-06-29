#pragma once

#include "settings.hpp"
#include "logger.hpp"

namespace DebugAPI_IMPL::Draw {

    inline constexpr int32_t CAPSULE_SIDES = 16;

    struct NearbyActorDrawState
    {
        std::vector<RE::ActorHandle> handles{};
        std::chrono::steady_clock::time_point nextScan{};
        int scannedCount{ 0 };
        int scannedHighCount{ 0 };
        int scannedMiddleHighCount{ 0 };
        int scannedMiddleLowCount{ 0 };
        int acceptedCount{ 0 };
        int rejectedCount{ 0 };
        bool limitReached{ false };
    };

    inline NearbyActorDrawState& GetNearbyActorDrawState()
    {
        static NearbyActorDrawState state{};
        return state;
    }

    inline void LogNearbyActorScan(const NearbyActorDrawState& a_state, const Settings::VCDSettings& a_settings)
    {
        logger::debug("Nearby actor scan: scanned={} (high={}, middleHigh={}, middleLow={}), accepted={}, rejected={}, radius={}, limit={}, limitReached={}",
            a_state.scannedCount,
            a_state.scannedHighCount,
            a_state.scannedMiddleHighCount,
            a_state.scannedMiddleLowCount,
            a_state.acceptedCount,
            a_state.rejectedCount,
            a_settings.nearbyActorScanRadius,
            a_settings.nearbyActorScanLimit,
            a_state.limitReached);
    }

    void DrawPlayerBumper();

    void DrawNearbyActorBumpers();

    void DrawSphere(RE::bhkSimpleShapePhantom* a_phantom, const RE::NiColorA& a_color, float a_lineThickness);

    void DrawCameraBumper();

}
