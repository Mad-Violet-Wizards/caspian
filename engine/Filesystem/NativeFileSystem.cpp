#include "engine/pch.hpp"

#include "NativeFileSystem.hpp"
#include "NativeFile.hpp"
#include <iostream>

namespace fs
{
	sf::Mutex NativeFileSystem::m_Mutex;

	NativeFileSystem::NativeFileSystem(std::string_view _path)
		: IFileSystem(_path)
	{
	}

	bool NativeFileSystem::Initialize()
	{
		if (m_IsInitialized)
			return true;

		// Check if path exists.
		std::filesystem::path path(m_Path);
		if (!std::filesystem::exists(path))
		{
			std::filesystem::create_directories(path);
		}

		auto fn_create_engine_dir = [](const std::filesystem::path& _root_path, std::string_view _dir_to_create)
			{
				std::filesystem::path final_path = _root_path / _dir_to_create;

				if (!std::filesystem::exists(final_path))
				{
					std::filesystem::create_directories(final_path);
				}
			};

		if (IsProjectFilesystem())
		{
			fn_create_engine_dir(path, "levels");
			fn_create_engine_dir(path, "tilemaps");
			fn_create_engine_dir(path, "fonts");
		}

		// Build files list.
		std::vector<std::string> paths = BuildFilesList(m_Path);
		auto files_count = paths.size();

		for (const std::string_view path : paths)
		{
			std::string_view relative_path = path;
			relative_path.remove_prefix(m_Path.size());

			std::shared_ptr<IFile> file = std::make_shared<NativeFile>(path);

			if (relative_path[0] == '\\' || relative_path[0] == '/')
				relative_path.remove_prefix(1);
				
				m_Mutex.lock();
				m_Files.emplace(relative_path, file);
				m_Mutex.unlock();
		}

		m_Mutex.lock();
		std::cout << "DEBUG: [NativeFileSystem] Initialized: " << m_Path << " with: " << files_count << " files\n";
		m_Mutex.unlock();

		m_IsInitialized = true;

		return m_IsInitialized;
	}

	bool NativeFileSystem::Shutdown()
	{
		for (auto& file : m_OpenedFiles)
			file->Close();

		m_Mutex.lock();
		m_OpenedFiles.clear();
		m_Files.clear();
		m_Mutex.unlock();
		m_Path.clear();
		m_IsInitialized = false;
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

		m_Mutex.lock();
		std::shared_ptr<IFile> file = m_Files.at({ _path.cbegin(), _path.cend() });
		m_Mutex.unlock();

		file->Open(_mode);

		m_Mutex.lock();
		m_OpenedFiles.push_back(file);
		m_Mutex.unlock();

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
				m_Mutex.lock();
				m_OpenedFiles.erase(it);
				m_Mutex.unlock();
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
			const std::string_view supported_extensions = IFile::TypeToStringExt(_file_type);

			if (supported_extensions != extension)
				return false;
		}

		bool os_file_operation_result = false;
		switch (_file_type)
		{
			// TODO: Add other file types.
			case IFile::EType::Text:
			case IFile::EType::JSON:
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

			m_Mutex.lock();
			m_Files.emplace(relative_path, file);
			m_Mutex.unlock();
		}

		return os_file_operation_result;
	}

	bool NativeFileSystem::RegisterFile(std::string_view _file_path)
	{
		std::filesystem::path file_path{ m_Path };
		file_path /= _file_path;

		if (IFile::StringExtToType(file_path.extension().string()) == IFile::EType::Unknown)
			return false;

		std::string_view relative_path = _file_path;

		std::shared_ptr<IFile> file = std::make_shared<NativeFile>(file_path.string());

		if (relative_path[0] == '\\' || relative_path[0] == '/')
			relative_path.remove_prefix(1);

		m_Mutex.lock();
		m_Files.emplace(relative_path, file);
		m_Mutex.unlock();

		return true;
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
	 
	std::string NativeFileSystem::GetAbsoluteFilePath(std::string_view _file_path) const
	{
		const std::string path{ _file_path };
		
		if (auto find_it = m_Files.find(path); find_it != m_Files.end())
		{
			const std::string res{ find_it->second->GetPath() };
			return res;
		}
	
		return {};
	}

}
