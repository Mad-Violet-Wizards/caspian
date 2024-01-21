#pragma once
#include "engine/Filesystem/FilesystemMark.hpp"
#include "engine/Core/Serializable/LevelSerializable.hpp"

namespace Level
{
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

		void PushInitialLevelData(std::shared_ptr<Serializable::JSON::LevelInfo>& _level_info);
		const std::vector<Serializable::JSON::LevelInfo>& GetInitialLevelsData() const { return m_InitialLevelsData; }

	private:

		std::vector<Serializable::JSON::LevelInfo> m_InitialLevelsData;
	};
}