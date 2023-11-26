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
	class LoadLevelWindow : public IWindow
	{
	public:

		LoadLevelWindow(Manager* _mgr) : IWindow(_mgr) {}
		~LoadLevelWindow() = default;

		void Render() override;
	};
	/////////////////////////////////////////////////////////
	class TilesetListWindow : public IWindow, public IAssetsTableActionsListener
	{
		public:

			TilesetListWindow(Manager* _mgr) 
				: IWindow(_mgr) 
				{ 
					m_TilesetPreview = nullptr;
				}

			~TilesetListWindow() = default;

			void OnAssetSelected(const SelectedAssetData& data) override;

			void Render() override;

	private:

			std::optional<SelectedAssetData> m_QueuedToAddAsTileset;
			
			const sf::Texture* m_TilesetPreview;
			
	};


	/////////////////////////////////////////////////////////
	class LevelEditorWindow : public IWindow
	{
		public:

			LevelEditorWindow(Manager* _mgr) 
				: IWindow(_mgr) 
				, m_NewLevelWindow(_mgr)
				, m_LoadLevelWindow(_mgr)
				, m_TilesetListWindow(_mgr) {}

			~LevelEditorWindow() = default;

			void Update(float _dt) override;
			void Render() override;

		private:

			NewLevelWindow m_NewLevelWindow;
			LoadLevelWindow m_LoadLevelWindow;

			TilesetListWindow m_TilesetListWindow;

	};
}