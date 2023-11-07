#include "engine/pch.hpp"

#include "Assets.hpp"
#include <iostream>

void Assets::Storage::LoadTextureFsFilesBatch(const std::vector<fs::IFile*>& _files)
{
	for (auto& file : _files)
	{
		if (!file->IsOpen())
			continue;

		Resource<sf::Texture> texture;
		std::vector<uint8_t> data;

		auto file_size = file->Size();
		file->Seek(0, fs::io::Origin::Begin);

		if (file->Read(data, file_size))
		{
			texture.LoadFromData(data);

			// Trim path to get only filename.
			std::string_view path = file->GetPath();
			path = path.substr(path.find_last_of("\\/") + 1);

			sf::Lock lock(m_Mutex);
			m_textures[{path.cbegin(), path.cend()}] = texture;
		}
	}
}

void Assets::Storage::LoadFontFsFilesBatch(const std::vector<fs::IFile*>& _files)
{
	for (auto& file : _files)
	{
		if (!file->IsOpen())
			continue;

			Resource<sf::Font> font;
			std::vector<uint8_t> data;

			if (file->Read(data, file->Size()))
			{
				font.LoadFromData(data);

				// Trim path to get only filename.
				std::string_view path = file->GetPath();
				path = path.substr(path.find_last_of("\\/") + 1);

				sf::Lock lock(m_Mutex);
				m_fonts[{path.cbegin(), path.cend()}] = font;
			}
	}
}

sf::Texture& Assets::Storage::GetTexture(const std::string& _path)
{
	sf::Lock lock(m_Mutex);
	// Get from map if exists.
	if (m_textures.find(_path) != m_textures.end())
	{
		return m_textures[_path].GetResource();
	}

	// Report error if not exists.
	// Otherwise we'll prove default asset.
	std::cout << "ERROR: Texture " << _path << " not found.\n";
	throw std::runtime_error("Texture wasn't found.");
}

sf::Font& Assets::Storage::GetFont(const std::string& _path)
{
	sf::Lock lock(m_Mutex);

	if (m_fonts.find(_path) != m_fonts.end())
	{
		return m_fonts[_path].GetResource();
	}

	// Otherwise we'll report default asset.
	std::cout << "ERROR: Font " << _path << " not found.\n";
	throw std::runtime_error("Font wasn't found.");
}

bool Assets::Storage::IsTexture(const std::string& _cmp) const
{
	sf::Lock lock(m_Mutex);

	return m_textures.find(_cmp) != m_textures.end();

}

bool Assets::Storage::IsFont(const std::string& _cmp) const
{
	sf::Lock lock(m_Mutex);

	return m_fonts.find(_cmp) != m_fonts.end();
}

std::vector<std::string> Assets::Storage::GetTexturesKeys() const
{
	std::vector<std::string> keys;
	keys.reserve(m_textures.size());

	std::ranges::for_each(m_textures, [&keys](auto& pair)
	{
		keys.push_back(pair.first);
	});

	return keys;
}

std::vector<std::string> Assets::Storage::GetFontKeys() const
{
	std::vector<std::string> keys;
	keys.reserve(m_fonts.size());

	std::ranges::for_each(m_fonts, [&keys](auto& pair)
	{
		keys.push_back(pair.first);
	});

	return keys;
}
