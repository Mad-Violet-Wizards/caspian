#pragma once

#include "IFileSystem.hpp"

namespace fs
{
	class Manager
	{
		public:

			Manager() = default;
			~Manager();

			void Mount(const std::string& alias, std::unique_ptr<IFileSystem> filesystem);
			void Dismount(const std::string& alias);

			bool Exists(const std::string &alias) const;
			IFileSystem* Get(const std::string& alias) const;

		private:

			std::map<std::string, std::unique_ptr<IFileSystem>> m_FileSystems;
	};
}