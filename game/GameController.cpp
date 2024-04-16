#include "game/pch.hpp"

void GameController::StartGame()
{
	m_IsGameRunning = true;

#ifdef _DEBUG
	ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->ConstructDebugPlayer();

	ApplicationSingleton::Instance().GetEngineController().GetToolsManager()->ShowNotification(Tools_Impl::ENotificationType::Info, "Game started.");
#endif
}

void GameController::ExitGame()
{
	m_IsGameRunning = false;

#ifdef _DEBUG
	ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->ReleaseDebugPlayer();

	ApplicationSingleton::Instance().GetEngineController().GetToolsManager()->ShowNotification(Tools_Impl::ENotificationType::Info, "Game stopped.");
#endif
}

bool GameController::IsGameRunning() const
{
		return m_IsGameRunning;
}

void GameController::Update(float _dt)
{

}

