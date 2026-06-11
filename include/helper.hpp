#pragma once

#include "plugin.hpp"

namespace VCD {

    inline float GetPresetScale()
    {
        return RE::bhkWorld::GetWorldScaleInverse();
    }

    inline RE::hkVector4 ToHkVector(const RE::NiPoint3& a_vec)
    {
        return RE::hkVector4(a_vec.x, a_vec.y, a_vec.z, 0.0f);
    }

    inline RE::NiPoint3 ToNiPoint3(const RE::hkVector4& a_vec)
    {
        return RE::NiPoint3(a_vec.quad.m128_f32[0], a_vec.quad.m128_f32[1], a_vec.quad.m128_f32[2]);
    }

}
