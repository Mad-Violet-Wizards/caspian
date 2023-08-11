#include "engine/pch.hpp"

#include "NativeFileSystem.hpp"
#include "NativeFile.hpp"

namespace fs
{
	NativeFileSystem::NativeFileSystem(std::string_view _path)
		: IFileSystem(_path)
	{
	}

	bool NativeFileSystem::Initialize()
	{
		if (m_IsInitialized)
			return true;

		std::vector<std::string> paths = BuildFilesList(m_Path);

		for (const std::string_view path : paths)
		{
			std::string_view relative_path = path;
			relative_path.remove_prefix(m_Path.size());

			std::shared_ptr<IFile> file = std::make_shared<NativeFile>(relative_path);

			m_Files.emplace(relative_path, file);
		}

		m_IsInitialized = true;

		return m_IsInitialized;
	}

	bool NativeFileSystem::Shutdown()
	{
		m_Files.clear();
		m_OpenedFiles.clear();
		m_Path.clear();

		return true;
	}

	bool NativeFileSystem::IsInitialized() const
	{
		return m_IsInitialized;
	}

	std::shared_ptr<IFile> NativeFileSystem::OpenFile(std::string_view _path, io::OpenMode _mode)
	{
		if (!m_Files.count(_path))
			return nullptr;

		for (const auto file : m_OpenedFiles)
		{
			if (file->GetPath() == _path)
				return file;
		}

		std::shared_ptr<IFile> file = m_Files.at(_path);
		file->Open(_mode);

		// TODO: Extra check if file exists on the disk & we've privaledges to open it.

		return file;
	}

	bool NativeFileSystem::CloseFile(std::shared_ptr<IFile> _file)
	{
		for (auto it = m_OpenedFiles.begin(); it != m_OpenedFiles.end(); ++it)
		{
			if (*it == _file)
			{
				_file->Close();
				m_OpenedFiles.erase(it);
				return true;
			}
		}

		return false;
	}

	bool NativeFileSystem::CreateFile(std::string_view _file_path, IFile::EType _file_type)
	{
		const std::string_view file_extension_from_etype = IFile::TypeToFileExtension(_file_type);
		
		if (file_extension_from_etype == IFile::S_UNKNOWN_ETYPE_STR)
			return false;

		std::filesystem::path file_path { _file_path };

		if (file_path.extension() != file_extension_from_etype)
			return false;

		return std::filesystem::create_directories(file_path.parent_path());
	}

	bool NativeFileSystem::RemoveFile(std::string_view _file_path)
	{
		if (!std::filesystem::exists(_file_path))
			return false;

		return std::filesystem::remove(_file_path);
	}

	bool NativeFileSystem::CopyFile(std::string_view _src_path, std::string_view _dest_path)
	{
		const bool src_file_exists = std::filesystem::exists(_src_path);
		const bool dest_exists = std::filesystem::exists(_dest_path);

		if (!src_file_exists || !dest_exists)
			return false;

		return std::filesystem::copy_file(_src_path, _dest_path);
	}

	bool NativeFileSystem::RenameFile(std::string_view _src_path, std::string_view _dest_path)
	{
	const bool src_file_exists = std::filesystem::exists(_src_path);
		const bool dest_exists = std::filesystem::exists(_dest_path);

		if (!src_file_exists || dest_exists)
			return false;

		std::filesystem::rename(_src_path, _dest_path);
		return true;
	}

	bool NativeFileSystem::FileExists(std::string_view _file_path) const
	{
		return std::filesystem::exists(_file_path);
	}
}
