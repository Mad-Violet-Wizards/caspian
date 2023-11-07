#include "engine/pch.hpp"

#include "IFileSystem.hpp"

namespace fs
{
	IFileSystem::IFileSystem(std::string_view _path)
		: m_IsInitialized(false)
		, m_Path(_path)
	{ }

	const std::vector<std::string> IFileSystem::GetFilesAliases() const
	{
		std::vector<std::string> files_aliases;

		files_aliases.reserve(m_Files.size());

		for (auto const& [key, value] : m_Files)
			files_aliases.push_back(key);
		
		return files_aliases;
	}

	std::vector<std::string> IFileSystem::BuildFilesList(std::string_view _path) const
	{
		std::vector<std::string> files;

		if (std::filesystem::exists(_path))
			for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(_path))
				files.push_back(dir_entry.path().string());

		return files;
	}
}