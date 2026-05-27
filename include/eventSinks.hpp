#pragma once

namespace EventSinks {

	// ideal for checking location is inteiror or exterior or checking for keywords, only runs on player
	struct PlayerCellEvent : RE::BSTEventSink<RE::BGSActorCellEvent>
	{
		static void RegisterEventSink();

		static PlayerCellEvent* GetSingleton()
		{
			static PlayerCellEvent singleton;
			return &singleton;
		}

	private:
		RE::BSEventNotifyControl ProcessEvent(
			const RE::BGSActorCellEvent* a_event,
			RE::BSTEventSource<RE::BGSActorCellEvent>*) override;
	};


	// ideal for combat events runs on all npcs must filter for player
	struct CombatEventSink : public RE::BSTEventSink<RE::TESCombatEvent>
	{
		CombatEventSink() = default;
		CombatEventSink(const CombatEventSink&) = delete;
		CombatEventSink(CombatEventSink&&) = delete;

		CombatEventSink& operator=(const CombatEventSink&) = delete;
		CombatEventSink& operator=(CombatEventSink&&) = delete;

		static void RegisterEventSink();

		static CombatEventSink* GetSingleton()
		{
			static CombatEventSink singleton;
			return &singleton;
		}

	private:
		RE::BSEventNotifyControl ProcessEvent(
			const RE::TESCombatEvent* event,
			RE::BSTEventSource<RE::TESCombatEvent>*) override;
	};


	void InstallEventSinks();


}