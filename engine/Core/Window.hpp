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

		void UpdateTitle(const std::string& _title);
		
		[[nodiscard]] sf::RenderWindow& GetRenderWindow() { return m_renderWindow; }
		const sf::View& GetView() const { return m_renderWindow.getView(); }

	private:

		Events::Dispatcher* m_eventDispatcher = nullptr;
		sf::RenderWindow m_renderWindow;
};