#include "game/pch.hpp"

#include "Game.hpp"

#include <imgui/imgui.h>
#include <imgui-SFML/imgui-SFML.h>
#include <iostream>

using namespace Game;

////////////////////////////////////////////////////////////////////////////////
/* MAIN */
Main::Main()
	: m_window("Caspian Game")
{
	m_deltaTime = m_clock.restart().asSeconds();

	// Event dispatcher must be allocated almost immadietly as it's core system.
	m_eventDispatcher = std::make_unique<EventDispatcher>();

	#if defined(DEBUG)
	ImGui::SFML::Init(m_window.GetRenderWindow());
	#endif
}

Main::~Main()
{
	#if defined(DEBUG)
	ImGui::SFML::Shutdown();
	#endif
}

void Main::MainLoop()
{
	Update();
	LateUpdate();
	Draw();
	CalculateDeltaTime();
}

void Main::Update()
{
	m_window.Update();

	#if defined(DEBUG)
		ImGui::SFML::Update(m_window.GetRenderWindow(), sf::seconds(m_deltaTime));
	#endif
}

void Main::LateUpdate()
{
	#if defined(DEBUG)
	// Draw debug imGui shit.
	{
		ImGui::Begin("Hello, world!");
		ImGui::Button("Some debug button.");
		ImGui::End();
	}
	#endif
}

void Main::Draw()
{
	m_window.BeginDraw();

	#if defined(DEBUG)
	{
		ImGui::SFML::Render(m_window.GetRenderWindow());
	}
	#endif

	m_window.EndDraw();
}

void Main::CalculateDeltaTime()
{
	m_deltaTime = m_clock.restart().asSeconds();
}

bool Main::IsRunning() const
{
	return m_window.IsOpen();
}

////////////////////////////////////////////////////////////////////////////////
/* DEBUG */
#if defined(DEBUG)
	void DebugHelper::InitializeDebugEventListeners()
	{
		m_keyReleasedListener = std::make_unique<EventListener>();
		m_keyReleasedListener->NotifyOn(sf::Event::KeyReleased);

		m_keyReleasedListener->SetCallback([](const sf::Event& event)
			{
				std::cout << "DEBUG: [EventListener] Key released: " << event.key.code << "\n";
			});

		auto& main_instance = Game::MainSingleton::Instance();

		main_instance.GetEventDispatcher()->AddObserver(m_keyReleasedListener.get());
	}
#endif