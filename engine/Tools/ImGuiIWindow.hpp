#pragma once

#include "ImGuiUtils.hpp"

namespace Tools
{
	class ImGuiManager;
	class ImGuiIWindow
	{
	public:

		ImGuiIWindow(ImGuiManager* _mgr)
			: m_Manager(_mgr) {}

		virtual ~ImGuiIWindow() = default;

		virtual void Update(float _dt) = delete;
		virtual void Render() = 0;

	public:

		bool m_Active = utils::TOOLS_INIT_ACTIVE_STATE;

	protected:

		ImGuiManager* m_Manager = nullptr;
	};
};