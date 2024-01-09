#include "engine/pch.hpp"

#include "Projects.hpp"

void Projects::Manager::FillProjectsList(const std::shared_ptr<Serializable::JSON::ProjectsInfo>& _projects_info)
{
	for (const Serializable::JSON::ProjectInfo& project_info : _projects_info->m_Projects)
	{
		m_Projects.emplace_back(project_info.m_ProjectName, project_info.m_ProjectPath);
	}
}

void Projects::Manager::PushProject(const Serializable::JSON::ProjectInfo& _project_info)
{
	m_Projects.emplace_back(_project_info.m_ProjectName, _project_info.m_ProjectPath);
}

void Projects::Manager::RemoveProject(const std::string& _project_name)
{
	m_Projects.erase(std::remove_if(m_Projects.begin(), m_Projects.end(), [&_project_name](const Project& _project)
		{
			return _project.m_ProjectName == _project_name;
		}), m_Projects.end());
}

std::vector<Serializable::JSON::ProjectInfo> Projects::Manager::GetProjectsInfo() const
{
	std::vector<Serializable::JSON::ProjectInfo> projects_info;
	projects_info.reserve(m_Projects.size());

	for (const Project& p : m_Projects)
	{
				projects_info.emplace_back(p.m_ProjectName, p.m_ProjectPath);
	}

	return projects_info;
}

bool Projects::Manager::SetCurrentProject(const std::string& _project_name)
{
	for (const Project& p : m_Projects)
	{
		if (p.m_ProjectName == _project_name)
		{
			m_CurrentProject = p;

			ApplicationSingleton::Instance().GetEngineModule().OnProjectChanged();

			return true;
		}
	}

	return false;
}
