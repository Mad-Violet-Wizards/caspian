#include "engine/pch.hpp"

#include "EngineModule.hpp"

std::string EngineModule::GetEngineVersionString()
{
	const std::array<unsigned char, 4> engine_version_bytes = GetEngineVersion();

	// Convert the version array to a string.
}