#include "game/pch.hpp"

#include <iostream>

#include "Game.hpp"
#include "engine/Design-Patterns/Singleton.hpp"

int main()
{
	Game::MainSingleton::Create();

	auto& main_instance = Game::MainSingleton::Instance();

	std::unique_ptr<Events::Dispatcher> event_dispatcher_system = std::make_unique<Events::Dispatcher>();
	main_instance.SetEventDispatcher(std::move(event_dispatcher_system));

	#if defined(DEBUG)
	std::unique_ptr<Game::DebugHelper> game_debug_helper = std::make_unique<Game::DebugHelper>();
	game_debug_helper->InitializeDebugEventListeners();

	std::unique_ptr<Tools::Manager> tools_manager = std::make_unique<Tools::Manager>();
	tools_manager->InitializeEventListeners();
	main_instance.SetToolsManager(std::move(tools_manager));
	#endif

	while (main_instance.IsRunning())
	{
		main_instance.MainLoop();
	}
}