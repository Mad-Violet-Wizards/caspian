#include "engine/pch.hpp"

#include "Tools.hpp"

using namespace Tools;

////////////////////////////////////////////////////////
/* Manager */
void Manager::Update(float _dt, EToolsSystem _tools_system)
{
	if (ApplicationSingleton::Instance().GetEngineController().GetKeyboardInputController()->IsKeyDown(ESupportedKey::Grave))
		m_ImplManager.ToggleActiveState();

	if (_tools_system == EToolsSystem::None) [[unlikely]]
		return;

	if (_tools_system == EToolsSystem::ImGui)
		m_ImplManager.Update(_dt);
}

void Manager::Render(EToolsSystem _tools_system)
{
	if (_tools_system == EToolsSystem::None) [[unlikely]]
		return;

	if (_tools_system == EToolsSystem::ImGui)
		m_ImplManager.Render();
}

void Manager::ShowNotification(Tools_Impl::ENotificationType _type, std::string_view _msg)
{
	m_ImplManager.ShowNotification(_type, _msg);
}
