#pragma once

#include "imgui-tools-impl/ToolsImpl.hpp"

namespace Tools
{
	/////////////////////////////////////////////////////////
	enum class EToolsSystem
	{
		None,
		ImGui
	};

	/////////////////////////////////////////////////////////
	class Manager
	{
		public:

			Manager() = default;
			~Manager() = default;

			void Update(float _dt, EToolsSystem _tools_system);
			void Render(EToolsSystem _tools_system);

			void InitializeEventListeners();

		private:

			Tools_Impl::Manager m_ImplManager;

			std::unique_ptr<Events::Listener> m_keyReleasedListener;
	};
};