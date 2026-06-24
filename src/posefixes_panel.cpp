#include "posefixes_panel.hpp"

namespace UI {

	void RenderPoseFixes()
	{
		auto& settings = Settings::GetSettings();

		GUI::Checkbox("Enable Pose Fixes", &settings.enablePoseFixes);
		Tooltip("Enable collision fix for actor poses that vanilla collision does not handle correctly.");

		GUI::Spacing();

		GUI::Checkbox("Fix Player Sitting", &settings.fixPlayerSitting);
		Tooltip("Shrink the player collision capsule while sitting on supported furniture.");

		GUI::Checkbox("Fix NPC Sitting", &settings.fixNPCSitting);
		Tooltip("Shrink nearby NPC collision capsules while sitting on supported furniture.");

		GUI::Spacing();

		if (CTAButton("Save Pose Fixes", Settings::IsPoseFixesDirty())) {
			Settings::SavePoseFixes();
		}
		Tooltip("Save Pose Fixes settings to disk.");

		GUI::SameLine();

		if (CTAButton("Default", !Settings::IsPoseFixesDefault())) {
			Settings::ResetPoseFixes();
		}
		Tooltip("Reset Pose Fixes settings to defaults without saving.");
	}

	void __stdcall RenderPoseFixesMenu()
	{
		RenderPoseFixes();
	}

}
