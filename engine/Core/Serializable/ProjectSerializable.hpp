#pragma once

#include "Serializable.hpp"
#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>

namespace Serializable
{
	namespace JSON
	{
		class ProjectInfo : public ISerializable::JSON
		{
		public:

			ProjectInfo() = default;
			ProjectInfo(const std::string& _project_name, const std::string& _project_path)
				: m_ProjectName{ _project_name }
				, m_ProjectPath{ _project_path }
			{
			}

			std::string m_ProjectName;
			std::string m_ProjectPath;

			void dummy() override {}

			template<class Archive>
			void serialize(Archive& ar)
			{
				ar(cereal::make_nvp("Name", m_ProjectName),
					cereal::make_nvp("Path", m_ProjectPath));
			}
		};

		class ProjectsInfo : public ISerializable::JSON
		{
		public:

			ProjectsInfo() = default;
			ProjectsInfo(const std::vector<ProjectInfo>& _projects)
				: m_Projects{ _projects }
			{
			}

			std::vector<ProjectInfo> m_Projects;

			void dummy() override {}

			template<class Archive>
			void serialize(Archive& ar)
			{
				ar(cereal::make_nvp("Projects", m_Projects));
			}
		};
	}

	namespace Binary
	{
		class MagicFileInfo : public ISerializable::Binary
		{
			public:
			
			MagicFileInfo() :
				m_MagicNumber{ 0x43, 0x41, 0x53, 0x50 }
			{}

			MagicFileInfo(const std::array<unsigned char, 4> engine_version)
				: m_EngineVersion{ engine_version }
				, m_MagicNumber{ 0x43, 0x41, 0x53, 0x50 }
			{

			}

			std::array<unsigned char, 4> m_EngineVersion;
			std::array<unsigned char, 4> m_MagicNumber;

			void dummy() override {}

			template<class Archive>
			void serialize(Archive& ar)
			{
				ar(m_EngineVersion, m_MagicNumber);
			}
		};
	}
}

using namespace Serializable::JSON;
using namespace Serializable::Binary;
using namespace ISerializable;

CEREAL_REGISTER_TYPE(ProjectInfo)
CEREAL_REGISTER_TYPE(ProjectsInfo)
CEREAL_REGISTER_TYPE(MagicFileInfo)

CEREAL_REGISTER_POLYMORPHIC_RELATION(JSON, ProjectInfo)
CEREAL_REGISTER_POLYMORPHIC_RELATION(JSON, ProjectsInfo)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Binary, MagicFileInfo)
