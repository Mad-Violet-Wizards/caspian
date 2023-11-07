#pragma once

#include <iostream>

#include "engine/Filesystem/IFile.hpp"

namespace Assets
{
	enum class EResourceType
	{
		Texture,
		Font,
		Unknown
	};



	template<typename T>
	concept ResourceAcceptableType = requires(T type)
	{
		{
			std::is_same<T, sf::Texture>::value ||
			std::is_same<T, sf::Font>::value
		};
	};

	template <ResourceAcceptableType T>
	class Resource
	{
		public:

			Resource() = default;
			~Resource() = default;

			void LoadFromFile(const std::string& _path)
			{
				if (!m_resource.loadFromFile(_path))
				{
					std::cout << "Failed to load resource from file: " << _path << "\n";
				}
			}

			void LoadFromData(const std::vector<uint8_t> _data)
			{
				if (!m_resource.loadFromMemory(_data.data(), _data.size()))
				{
					std::cout << "Failed to load resource from data\n";
				}
			}

			T& GetResource()
			{
				return m_resource;
			}

			EResourceType GetType() const
			{
				switch (T)
				{
					case sf::Texture:
						return EResourceType::Texture;
					case sf::Font:
						return EResourceType::Font;
					default:
						return EResourceType::Unknown;
				}
			}

		private:
			
			T m_resource;
	};

	class Storage
	{
		public:

			Storage() = default;
			~Storage() = default;

			void LoadTextureFsFilesBatch(const std::vector<fs::IFile*>& _files);
			void LoadFontFsFilesBatch(const std::vector<fs::IFile*>& _files);

			sf::Texture& GetTexture(const std::string& _path);
			sf::Font& GetFont(const std::string& _path);

			size_t GetTexturesCount() const { return m_textures.size(); }
			size_t GetFontsCount() const { return m_fonts.size(); }

			size_t GetTotalResourcesCount() const { return m_textures.size() + m_fonts.size(); }

			bool IsTexture(const std::string& _cmp) const;
			bool IsFont(const std::string& _cmp) const;

			[[nodiscard]] std::vector<std::string> GetTexturesKeys() const;
			[[nodiscard]] std::vector<std::string> GetFontKeys() const;
 
		private:

			std::unordered_map<std::string, Resource<sf::Texture>> m_textures;
			std::unordered_map<std::string, Resource<sf::Font>> m_fonts;

			mutable sf::Mutex m_Mutex;
	};
}