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

	struct TilesetTile
	{
		Random::UUID m_TilesetUUID;
		unsigned int m_TileX;
		unsigned int m_TileY;

		bool Valid() const { return m_TilesetUUID != Random::EMPTY_UUID; }
	};

	struct WorkingLayer
	{
		unsigned int m_LayerIndex;
		ETag m_Tag;

		bool Valid() const { return m_Tag != ETag::None; }
	};

	public:

		LevelEditController();
		~LevelEditController();

		void Update(float _dt) override;
		void SetMode(ELevelEditControllerMode _mode);
		void SetSelectedWorkingLayer(unsigned int _layer, ETag _drawable_type);
		void OnTilesetTileSelected(Random::UUID _tilesetId, unsigned int _tile_x, unsigned int _tile_y);
		void OnEvent(const sf::Event& _event);

		const WorkingLayer& GetCurrentSelectedWorkingLayer() const { return m_SelectedWorkingLayer; }

	private:

		void OnStateChanged();
		const sf::Vector2u RoundMouseWorldPosition(const sf::Vector2f& _pos, unsigned int _tileSize) const;

	private:

		void OnPaint();
		void OnErase();

		void OnPlaceNewCollision();
		void OnRemoveCollision();

		ELevelEditControllerMode m_Mode;
		ELevelEditControllerMode m_PrevMode;

		TilesetTile m_SelectedTilesetTile;
		WorkingLayer m_SelectedWorkingLayer;
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