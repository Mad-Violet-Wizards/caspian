#include "game/pch.hpp"

#include <iostream>

#include "Game.hpp"

int main()
{
	Game game;

	while (game.IsRunning())
	{
		game.GameLoop();
	}
}