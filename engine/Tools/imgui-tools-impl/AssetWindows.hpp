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

			void SetAvailability(bool _available) { m_Available = _available; }
			bool GetAvailability() const { return m_Available; }

		public:

			struct Internal_AssetTableData
			{
				std::string m_Name;
				std::string m_Path;
				std::string m_Type;

				bool m_bSearchFlag = false;
			};


		private:

			std::vector<Internal_AssetTableData> m_RegisteredAssets;
			bool m_Available = false;
			
			std::string m_SearchPhrase;
	};
};