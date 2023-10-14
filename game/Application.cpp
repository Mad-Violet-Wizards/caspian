#include "game/pch.hpp"

#include "Application.hpp"

#include <imgui/imgui.h>
#include <imgui-SFML/imgui-SFML.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
/* APPLICATION */
Application::Application()
	: m_window("Caspian Engine | Empty Project")
{
	m_deltaTime = m_clock.restart().asSeconds();

#if defined(DEBUG)
		ImGui::SFML::Init(m_window.GetRenderWindow());
#endif
}

Application::~Application()
{
#if defined(DEBUG)
		ImGui::SFML::Shutdown();
#endif
}

void Application::MainLoop()
{
	Update();
	LateUpdate();
	Draw();
	CalculateDeltaTime();
}

void Application::Update()
{
	m_window.Update();

#if defined(DEBUG)

	ImGui::SFML::Update(m_window.GetRenderWindow(), sf::seconds(m_deltaTime));

	m_engineModule.Update();

	if (auto tools_mgr = m_engineModule.GetToolsManager())
	{
		tools_mgr->Update(m_deltaTime, Tools::EToolsSystem::ImGui);
	}
#endif
}

void Application::LateUpdate()
{
#if defined(DEBUG)
	if (auto tools_mgr = m_engineModule.GetToolsManager())
	{
		tools_mgr->Render(Tools::EToolsSystem::ImGui);
	}
#endif
}

void Application::Draw()
{
	m_window.BeginDraw();

#if defined(DEBUG)
		ImGui::SFML::Render(m_window.GetRenderWindow());
#endif

	m_window.EndDraw();
}

void Application::CalculateDeltaTime()
{
	m_deltaTime = m_clock.restart().asSeconds();
}

bool Application::IsRunning() const
{
	return m_window.IsOpen();
}


////////////////////////////////////////////////////////////////////////////////
/* DEBUG */
#if defined(DEBUG)
	void DebugHelper::InitializeDebugEventListeners()
	{
		m_keyReleasedListener = std::make_unique<Events::Listener>();
		m_keyReleasedListener->NotifyOn(sf::Event::KeyReleased);

		m_keyReleasedListener->SetCallback([](const sf::Event& event)
			{
				std::cout << "DEBUG: [EventListener] Key released: " << event.key.code << "\n";
			});

		auto& main_instance = ApplicationSingleton::Instance();

		main_instance.GetEventDispatcher()->AddObserver(m_keyReleasedListener.get());
	}
#endif