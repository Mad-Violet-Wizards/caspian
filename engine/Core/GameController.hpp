#pragma once

#include "engine/Scenes/StateMachine.hpp"

class GameController
{
	public:

		GameController() = default;
		~GameController() = default;

	private:

		Scenes::StateMachine m_SceneStateMachine;

};