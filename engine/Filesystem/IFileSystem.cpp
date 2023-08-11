#include "engine/pch.hpp"

#include "IFileSystem.hpp"

namespace fs
{
	IFileSystem::IFileSystem(std::string_view _path)
		: m_IsInitialized(false)
		, m_Path(_path)
	{ }

	std::vector<std::string> IFileSystem::BuildFilesList(std::string_view _path) const
	{
		std::vector<std::string> files;

		auto files_in_dir = std::distance(std::filesystem::directory_iterator(_path), std::filesystem::directory_iterator{});
		files.reserve(files_in_dir);

		if (std::filesystem::exists(_path))
			for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(_path))
				files.push_back(dir_entry.path().string());

		return files;
	}
}