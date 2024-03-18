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
};

///////////////////////////////////////////////////////////////////
enum class ELevelDebugControllerMode
{
	None,
	Place,
	Erase
};

class LevelDebugController : public IDebugController
{
	public:

		LevelDebugController();
		~LevelDebugController();

		void Update(float _dt) override;
		void SetMode(ELevelDebugControllerMode _mode);

	private:

		ELevelDebugControllerMode m_Mode;
};


enum class EDebugControllerType
{
	Camera,
	Level
};

//////////////////////////////////////////////////////////////////
class DebugEditorControllers
{
	public:

		DebugEditorControllers();
		~DebugEditorControllers() = default;

		void Update(float _dt);

		CameraDebugController* GetCameraController() const;
		LevelDebugController* GetLevelController() const;

	private:

		std::unordered_map<EDebugControllerType, std::unique_ptr<IDebugController>> m_DebugControllers;
};