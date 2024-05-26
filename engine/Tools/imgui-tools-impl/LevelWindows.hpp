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
		std::string m_TileSize;
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

			struct SelectedTileInfo
			{
				sf::Sprite m_Sprite;

				sf::IntRect m_Rect;
				Random::UUID m_TilesetUUID;
				bool m_Selected = false;
			};

			TilesetListWindow(Manager* _mgr) 
				: IWindow(_mgr) 
				{ 
					m_TilesetPreview = nullptr;
				}

			~TilesetListWindow() = default;

			void OnAssetSelected(const SelectedAssetData& data) override;

			void Update(float _dt) override;
			void Render() override;

			SelectedTileInfo* GetSelectedTileInfo() { return m_SelectedTileInfo; }

	private:

		std::string GetComboBoxTilesetName(Random::UUID _tileset_uuid) const;

	private:
		
			Random::UUID m_PrevTilesetUUID;
			bool m_bTilesetsListsOpened = false;

			std::optional<SelectedAssetData> m_QueuedToAddAsTileset;

			std::string m_TilesetName;
			std::string m_nTileWidth;
			std::string m_nTileHeight;

			float m_CurrentTilesetWidth = 0.f;
			float m_CurrentTilesetHeight = 0.f;
			
			const sf::Texture* m_TilesetPreview;
			SelectedTileInfo* m_SelectedTileInfo;

			std::vector<SelectedTileInfo> m_CachedTilesetSprites;
			std::map<Random::UUID, std::vector<sf::IntRect>> m_CachedTilesetRects;
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