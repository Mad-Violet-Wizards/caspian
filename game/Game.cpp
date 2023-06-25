#include "game/pch.hpp"

#include "Game.hpp"

Game::Game()
	: m_window("Caspian Game")
{
	m_deltaTime = m_clock.restart().asSeconds();
}

Game::~Game()
{

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
}

void Game::LateUpdate()
{

}

void Game::Draw()
{
	m_window.BeginDraw();
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
