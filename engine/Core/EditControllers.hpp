#pragma once
///////////////////////////////////////////////////////////////////
class IEditorController
{
	public:
	
		IEditorController() = default;
		virtual ~IEditorController() = default;

		virtual void Update(float _dt) = 0;

		void SetActive(bool _state);
		bool IsActive() const { return m_Active; }

		virtual void OnActivated() {};
		virtual void OnDeactivated() {};

	protected:

		bool m_Active = false;
};

///////////////////////////////////////////////////////////////////
class CameraEditController : public IEditorController
{
	public:

		CameraEditController();
		~CameraEditController() = default;

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
enum class ELevelEditControllerMode
{
	None,
	PaintTile,
	EditCollisions,
	Erase
};

class LevelEditController : public IEditorController
{
	public:

		LevelEditController();
		~LevelEditController();

		void Update(float _dt) override;
		void SetMode(ELevelEditControllerMode _mode);
		void SetWorkingLayer(unsigned int _layer);
		void OnTilesetTileSelected(Random::UUID _tilesetId, unsigned int _tile_x, unsigned int _tile_y);

		void OnLevelActivated(Level::Level* _level);
		void OnLevelDeactivated();

	private:

		void OnStateChanged();
		const sf::Vector2u RoundMouseWorldPosition(const sf::Vector2f& _pos, unsigned int _tileSize) const;

	private:

		void OnPaint();
		void OnErase();

		void OnPlaceNewCollision();
		void OnRemoveCollision();

		void UpdateHighlightTile();

		ELevelEditControllerMode m_Mode;
		ELevelEditControllerMode m_PrevMode;
		unsigned int m_WorkingLayer = -1;

		Random::UUID m_TilesetUUID;
		unsigned int m_TileSize;
		unsigned int m_TileX;
		unsigned int m_TileY;
};


enum class EDebugControllerType
{
	Camera,
	Level
};

//////////////////////////////////////////////////////////////////
class EditControllersManager
{
	public:

		EditControllersManager();
		~EditControllersManager() = default;

		void Update(float _dt);

		CameraEditController* GetCameraController() const;
		LevelEditController* GetLevelController() const;

	private:

		std::unordered_map<EDebugControllerType, std::unique_ptr<IEditorController>> m_DebugControllers;
};