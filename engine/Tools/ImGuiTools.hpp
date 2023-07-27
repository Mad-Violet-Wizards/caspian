#pragma once

namespace Tools
{
	/////////////////////////////////////////////////////////
	class IImGuiWidget
	{
		virtual void Update(float _dt) = 0;
		virtual void Render() = 0;
	};

	/////////////////////////////////////////////////////////
	class ImGuiMenubar : public IImGuiWidget
	{
		void Update(float _dt) override;
		void Render() override;
	};

	/////////////////////////////////////////////////////////
	class ImGuiManager
	{
		public:

			ImGuiManager() = default;
			~ImGuiManager() = default;

			void ToggleActiveState();

			void Update(float _dt);
			void Render();

		private:

			bool m_Active : 1 = false;
	};
};