#include "game/pch.hpp"

#include <iostream>

#include "Game.hpp"
#include "engine/Design-Patterns/Singleton.hpp"

int main()
{
	Game::MainSingleton::Create();

	auto& main_instance = Game::MainSingleton::Instance();

	while (main_instance.IsRunning())
	{
		main_instance.MainLoop();
	}
}