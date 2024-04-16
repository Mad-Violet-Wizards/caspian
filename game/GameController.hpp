#pragma once

#include "engine/Core/IGameController.hpp"

class GameController : public IGameController
{
public:
	void StartGame() override;
	void ExitGame() override;
	bool IsGameRunning() const override;

	void Update(float _dt) override;
};