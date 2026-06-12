#include "settings.hpp"

#include "dynamics.hpp"
#include "logger.hpp"
#include "manager.hpp"

#include "nlohmann/json.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <cstddef>

namespace Settings {

	namespace fs = std::filesystem;
	using json = nlohmann::json;

	namespace {
		bool CollisionDataEqual(const VCD::CollisionData& a_left, const VCD::CollisionData& a_right)
		{
			return a_left.bump.translation.x == a_right.bump.translation.x &&
				a_left.bump.translation.y == a_right.bump.translation.y &&
				a_left.bump.translation.z == a_right.bump.translation.z &&
				a_left.bump.scale == a_right.bump.scale &&
				a_left.capsule.radius == a_right.capsule.radius &&
				a_left.capsule.height == a_right.capsule.height &&
				a_left.capsule.point1.x == a_right.capsule.point1.x &&
				a_left.capsule.point1.y == a_right.capsule.point1.y &&
				a_left.capsule.point1.z == a_right.capsule.point1.z &&
				a_left.capsule.point2.x == a_right.capsule.point2.x &&
				a_left.capsule.point2.y == a_right.capsule.point2.y &&
				a_left.capsule.point2.z == a_right.capsule.point2.z;
		}

		bool PresetOverridesEqual(const VCDSettings::PresetOverride& a_left, const VCDSettings::PresetOverride& a_right)
		{
			return a_left.edited == a_right.edited && CollisionDataEqual(a_left.data, a_right.data);
		}

		json PointToJson(const VCD::Vec3& a_point)
		{
			return json::array({ a_point.x, a_point.y, a_point.z });
		}

		json PointToJson(const RE::NiPoint3& a_point)
		{
			return json::array({ a_point.x, a_point.y, a_point.z });
		}

		bool ReadVec3(VCD::Vec3& a_out, const json& a_json)
		{
			if (!a_json.is_array()) {
				return false;
			}

			if (a_json.size() > 0) {
				a_out.x = a_json[0].get<float>();
			}

			if (a_json.size() > 1) {
				a_out.y = a_json[1].get<float>();
			}

			if (a_json.size() > 2) {
				a_out.z = a_json[2].get<float>();
			}

			return true;
		}

		bool ReadNiPoint3(RE::NiPoint3& a_out, const json& a_json)
		{
			if (!a_json.is_array()) {
				return false;
			}

			if (a_json.size() > 0) {
				a_out.x = a_json[0].get<float>();
			}

			if (a_json.size() > 1) {
				a_out.y = a_json[1].get<float>();
			}

			if (a_json.size() > 2) {
				a_out.z = a_json[2].get<float>();
			}

			return true;
		}

		json CollisionDataToJson(const VCD::CollisionData& a_data)
		{
			return {
				{ "position", PointToJson(a_data.bump.translation) },
				{ "scale", a_data.bump.scale },
				{ "radius", a_data.capsule.radius },
				{ "height", a_data.capsule.height },
				{ "firstPoint", PointToJson(a_data.capsule.point1) },
				{ "secondPoint", PointToJson(a_data.capsule.point2) }
			};
		}

		void CollisionDataFromJson(const json& a_json, VCD::CollisionData& a_data)
		{
			if (a_json.contains("position")) {
				ReadNiPoint3(a_data.bump.translation, a_json.at("position"));
			}

			if (a_json.contains("scale")) {
				a_data.bump.scale = a_json.at("scale").get<float>();
			}

			if (a_json.contains("radius")) {
				a_data.capsule.radius = a_json.at("radius").get<float>();
			}

			if (a_json.contains("firstPoint")) {
				ReadVec3(a_data.capsule.point1, a_json.at("firstPoint"));
			}

			if (a_json.contains("secondPoint")) {
				ReadVec3(a_data.capsule.point2, a_json.at("secondPoint"));
			}

			a_data.RecalculateHeight();
		}

		json PresetOverridesToJson(const VCDSettings& a_settings)
		{
			json presets = json::object();

			for (std::size_t i = 0; i < a_settings.presets.size(); ++i) {
				const auto preset = static_cast<VCD::Preset>(i);
				const auto& presetOverride = a_settings.presets[i];
				auto entry = json{
					{ "edited", presetOverride.edited }
				};

				if (presetOverride.edited) {
					entry.update(CollisionDataToJson(presetOverride.data));
				}

				presets[PresetToString(preset)] = entry;
			}

			return presets;
		}

		void PresetOverridesFromJson(const json& a_json, VCDSettings& a_settings)
		{
			if (!a_json.contains("presets") || !a_json.at("presets").is_object()) {
				return;
			}

			const auto& presets = a_json.at("presets");
			for (std::size_t i = 0; i < a_settings.presets.size(); ++i) {
				const auto preset = static_cast<VCD::Preset>(i);
				const auto name = PresetToString(preset);
				if (!presets.contains(name)) {
					continue;
				}

				const auto& entry = presets.at(name);
				auto& presetOverride = a_settings.presets[i];

				if (entry.contains("edited")) {
					presetOverride.edited = entry.at("edited").get<bool>();
				}

				if (presetOverride.edited) {
					CollisionDataFromJson(entry, presetOverride.data);
				}
			}
		}

