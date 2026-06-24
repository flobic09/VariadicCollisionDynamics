
#include "draw.hpp"
#include "hooks.hpp"
#include "logger.hpp"
#include "manager.hpp"
#include "dynamics.hpp"
#include "settings.hpp"
#include "raycast.hpp"
#include "preset.hpp"

#include <CLibUtilsQTR/DrawDebug.hpp>

using namespace Hook; 
using namespace DebugAPI_IMPL;

void PlayerUpdate::thunk(RE::PlayerCharacter* player, float delta) {

	func(player, delta);

	if (!player) {
		logger::warn("Player is null, cannot update.");
		return;
	}

	// block if player is sneaking. Handled in SneakHandlerCanProcess hook
	if (!player->IsSneaking()) {
		Dynamics::Update(player);
	}

	Dynamics::UpdateNPCs(player);

	const auto& settings = Settings::GetSettings();
	if (settings.drawCollision || settings.drawNearbyActors) {
		DebugAPI::GetSingleton()->LinesToDraw.clear();
	}

	if (settings.drawCollision) {
		Draw::DrawPlayerBumper();
	}

	if (settings.drawNearbyActors) {
		Draw::DrawNearbyActorBumpers();
	}

	if (settings.drawCollision || settings.drawNearbyActors) {
		DebugAPI::GetSingleton()->Update();
	}
}

void PlayerUpdate::Install()
{
	func = REL::Relocation<std::uintptr_t>(RE::PlayerCharacter::VTABLE[0]).write_vfunc(0xAD, thunk);
	logger::info("Player update hook installed");
}

bool SneakHandlerCanProcess::thunk(RE::SneakHandler* a_this, RE::InputEvent* a_event) {
	if (!a_this || !a_event) return func(a_this, a_event);

	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) return func(a_this, a_event);

	auto& manager = VCD::Manager::GetSingleton();
	auto current = Dynamics::GetPresetState().current;
	const auto* config = manager.GetPresetConfig(current);
	const auto* defaultConfig = manager.GetDefaultPresetConfig(current);

	if (!config) return func(a_this, a_event);

	if (player->IsSneaking()) {
		const auto defaultTop = defaultConfig ? defaultConfig->data.capsule.point1.z : config->data.capsule.point1.z;
		const auto targetTop = defaultTop - 0.5F;

		if (config->data.capsule.point1.z != targetTop) {
			auto data = config->data;
			data.capsule.point1.z = targetTop;
			data.RecalculateHeight();
			manager.SetCollisionData(player, data, current, "sneak_crouch", false);
		}
		return func(a_this, a_event);
	}
	else {
		// Player is trying to unsneak
		if (!raycast::canStandUp()) {
			return false; // block it
		}
		// Can stand, restore last active preset
		manager.SetCollisionData(player, config->data, current, "sneak_restore", false);
		return func(a_this, a_event);
	}

	return func(a_this, a_event);
}
void SneakHandlerCanProcess::Install()
{
	func = REL::Relocation<std::uintptr_t>(RE::SneakHandler::VTABLE[0]).write_vfunc(0x01, thunk);
	logger::info("Can Process Sneak hook installed");
}
