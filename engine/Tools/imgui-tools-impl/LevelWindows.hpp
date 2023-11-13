#pragma once

#include "IWindow.hpp"

namespace Tools_Impl
{
	class NewLevelWindow : public IWindow
	{
	public:

		NewLevelWindow(Manager* _mgr) : IWindow(_mgr) {}
		~NewLevelWindow() = default;

		void Render() override;

	private:

		std::string m_LevelName;
		std::string m_LevelPath;
		std::string m_nTileWidth;
		std::string m_nTileHeight;
	};
}