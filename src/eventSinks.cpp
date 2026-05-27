#include "eventSinks.h"
#include "logger.hpp"

namespace EventSinks {

// normally bgsActorCelLEvent runs for every NPC but since we made the player the event source,
// this only runs for player character, ideal for changing collision base on interiors / or location type keywords
RE::BSEventNotifyControl PlayerCellEvent::ProcessEvent(const RE::BGSActorCellEvent* event,
	RE::BSTEventSource<RE::BGSActorCellEvent>*) {


	if (!event || event->flags == RE::BGSActorCellEvent::CellFlag::kLeave) {
		return RE::BSEventNotifyControl::kContinue;
	}

	auto player = RE::PlayerCharacter::GetSingleton();

	if (!player) return RE::BSEventNotifyControl::kContinue;

	auto cell = RE::TESForm::LookupByID<RE::TESObjectCELL>(event->cellID);
	if (!cell) {
		return RE::BSEventNotifyControl::kContinue;
	}

	// can change collision based on location keywords or check if player went from interior to exterior or vice versa
	/* 
	globals::currentCellIsInterior = cell->IsInteriorCell();

	if (globals::lastCellWasInterior != globals::currentCellIsInterior) {

	}
	globals::lastCellWasInterior = globals::currentCellIsInterior;*/ 

	return RE::BSEventNotifyControl::kContinue;
}

void PlayerCellEvent::RegisterEventSink()
{
	if (auto* player = RE::PlayerCharacter::GetSingleton()) {
		player->AsBGSActorCellEventSource()->AddEventSink(PlayerCellEvent::GetSingleton());
		logger::info("BGSActorCellEvent sink registered");
	}
}

// combat event runs on all npcs must filter for player
RE::BSEventNotifyControl CombatEventSink::ProcessEvent(const RE::TESCombatEvent* event,
	RE::BSTEventSource <RE::TESCombatEvent>*) {

	if (!event) {
		return RE::BSEventNotifyControl::kContinue;
	}

	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		return RE::BSEventNotifyControl::kContinue;
	}

	// filter for player only if wanted
	if (event->actor.get() != player) {
			return RE::BSEventNotifyControl::kContinue;
	}

	// change collision here

	return RE::BSEventNotifyControl::kContinue;
}

void CombatEventSink::RegisterEventSink() {
	auto* eventSink = CombatEventSink::GetSingleton();

	auto* eventSourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
	eventSourceHolder->AddEventSink<RE::TESCombatEvent>(eventSink);
}

void InstallEventSinks() {
	PlayerCellEvent::RegisterEventSink(); 
	CombatEventSink::RegisterEventSink(); 
}

}