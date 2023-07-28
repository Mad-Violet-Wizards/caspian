#pragma once

#include "IImGuiWidget.hpp"

namespace Tools
{
	/////////////////////////////////////////////////////////
	class ImGuiImportAssetWindow : public IImGuiWidget
	{
	public:

		ImGuiImportAssetWindow() = default;
		~ImGuiImportAssetWindow() = default;

		void Render() override;
	};

	/////////////////////////////////////////////////////////
	class ImGuiAssetListWindow : public IImGuiWidget
	{
	public:

		ImGuiAssetListWindow() = default;
		~ImGuiAssetListWindow() = default;

		void Render() override;
	};

};