#include "game/pch.hpp"

#include "Game.hpp"

#include <imgui/imgui.h>
#include <imgui-SFML/imgui-SFML.h>

using namespace Game;

Main::Main()
	: m_window("Caspian Game")
{
	m_deltaTime = m_clock.restart().asSeconds();

	if constexpr(DEBUG)
	{
		ImGui::SFML::Init(m_window.GetRenderWindow());
	}
}

Main::~Main()
{
	if constexpr(DEBUG)
	{
		ImGui::SFML::Shutdown();
	}
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

	if constexpr(DEBUG)
	{
		ImGui::SFML::Update(m_window.GetRenderWindow(), sf::seconds(m_deltaTime));
	}
}

void Main::LateUpdate()
{
	// Draw debug imGui shit.
	if constexpr (DEBUG) {
		ImGui::Begin("Hello, world!");
		ImGui::Button("Some debug button.");
		ImGui::End();
	}
}

void Main::Draw()
{
	m_window.BeginDraw();

	if constexpr(DEBUG)
	{
		ImGui::SFML::Render(m_window.GetRenderWindow());
	}

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
