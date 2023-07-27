#include "engine/pch.hpp"

#include "engine/Tools/Tools.hpp"
#include "game/Game.hpp"

using namespace Tools;

////////////////////////////////////////////////////////
/* Manager */
void Manager::Update(float _dt, EToolsSystem _tools_system)
{
	if (_tools_system == EToolsSystem::None) [[unlikely]]
		return;

	if (_tools_system == EToolsSystem::ImGui)
		m_imGuiManager.Update(_dt);
}

void Manager::Render(EToolsSystem _tools_system)
{
	if (_tools_system == EToolsSystem::None) [[unlikely]]
		return;

	if (_tools_system == EToolsSystem::ImGui)
		m_imGuiManager.Render();
}

void Manager::InitializeEventListeners()
{
	m_keyReleasedListener = std::make_unique<EventListener>();
	m_keyReleasedListener->NotifyOn(sf::Event::KeyReleased);

	m_keyReleasedListener->SetCallback([&](const sf::Event& event)
		{
			const sf::Keyboard keyboard;
			sf::Keyboard::Key key = keyboard.localize(event.key.scancode);

			if (key == sf::Keyboard::Key::Grave)
				m_imGuiManager.ToggleActiveState();
		});

	if (Game::MainSingleton::IsValid())
	{
		auto& main_instance = Game::MainSingleton::Instance();

		main_instance.GetEventDispatcher()->AddObserver(m_keyReleasedListener.get());
	}
}