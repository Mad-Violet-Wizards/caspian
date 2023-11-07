#pragma once

namespace Level
{
	class Tile
	{
	public:

		Tile(unsigned int _tileset_id, size_t _tileset_x, size_t _tileset_y, size_t width, size_t height);
		~Tile() = default;

		unsigned int GetTilesetID() const { return m_TilesetID; }
		unsigned int GetTilesetX() const { return m_TilesetX; }
		unsigned int GetTilesetY() const { return m_TilesetY; }
		unsigned int GetID() const { return m_ID; }
		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

		void Draw(Window& _window);

	private:

		unsigned int m_TilesetID;
		unsigned int m_ID;
		unsigned int m_TilesetX;
		unsigned int m_TilesetY;
		unsigned int m_Width;
		unsigned int m_Height;
	};


	class Layer
	{
	public:

		Layer(size_t width, size_t height);
		~Layer() = default;

	private:

		unsigned int m_Width;
		unsigned int m_Heihght;

		std::vector<std::vector<Tile>> m_Tiles;

	};

	class Tileset
	{
		public:

			Tileset(const std::string& _path);
			~Tileset() = default;

		private:

			unsigned int m_ID;
			unsigned int m_TileCount;
			unsigned int m_TileWidth;
			unsigned int m_TileHeight;
			unsigned int m_TileSpacing;
			unsigned int m_TileMargin;
			unsigned int m_ImageWidth;
			unsigned int m_ImageHeight;

			std::string m_ImagePath;
	};

	class Level
	{
	public:

		Level() = default;
		~Level() = default;

		void Draw(Window& _window);
		void Update(float _dt);

	private:

		std::vector<Layer> m_layers;
	};

	class World
	{
	public:

		World() = default;
		~World() = default;

		void ActivateLevel(const std::string& _level_name);
		void DeactivateLevel(const std::string& _level_name);

		void SwitchToLevel(const std::string& _level_name);

		void Draw();
		void Update(float _dt);

	private:

		std::unordered_map<std::string, Level> m_levels;
	};
}