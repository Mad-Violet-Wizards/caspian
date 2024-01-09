#pragma once

#include "Serializable/ProjectSerializable.hpp"

namespace Projects
{
	class Project
	{
	public:

		std::string m_ProjectName;
		std::string m_ProjectPath;

		[[nodiscard]] bool operator==(const Project& _other) const
		{
			return m_ProjectName == _other.m_ProjectName &&
				m_ProjectPath == _other.m_ProjectPath;
		}
	};

	class Manager
	{
	public:

		Manager() = default;
		~Manager() = default;

		void FillProjectsList(const std::shared_ptr<Serializable::JSON::ProjectsInfo>& _projects_info);
		void PushProject(const Serializable::JSON::ProjectInfo& _project_info);
		void RemoveProject(const std::string& _project_name);
		std::vector<Serializable::JSON::ProjectInfo> GetProjectsInfo() const;
		const std::vector<Project>& GetProjects() const { return m_Projects; }

		bool SetCurrentProject(const std::string& _project_name);
		const Project& GetCurrentProject() const { return m_CurrentProject.value(); }
		bool IsAnyProjectLoaded() const { return m_CurrentProject.has_value(); }

	private:

		std::vector<Project> m_Projects;
		std::optional<Project> m_CurrentProject = std::nullopt;

	};
}