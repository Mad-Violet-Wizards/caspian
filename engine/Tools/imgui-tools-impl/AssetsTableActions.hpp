#pragma once

namespace Tools_Impl
{


	class SelectedAssetData
	{
		public:
		
			std::string_view m_RelativePath; // To get the data from the asset manager.
	};

	class IAssetsTableActionsListener
	{
	public:

		virtual void OnAssetSelected(const SelectedAssetData& data) = 0;

	};
}