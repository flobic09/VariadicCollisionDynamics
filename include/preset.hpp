#pragma once

#include "plugin.hpp"

#include <string_view>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace VCD {

    inline constexpr std::string_view kCharacterBumperNodeName = "CharacterBumper";
    inline constexpr std::string_view kTemplateMeshDir = R"(meshes\Variadic Collision Tweaks\Templates)";

    enum class Preset
    {
        kVanillaLike,
        kPersonalSpace,
        kCompact,
        kBulky
    };

    struct PresetMesh
    {
        Preset preset{};
        std::string name{};
        std::string path{};

        RE::NiPointer<RE::NiNode> root{};
        RE::NiPointer<RE::bhkSPCollisionObject> spCollisionObject{};

        bool loaded{ false };
        bool foundCharacterBumper{ false };
        bool foundBhkSPCollisionObject{ false };
        bool foundCapsuleShape{ false };

        RE::BSResource::ErrorCode loadResult{};
    };

    
    template <class Enum>
    constexpr auto ToUnderlying(const Enum& a_value) 
    {
        return static_cast<std::underlying_type_t<Enum>>(a_value);
    }

    inline std::string MakeTemplatePath(const std::string& a_fileName)
    {
        std::string path{ kTemplateMeshDir };
        path += '\\';
        path += a_fileName;
        return path;
    }


}