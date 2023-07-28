#include "engine/pch.hpp"

#include "ImGuiValidators.hpp"

#include <regex>

namespace Tools
{
	namespace validate_input
	{
		bool is_valid_project_name(std::string_view _project_name)
		{
			const std::string& project_name = std::string(_project_name);

			if (project_name.empty())
				return false;

			const std::regex reg("^[A-Za-z0-9_-]+$");
			return std::regex_match(project_name, reg);
		}

		bool is_valid_project_path(std::string_view _project_path)
		{
			const std::string& project_path = std::string(_project_path);

			if (project_path.empty())
				return false;

			if (!project_path.starts_with("caspian/") || !project_path.starts_with("caspian\\"))
				return false;

			if (!project_path.find("projects"))
				return false;
		}
	};
};