		json ToJson(const VCDSettings& a_settings)
		{
#define BOOL2JSON(S, D) { #S, a_settings.S },
#define PRESET2JSON(S, D) { #S, PresetToString(a_settings.S) },

			auto data = json{
				FOREACH_BOOL_SETTING(BOOL2JSON)
				FOREACH_PRESET_SETTING(PRESET2JSON)
			};

			data["presets"] = PresetOverridesToJson(a_settings);
			return data;
		}

		void FromJson(const json& a_json, VCDSettings& a_settings)
		{
#define BOOL2GETTER(S, D) getBool(a_json, #S, a_settings.S);
#define PRESET2GETTER(S, D) getPreset(a_json, #S, a_settings.S);

			FOREACH_BOOL_SETTING(BOOL2GETTER)
			FOREACH_PRESET_SETTING(PRESET2GETTER)
			PresetOverridesFromJson(a_json, a_settings);
		}
	}

	bool SettingsEqual(const VCDSettings& a_left, const VCDSettings& a_right)
	{
#define SETTING2EQ(S, D) a_left.S == a_right.S &&

		if (!(FOREACH_BOOL_SETTING(SETTING2EQ) FOREACH_PRESET_SETTING(SETTING2EQ) true)) {
			return false;
		}

		for (std::size_t i = 0; i < a_left.presets.size(); ++i) {
			if (!PresetOverridesEqual(a_left.presets[i], a_right.presets[i])) {
				return false;
			}
		}

		return true;
	}

	void CaptureCurrent(VCDSettings& a_settings)
	{
		auto& config = Dynamics::GetConfig();
		a_settings.outdoor = config.outdoor;
		a_settings.indoor = config.indoor;
		a_settings.combat = config.combat;
		a_settings.neutral = config.neutral;
	}

	void ApplySettings(const VCDSettings& a_settings)
	{
		auto& config = Dynamics::GetConfig();
		auto& manager = VCD::Manager::GetSingleton();
		config.outdoor = a_settings.outdoor;
		config.indoor = a_settings.indoor;
		config.combat = a_settings.combat;
		config.neutral = a_settings.neutral;

		for (std::size_t i = 0; i < a_settings.presets.size(); ++i) {
			const auto preset = static_cast<VCD::Preset>(i);
			if (!a_settings.presets[i].edited) {
				manager.RestorePresetDefault(preset);
				continue;
			}

			if (auto* presetConfig = manager.GetPresetConfig(preset)) {
				presetConfig->data = a_settings.presets[i].data;
				presetConfig->data.RecalculateHeight();
			}
		}
	}

	void MarkPresetEdited(const VCD::Preset& a_preset, const VCD::CollisionData& a_data)
	{
		const auto index = static_cast<std::size_t>(VCD::ToUnderlying(a_preset));
		auto& settings = GetSettings();
		if (index >= settings.presets.size()) {
			return;
		}

		settings.presets[index].edited = true;
		settings.presets[index].data = a_data;
		settings.presets[index].data.RecalculateHeight();
	}

	void ClearPresetEdited(const VCD::Preset& a_preset)
	{
		const auto index = static_cast<std::size_t>(VCD::ToUnderlying(a_preset));
		auto& settings = GetSettings();
		if (index >= settings.presets.size()) {
			return;
		}

		settings.presets[index] = {};
		VCD::Manager::GetSingleton().RestorePresetDefault(a_preset);
	}

	bool IsDirty()
	{
		CaptureCurrent(GetSettings());
		return !SettingsEqual(GetSettings(), GetSavedSettings());
	}

	bool Load()
	{
		auto settings = VCDSettings{};
		const auto path = GetSettingsPath();
		std::error_code ec;

		if (!fs::exists(path, ec)) {
			GetSettings() = settings;
			GetSavedSettings() = settings;
			ApplySettings(settings);
			logger::info("Settings file does not exist, using defaults: {}", path);
			return true;
		}

		std::ifstream file(path);
		if (!file.is_open()) {
			logger::error("Failed to open settings file: {}", path);
			return false;
		}

		std::string raw((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		try {
			const auto data = json::parse(raw, nullptr, true, true);
			FromJson(data, settings);
		}
		catch (const json::exception& e) {
			logger::error("Failed to parse settings file {}: {}", path, e.what());
			return false;
		}

		GetSettings() = settings;
		GetSavedSettings() = settings;
		ApplySettings(settings);
		logger::info("Settings loaded: {}", path);
		return true;
	}

	bool Save()
	{
		CaptureCurrent(GetSettings());

		const auto path = fs::path(GetSettingsPath());
		std::error_code ec;
		fs::create_directories(path.parent_path(), ec);

		if (ec) {
			logger::error("Failed to create settings directory {}: {}", path.parent_path().string(), ec.message());
			return false;
		}

		std::ofstream file(path);
		if (!file.is_open()) {
			logger::error("Failed to open settings file for writing: {}", path.string());
			return false;
		}

		file << ToJson(GetSettings()).dump(4);
		GetSavedSettings() = GetSettings();
		logger::info("Settings saved: {}", path.string());
		return true;
	}

}
