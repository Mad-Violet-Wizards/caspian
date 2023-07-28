#pragma once

namespace Tools
{
	namespace validate_input
	{
		bool is_valid_project_name(std::string_view _project_name);
		bool is_valid_project_path(std::string_view _project_path);
	};
};