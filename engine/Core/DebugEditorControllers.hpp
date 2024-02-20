#pragma once
///////////////////////////////////////////////////////////////////
class IDebugController
{
	public:
	
		IDebugController() = default;
		virtual ~IDebugController() = default;

		virtual void Update(float _dt) = 0;

		void SetActive(bool _state);
		bool IsActive() const { return m_Active; }

		virtual void OnActivated() {};
		virtual void OnDeactivated() {};

	protected:

		bool m_Active = false;
};

///////////////////////////////////////////////////////////////////
class CameraDebugController : public IDebugController
{
	public:

		CameraDebugController();
		~CameraDebugController() = default;

		void OnActivated() override;
		void OnDeactivated() override;

		void Update(float _dt) override;

		bool ValidateMouseDelta() const;
		const sf::Vector2i& GetMouseDelta() const;

	private:

		void OnZoomChanged(float _delta);

		void InitializeEventListeners();


	private:

		sf::Vector2i m_LastMousePos;
		sf::Vector2i m_MouseDelta;
		
		std::unique_ptr<Events::Listener> m_mouseScrollListener;
};


enum class EDebugControllerType
{
	Camera
};

//////////////////////////////////////////////////////////////////
class DebugEditorControllers
{
	public:

		DebugEditorControllers();
		~DebugEditorControllers() = default;

		void Update(float _dt);

		CameraDebugController* GetCameraController() const;

	private:

		std::unordered_map<EDebugControllerType, std::unique_ptr<IDebugController>> m_DebugControllers;
};