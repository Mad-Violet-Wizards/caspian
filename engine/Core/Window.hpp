#pragma once

class Window
{
	public:

		Window(const std::string& windowName);
		~Window() = default;

		void Update();
		void BeginDraw();
		void Draw(const sf::Drawable& drawable);
		void EndDraw();

		bool IsOpen() const;
		void Exit();
		
		[[nodiscard]] sf::RenderWindow& GetRenderWindow() { return m_renderWindow; }

	private:

		sf::RenderWindow m_renderWindow;
};