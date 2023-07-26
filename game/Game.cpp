#include "game/pch.hpp"

#include "Game.hpp"

#include <imgui/imgui.h>
#include <imgui-SFML/imgui-SFML.h>

Game::Game()
	: m_window("Caspian Game")
{
	m_deltaTime = m_clock.restart().asSeconds();

	if constexpr(DEBUG)
	{
		ImGui::SFML::Init(m_window.GetRenderWindow());
	}
}

Game::~Game()
{
	if constexpr(DEBUG)
	{
		ImGui::SFML::Shutdown();
	}
}

void Game::GameLoop()
{
	Update();
	LateUpdate();
	Draw();
	CalculateDeltaTime();
}

void Game::Update()
{
	m_window.Update();

	if constexpr(DEBUG)
	{
		ImGui::SFML::Update(m_window.GetRenderWindow(), sf::seconds(m_deltaTime));
	}
}

void Game::LateUpdate()
{
	// Draw debug imGui shit.
	if constexpr (DEBUG) {
		ImGui::Begin("Hello, world!");
		ImGui::Button("Some debug button.");
		ImGui::End();
	}
}

void Game::Draw()
{
	m_window.BeginDraw();

	if constexpr(DEBUG)
	{
		ImGui::SFML::Render(m_window.GetRenderWindow());
	}

	m_window.EndDraw();
}

void Game::CalculateDeltaTime()
{
	m_deltaTime = m_clock.restart().asSeconds();
}

bool Game::IsRunning() const
{
	return m_window.IsOpen();
}
