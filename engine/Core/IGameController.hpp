#pragma once

class IGameController
{
public:

	virtual ~IGameController() = default;

	virtual void StartGame() = 0;
	virtual void ExitGame() = 0;
	virtual bool IsGameRunning() const = 0;

	virtual void Update(float _dt) = 0;

protected:

	bool m_IsGameRunning = false;
};