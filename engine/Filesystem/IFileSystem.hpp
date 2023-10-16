#pragma once

#include "IFile.hpp"

namespace fs
{
	namespace io { enum class OpenMode;  }

	class IFileSystem
	{
		public:

			IFileSystem(std::string_view _path);
			virtual ~IFileSystem() = default;

			virtual bool Initialize() = 0;
			virtual bool Shutdown() = 0;

			virtual bool IsInitialized() const = 0;


			[[nodiscard]] virtual std::shared_ptr<IFile> OpenFile(std::string_view _file_path, io::OpenMode _open_mode) = 0;
			[[nodiscard]] virtual bool CloseFile(std::shared_ptr<IFile> _file) = 0;
			virtual bool CreateFile(std::string_view _file_path, IFile::EType _file_type) = 0;
			virtual bool RegisterFile(std::string_view _file_path) = 0;
			virtual bool RemoveFile(std::string_view _file_path) = 0;
			virtual bool CopyFile(std::string_view _src_path, std::string_view _dest_path) = 0;
			virtual bool RenameFile(std::string_view _src_path, std::string_view _dest_path) = 0;
			virtual bool FileExists(std::string_view _file_path) const = 0;

			virtual const std::string& GetPath() const { return m_Path; };

			[[nodiscard]] virtual std::vector<std::string> BuildFilesList(std::string_view _path) const;

	protected:

			std::unordered_map<std::string, std::shared_ptr<IFile>> m_Files;
			std::vector<std::shared_ptr<IFile>> m_OpenedFiles;

			bool m_IsInitialized;

			std::string m_Path;
	};
}