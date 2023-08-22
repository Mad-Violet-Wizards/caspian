#include "engine/pch.hpp"

#include "FsManager.hpp"

namespace fs
{
	Manager::~Manager()
	{
		for (auto& [alias, fs] : m_FileSystems)
					fs->Shutdown();
	}

	void Manager::Mount(const std::string& alias, std::unique_ptr<IFileSystem> filesystem)
	{
		m_FileSystems[alias] = std::move(filesystem);
	}

	void Manager::Dismount(const std::string &alias)
	{
		if (!m_FileSystems.count(alias))
			return;

		auto it = m_FileSystems.find(alias);
		it->second->Shutdown();
		m_FileSystems.erase(it);
	}

	bool Manager::Exists(const std::string &alias) const
	{
		return m_FileSystems.count(alias);
	}

	IFileSystem* Manager::Get(const std::string& alias) const
	{
		if (!m_FileSystems.count(alias))
			return nullptr;

		return m_FileSystems.at(alias).get();
	}
}