#pragma once

namespace Tools
{
	class IImGuiWidget
	{
	public:

		IImGuiWidget() = default;
		virtual ~IImGuiWidget() = default;

		virtual void Update(float _dt) = delete;
		virtual void Render() = 0;

	public:

		bool m_Active = false;
	};
};