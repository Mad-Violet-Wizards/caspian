#include "engine/pch.hpp"

#include "Window.hpp"

#include <imgui-SFML/imgui-SFML.h>

#include "game/Game.hpp"

Window::Window(const std::string& windowName)
	: m_renderWindow(sf::VideoMode(800, 600), windowName)
{
}

void Window::Update()
{
	sf::Event event;

	while (m_renderWindow.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(event);

		if (Game::MainSingleton::IsValid())
		{
			auto& main_singleton = Game::MainSingleton::Instance();

			// Get the EventDispatcher from main_singleton here and process SFML event.
		}

		if (event.type == sf::Event::Closed)
			m_renderWindow.close();
	}
}

void Window::BeginDraw()
{
	m_renderWindow.clear(sf::Color::Black);
}

void Window::Draw(const sf::Drawable& drawable)
{
	m_renderWindow.draw(drawable);
}

void Window::EndDraw()
{
	m_renderWindow.display();
}

bool Window::IsOpen() const
{
	return m_renderWindow.isOpen();
}

void Window::Exit()
{
	m_renderWindow.close();
}