#pragma once

#include "Utils.hpp"

namespace Tools_Impl
{
	class Manager;

	class IWindow
	{
	public:

		IWindow(Manager* _mgr)
			: m_Manager(_mgr) {}

		virtual ~IWindow() = default;

		virtual void Update(float _dt) {};
		virtual void Render() = 0;

	public:

		bool m_Active = utils::TOOLS_INIT_ACTIVE_STATE;

	protected:

		Manager* m_Manager = nullptr;
	};
};