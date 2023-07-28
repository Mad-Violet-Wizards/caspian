#pragma once

#include "ImGuiTools.hpp"

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

			ImGuiManager m_imGuiManager;

			std::unique_ptr<Events::Listener> m_keyReleasedListener;
	};
};