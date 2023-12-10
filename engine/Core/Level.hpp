#pragma once
#include "engine/Filesystem/FilesystemMark.hpp"

namespace Level
{
	namespace Data
	{
		// TODO: Deserialize/Serializer json struct wrapper.
		struct JsonRootFileData
		{
			JsonRootFileData(const std::string& _level_name, 
											 const std::string& _chunk_root_file, 
											 unsigned int _tile_width, 
											 unsigned int _tile_height)
				: m_LevelName{ _level_name }
				, m_ChunkRootFile{ _chunk_root_file }
				, m_TileWidth{ _tile_width }
				, m_TileHeight{ _tile_height }
			{
			}

			JsonRootFileData(nlohmann::json& _json)
				: m_LevelName { _json["name"].get<std::string>() }
				, m_ChunkRootFile{ _json["chunk_root_file"].get<std::string>() }
				, m_TileWidth{ _json["tile_width"].get<unsigned int>() }
				, m_TileHeight{ _json["tile_height"].get<unsigned int>() }
			{
			}

			nlohmann::json Deserialize() const
			{
				nlohmann::json res;
				res["name"] = m_LevelName;
				res["chunk_root_file"] = m_ChunkRootFile;
				res["tile_width"] = m_TileWidth;
				res["tile_height"] = m_TileHeight;
				res["fs_mark"] = fs::EFilesystemMarkToHash(fs::EFilesystemMark::Level);

				return res;
			}

			std::string m_LevelName;
			std::string m_ChunkRootFile;
			unsigned int m_TileWidth;
			unsigned int m_TileHeight;
		};

		class ChunkRootFile
		{
				ChunkRootFile() = default;
		};
	}

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

		void EmplaceInitialLevelData(nlohmann::json& json);
		const std::vector<Data::JsonRootFileData>& GetInitialLevelsData() const { return m_InitialLevelsData; }

	private:

		std::unordered_map<std::string, Level> m_levels;

		std::vector<Data::JsonRootFileData> m_InitialLevelsData;
	};
}