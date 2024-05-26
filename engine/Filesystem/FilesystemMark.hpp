#pragma once

namespace fs
{
	enum class EFilesystemMark
	{
		Level,
		Animation
	};

	inline std::string EFilesystemMarkToString(EFilesystemMark _mark)
	{
		switch (_mark)
		{
			case EFilesystemMark::Level: return "Level";
			default: return "Unknown";
		}
	}

	inline EFilesystemMark EFilesystemMarkFromString(std::string_view _mark)
	{
		if (_mark == "Level") return EFilesystemMark::Level;
		return EFilesystemMark::Level;
	}

	inline unsigned int EFilesystemMarkToHash(EFilesystemMark _mark)
	{
		switch (_mark)
		{
			case EFilesystemMark::Level: return 0x4c657665;
			default: return 0x00000000;
		}
	}

	inline EFilesystemMark EFilesystemMarkFromHash(unsigned int _hash)
	{
		switch (_hash)
		{
			case 0x4c657665: return EFilesystemMark::Level;
			default: return EFilesystemMark::Level;
		}
	}
}