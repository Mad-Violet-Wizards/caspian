#include "engine/pch.hpp"

#include "IFile.hpp"


namespace fs
{
	const char* IFile::S_UNKNOWN_ETYPE_STR = "INVALID_ETYPE_STRING";

	IFile::IFile(std::string_view _path)
		: m_Path(_path)
		, m_IsReadOnly(false)
		, m_Type(IFile::EType::Unknown)
		, m_OpenMode(io::OpenMode::None)
	{

	}

};