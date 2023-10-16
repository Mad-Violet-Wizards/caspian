#pragma once

#include "IFileSystem.hpp"

namespace fs
{
	namespace io { enum class OpenMode; }
	
	class NativeFileSystem final : public IFileSystem
	{

		public:

			NativeFileSystem(std::string_view _path);
			~NativeFileSystem() = default;

			virtual bool Initialize() override;
			virtual bool Shutdown() override;

			virtual bool IsInitialized() const override;

			virtual std::shared_ptr<IFile> OpenFile(std::string_view _path, io::OpenMode _mode) override;
			virtual bool CloseFile(std::shared_ptr<IFile> _file) override;

			virtual bool CreateFile(std::string_view _file_path, IFile::EType _file_type) override;
			virtual bool RegisterFile(std::string_view _file_path) override;
			virtual bool RemoveFile(std::string_view _file_path) override;
			virtual bool CopyFile(std::string_view _src_path, std::string_view _dest_path) override;
			virtual bool RenameFile(std::string_view _src_path, std::string_view _dest_path) override;
			virtual bool FileExists(std::string_view _file_path) const override;

		private:

			static sf::Mutex m_Mutex;
	};
};