#pragma once

#include "IWindow.hpp"

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

			void Update(float _dt) override;
			void Render() override;

		public:

			struct Internal_AssetTableData
			{
				std::string m_Name;
				std::string m_Path;
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

			std::string m_SearchPhrase;
	};
};