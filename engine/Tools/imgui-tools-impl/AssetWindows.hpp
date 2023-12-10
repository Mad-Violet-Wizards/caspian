#pragma once

#include "IWindow.hpp"
#include "AssetsTableActions.hpp"

namespace Tools_Impl
{
	/////////////////////////////////////////////////////////
	class ImportAssetWindow : public IWindow
	{
	public:

		ImportAssetWindow(Manager* _mgr) : IWindow(_mgr) {}
		~ImportAssetWindow() = default;

		void Render() override;

	private:

		std::string m_srcPath;
		std::string m_destPath;
		std::string m_srcFileName;
	};

	/////////////////////////////////////////////////////////
	class AssetsListWindow : public IWindow
	{
		public:

			AssetsListWindow(Manager* _mgr) : IWindow(_mgr) {}
			~AssetsListWindow() = default;

			void OnOpenForAction(IAssetsTableActionsListener* _listener) { m_ActionListener = _listener; }

			void NotifyAssetSelected(const SelectedAssetData& _data);

			void Update(float _dt) override;
			void Render() override;

	public:

			struct Internal_AssetTableData
			{
				std::string m_Name;
				std::string m_RelativePath;
				std::string m_AbsolutePath;
				std::string m_Type;

				bool m_bSearchFlag = false;
			};

			struct Internal_AssetToDelete
			{
				Internal_AssetToDelete(const std::string& _name, const std::string& _type, const std::function<void(bool)>& _onDelete)
					: m_Name(_name), 
						m_Type(_type), 
						m_OnDelete(_onDelete) { }

				const std::string& m_Name;
				const std::string& m_Type;

				const std::function<void(bool)> m_OnDelete;
			};


		private:

			std::vector<Internal_AssetTableData> m_RegisteredAssets;
			std::optional<Internal_AssetToDelete> m_AssetSelectedToDelete;

			IAssetsTableActionsListener* m_ActionListener;

			std::string m_SearchPhrase;
	};
};