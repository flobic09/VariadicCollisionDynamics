
#include "draw.hpp"
#include "hooks.hpp"
#include "logger.hpp"
#include "manager.hpp"
#include "dynamics.hpp"
#include "settings.hpp"
#include "raycast.hpp"
#include "posefixes.hpp"

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
	logger::info("Player update hook installed.");
}

bool SneakHandlerCanProcess::thunk(RE::SneakHandler* a_this, RE::InputEvent* a_event) {
	if (!a_this || !a_event) return func(a_this, a_event);

	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) return func(a_this, a_event);

	auto& manager = VCD::Manager::GetSingleton();
	const auto sittingFlags = PoseFixes::PlayerSitting(player);

	if (player->IsSneaking()) {
		const auto canProcess = func(a_this, a_event);
		if (canProcess) {
			manager.FixSneakingPose(player, true, sittingFlags, false);
		}
		return canProcess;
	}

	auto standingHeight = manager.GetStandingCapsuleHeight(player);
	if (standingHeight <= 0.0F) {
		standingHeight = player->GetHeight();
	}

	if (!raycast::canStandUp(standingHeight)) {
		return false;
	}

	const auto canProcess = func(a_this, a_event);
	if (canProcess) {
		manager.FixSneakingPose(player, false, sittingFlags, false);
	}
	return canProcess;
}
void SneakHandlerCanProcess::Install()
{
	func = REL::Relocation<std::uintptr_t>(RE::SneakHandler::VTABLE[0]).write_vfunc(0x01, thunk);
	logger::info("Can Process Sneak hook installed");
}
