#include "engine/pch.hpp"

#include "Validators.hpp"

#include <regex>

namespace Tools_Impl
{
	namespace validate_input
	{
		bool is_valid_project_name(std::string_view _project_name)
		{
			auto null_terminator_count = std::count(_project_name.begin(), _project_name.end(), '\0');
			auto project_name_size = _project_name.size();

			if (null_terminator_count == project_name_size)
				return false;

			const std::string& project_name = std::string(_project_name);

			// TODO: Those invalid chars are valid only on Windows.
			const std::string invalid_chars = "<>:\"/\\|?*";

			for (const char& c : invalid_chars)
			{
				if (project_name.find(c) != std::string::npos)
					return false;
			}

		}

		bool is_valid_project_path(std::string_view _project_path)
		{
			const std::string& project_path = std::string(_project_path);

			if (project_path.empty())
				return false;

			if (!project_path.starts_with("caspian\\"))
				return false;

			if (!project_path.find("projects"))
				return false;
		}
	};
};