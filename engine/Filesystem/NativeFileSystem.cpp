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

			std::shared_ptr<IFile> file = std::make_shared<NativeFile>(path);

			if (relative_path[0] == '\\' || relative_path[0] == '/')
				relative_path.remove_prefix(1);

			m_Files.emplace(relative_path, file);
		}

		m_IsInitialized = true;

		return m_IsInitialized;
	}

	bool NativeFileSystem::Shutdown()
	{
		for (auto& file : m_OpenedFiles)
			file->Close();

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
		const std::string path{ _path };

		if (!m_Files.count(path))
			return nullptr;

		for (const auto file : m_OpenedFiles)
		{
			if (file->GetPath() == _path)
				return file;
		}

		std::shared_ptr<IFile> file = m_Files.at({ _path.cbegin(), _path.cend() });
		file->Open(_mode);
		m_OpenedFiles.push_back(file);

		// TODO: Extra check if file exists on the disk & we've privaleges to operate on it with current mode.

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
		std::filesystem::path file_path{ m_Path };
		file_path /= _file_path;

		// Validate extensions.
		if (_file_type != IFile::EType::Directory)
		{
			const std::string& extension = file_path.extension().string();
			const std::string_view supported_extensions = IFile::TypeToFileExtension(_file_type);

			if (supported_extensions != extension)
				return false;
		}

		bool os_file_operation_result = false;
		switch (_file_type)
		{
			// TODO: Add other file types.
			case IFile::EType::Text:
			{
				std::ofstream file(file_path);
				os_file_operation_result = file.good();
				break;
			}
			case IFile::EType::Directory:
				os_file_operation_result = std::filesystem::create_directory(file_path);
				break;
			default:
				return false;
		}

		if (os_file_operation_result)
		{
			std::string_view relative_path = _file_path;

			std::shared_ptr<IFile> file = std::make_shared<NativeFile>(file_path.string());

			if (relative_path[0] == '\\' || relative_path[0] == '/')
				relative_path.remove_prefix(1);

			m_Files.emplace(relative_path, file);
		}

		return os_file_operation_result;
	}

	bool NativeFileSystem::RemoveFile(std::string_view _file_path)
	{
		if (!FileExists(_file_path))
			return false;

		std::filesystem::path file_path{ m_Path };
		file_path /= _file_path;

		return std::filesystem::remove(file_path);
	}

	bool NativeFileSystem::CopyFile(std::string_view _src_path, std::string_view _dest_path)
	{
		const std::filesystem::path path{ m_Path };
		const std::filesystem::path src_path = path / _src_path;
		const std::filesystem::path dest_path = path / _dest_path;

		if (!std::filesystem::exists(src_path))
			return false;

		std::filesystem::copy(src_path, dest_path);

		return std::filesystem::exists(dest_path);
	}

	bool NativeFileSystem::RenameFile(std::string_view _src_path, std::string_view _dest_path)
	{
		const std::filesystem::path path{ m_Path };
		const std::filesystem::path src_path = path / _src_path;
		const std::filesystem::path dest_path = path / _dest_path;

		if (!std::filesystem::exists(src_path))
			return false;

		std::filesystem::rename(src_path, dest_path);
		
		return std::filesystem::exists(dest_path);
	}


	bool NativeFileSystem::FileExists(std::string_view _file_path) const
	{
		std::filesystem::path file_path{ m_Path };
		file_path /= _file_path;

		return std::filesystem::exists(file_path);
	}
}
