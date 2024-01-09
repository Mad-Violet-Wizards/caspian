#pragma once

#include "engine/Scenes/StateMachine.hpp"

class GameModule
{
	public:

		GameModule() = default;
		~GameModule() = default;

	private:

		Scenes::StateMachine m_SceneStateMachine;

};