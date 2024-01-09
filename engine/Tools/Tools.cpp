#include "engine/pch.hpp"

#include "Tools.hpp"

using namespace Tools;

////////////////////////////////////////////////////////
/* Manager */
void Manager::Update(float _dt, EToolsSystem _tools_system)
{
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

void Manager::InitializeEventListeners()
{
	m_keyReleasedListener = std::make_unique<Events::Listener>();
	m_keyReleasedListener->NotifyOn(sf::Event::KeyReleased);

	m_keyReleasedListener->SetCallback([&](const sf::Event& event)
		{
			const sf::Keyboard keyboard;
			sf::Keyboard::Key key = keyboard.localize(event.key.scancode);

			if (key == sf::Keyboard::Key::Grave)
				m_ImplManager.ToggleActiveState();
		});

	if (ApplicationSingleton::IsValid())
	{
		auto& main_instance = ApplicationSingleton::Instance();

		main_instance.GetEngineModule().GetEventDispatcher()->AddObserver(m_keyReleasedListener.get());
	}
}

void Manager::ShowNotification(Tools_Impl::ENotificationType _type, std::string_view _msg)
{
	m_ImplManager.ShowNotification(_type, _msg);
}
