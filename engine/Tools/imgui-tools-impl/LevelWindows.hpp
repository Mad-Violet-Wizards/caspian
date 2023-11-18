#pragma once

#include "IWindow.hpp"
#include "AssetsTableActions.hpp"

namespace Tools_Impl
{
	/////////////////////////////////////////////////////////
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

	/////////////////////////////////////////////////////////
	class TilesetListWindow : public IWindow, public IAssetsTableActionsListener
	{
	public:

		TilesetListWindow(Manager* _mgr) : IWindow(_mgr) {}
		~TilesetListWindow() = default;

		void OnAssetSelected(const SelectedAssetData& data) override;

		void Render() override;
	};

	/////////////////////////////////////////////////////////
	class LevelEditorWindow : public IWindow
	{
		public:

			LevelEditorWindow(Manager* _mgr) : IWindow(_mgr) {}
			~LevelEditorWindow() = default;

			void Update(float _dt) override;
			void Render() override;

		private:



	};
}