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
	};

	/////////////////////////////////////////////////////////
	class AssetsListWindow : public IWindow
	{
	public:

		AssetsListWindow(Manager* _mgr) : IWindow(_mgr) {}
		~AssetsListWindow() = default;

		void Render() override;
	};

};