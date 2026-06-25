#include "posefixes_panel.hpp"
#include "dynamics.hpp"
#include "posefixes.hpp"

namespace UI {

	void RenderPoseFixes()
	{
		auto& settings = Settings::GetSettings();

		if (GUI::BeginTable("PoseFixSettings", 2)) {
			GUI::TableSetupColumn("Fix", GUI::ImGuiTableColumnFlags_WidthFixed, 180.0F);
			GUI::TableSetupColumn("Scale", GUI::ImGuiTableColumnFlags_WidthStretch);

			GUI::TableNextRow();
			GUI::TableNextColumn();
			const auto playerSittingChanged = GUI::Checkbox("Fix Player Sitting", &settings.fixPlayerSitting);
			Tooltip("Shrink the player collision capsule while sitting on supported furniture.");
			GUI::TableNextColumn();
			GUI::SetCursorPosX(GUI::GetCursorPosX() + 12.0F);
			GUI::BeginDisabled(!settings.fixPlayerSitting);
			GUI::SetNextItemWidth(260.0F);
			const auto playerSittingScaleChanged = GUI::SliderFloat("Player Sitting Scale", &settings.playerSittingScale, 0.3F, 1.0F);
			GUI::EndDisabled();

			GUI::TableNextRow();
			GUI::TableNextColumn();
			const auto npcSittingChanged = GUI::Checkbox("Fix NPC Sitting", &settings.fixNPCSitting);
			Tooltip("Shrink nearby NPC collision capsules while sitting on supported furniture.");
			GUI::TableNextColumn();
			GUI::SetCursorPosX(GUI::GetCursorPosX() + 12.0F);
			GUI::BeginDisabled(!settings.fixNPCSitting);
			GUI::SetNextItemWidth(260.0F);
			const auto npcSittingScaleChanged = GUI::SliderFloat("NPC Sitting Scale", &settings.npcSittingScale, 0.3F, 1.0F);
			GUI::EndDisabled();

			GUI::TableNextRow();
			GUI::TableNextColumn();
			const auto sneakingFixChanged = GUI::Checkbox("Fix Player Sneaking", &settings.fixPlayerSneaking);
			GUI::TableNextColumn();
			GUI::SetCursorPosX(GUI::GetCursorPosX() + 12.0F);
			GUI::BeginDisabled(!settings.fixPlayerSneaking);
			GUI::SetNextItemWidth(260.0F);
			const auto sneakingScaleChanged = GUI::SliderFloat("Player Sneaking Scale", &settings.playerSneakingScale, 0.3F, 1.0F);
			GUI::EndDisabled();

			GUI::TableNextRow();
			GUI::TableNextColumn();
			GUI::BeginDisabled(!settings.enableNPCDynamics);
			const auto npcSneakingChanged = GUI::Checkbox("Fix NPC Sneaking", &settings.fixNPCSneaking);
			GUI::EndDisabled();
			GUI::TableNextColumn();
			GUI::SetCursorPosX(GUI::GetCursorPosX() + 12.0F);
			GUI::BeginDisabled(!settings.enableNPCDynamics || !settings.fixNPCSneaking);
			GUI::SetNextItemWidth(260.0F);
			const auto npcSneakingScaleChanged = GUI::SliderFloat("NPC Sneaking Scale", &settings.npcSneakingScale, 0.3F, 1.0F);
			GUI::EndDisabled();

			if (auto* player = RE::PlayerCharacter::GetSingleton(); player && player->IsSneaking()) {
				auto& manager = VCD::Manager::GetSingleton();
				const auto poseFlags = PoseFixes::PlayerPose(player);
				if (sneakingFixChanged || sneakingScaleChanged) {
					manager.FixSneakingPose(player, poseFlags, false);
				}
			}
			if (playerSittingChanged || playerSittingScaleChanged) {
				auto* player = RE::PlayerCharacter::GetSingleton();
				const auto* actorState = player ? player->AsActorState() : nullptr;
				if (actorState && actorState->IsSitting()) {
					const auto poseFlags = PoseFixes::PlayerPose(player);
					const auto& state = Dynamics::GetPresetState();
					if (state.applied) {
						VCD::Manager::GetSingleton().SetPreset(player, state.current, poseFlags, false);
					}
					else {
						VCD::Manager::GetSingleton().FixSittingPose(player, poseFlags, false);
					}
				}
			}
			if (npcSittingChanged || npcSittingScaleChanged || npcSneakingChanged || npcSneakingScaleChanged) {
				Dynamics::GetNPCDynamicsState().actors.clear();
			}

			GUI::EndTable();
		}

		GUI::Spacing();

		if (CTAButton("Save Pose Fixes", Settings::IsPoseFixesDirty())) {
			Settings::SavePoseFixes();
		}
		Tooltip("Save Pose Fixes settings to disk.");

		GUI::SameLine();

		if (CTAButton("Default", !Settings::IsPoseFixesDefault())) {
			Settings::ResetPoseFixes();
			Dynamics::GetNPCDynamicsState().actors.clear();
			if (auto* player = RE::PlayerCharacter::GetSingleton()) {
				const auto poseFlags = PoseFixes::PlayerPose(player);
				auto& manager = VCD::Manager::GetSingleton();
				if (player->IsSneaking()) {
					manager.FixSneakingPose(player, poseFlags, false);
				}
				else if (poseFlags.isSitting) {
					const auto& state = Dynamics::GetPresetState();
					if (state.applied) {
						manager.SetPreset(player, state.current, poseFlags, false);
					}
					else {
						manager.FixSittingPose(player, poseFlags, false);
					}
				}
			}
		}
		Tooltip("Reset Pose Fixes settings to defaults without saving.");
	}

	void __stdcall RenderPoseFixesMenu()
	{
		RenderPoseFixes();
	}

}
