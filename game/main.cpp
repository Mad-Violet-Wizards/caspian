#include "game/pch.hpp"

#include <iostream>

#include "Game.hpp"
#include "engine/Design-Patterns/Singleton.hpp"

int main()
{
	Game::MainSingleton::Create();

	auto& main_instance = Game::MainSingleton::Instance();

	#if defined(DEBUG)
	std::unique_ptr<Game::DebugHelper> game_debug_helper = std::make_unique<Game::DebugHelper>();
	game_debug_helper->InitializeDebugEventListeners();
	#endif

	while (main_instance.IsRunning())
	{
		main_instance.MainLoop();
	}
}