#pragma once

#include <vendor/include/nlohmann/json.hpp>
#include "engine/Core/Serializable/Serializable.hpp"

namespace fs
{
	namespace io
	{
		/////////////////////////////////////////////////////////
		enum class OpenMode
		{
			None = 0x00,
			In = 0x01,
			Out = 0x02,
			ReadWrite = In | Out,
			Append = 0x04,
			Truncate = 0x08,
			Binary = 0x10
		};

		inline OpenMode operator|(OpenMode _lhs, OpenMode _rhs)
		{
			return static_cast<OpenMode>(static_cast<int>(_lhs) | static_cast<int>(_rhs));
		}

		inline int operator&(OpenMode _lhs, OpenMode _rhs)
		{
			return static_cast<int>(_lhs) & static_cast<int>(_rhs);
		}

		/////////////////////////////////////////////////////////
		enum class Origin
		{
			Begin,
			Current,
			End
		};
	};



	/////////////////////////////////////////////////////////
	class IFile
	{
		public:

			enum class EType
			{
				Unknown,
				Texture,
				Font,
				Audio,
				Shader,
				Lua,
				JSON,
				Text,
				Data,
					Data_LevelRootChunk,
					Data_LevelChunk,
					Data_Tilemaps,
				Directory
			};

			static const char* S_UNKNOWN_ETYPE_STR;
			static const char* TypeToStringExt(EType _type)
			{
				switch(_type)
				{
					case EType::Texture:   return ".png";
					case EType::Font:      return ".ttf";
					case EType::Audio:     return ".wav";
					case EType::Shader:    return ".glsl";
					case EType::Lua:       return ".lua";
					case EType::JSON:      return ".json";
					case EType::Text:      return ".txt";
					case EType::Data:      return ".pak";
					case EType::Data_LevelRootChunk: return ".rootchunk";
					case EType::Data_LevelChunk:		 return ".chunk";
					case EType::Data_Tilemaps:			 return ".tilemaps";
					default:												 return S_UNKNOWN_ETYPE_STR;
				}
			}

			static EType StringExtToType(const std::string_view _type)
			{
				if (_type == ".png")		   return EType::Texture;
				if (_type == ".ttf")		   return EType::Font;
				if (_type == ".wav")		   return EType::Audio;
				if (_type == ".glsl")		   return EType::Shader;
				if (_type == ".lua")		   return EType::Lua;
				if (_type == ".json")		   return EType::JSON;
				if (_type == ".txt")			 return EType::Text;
				if (_type == ".pak")			 return EType::Data;
				if (_type == ".rootchunk") return EType::Data_LevelRootChunk;
				if (_type == ".chunk")		 return EType::Data_LevelChunk;
				if (_type == ".tilemaps") 	 return EType::Data_Tilemaps;
				return EType::Unknown;
			}

			static EType ETypeFromString(const std::string_view _str)
			{
				if (_str == "Texture")							return EType::Texture;
				if (_str == "Font")									return EType::Font;
				if (_str == "Audio")								return EType::Audio;
				if (_str == "Shader")								return EType::Shader;
				if (_str == "Lua")									return EType::Lua;
				if (_str == "JSON")									return EType::JSON;
				if (_str == "Text")									return EType::Text;
				if (_str == "Data")									return EType::Data;
				if (_str == "Directory")						return EType::Directory;
				if (_str == "Data_LevelRootChunk")	return EType::Data_LevelRootChunk;
				if (_str == "Data_LevelChunk")			return EType::Data_LevelChunk;
				if (_str == "Tilemaps")							return EType::Data_Tilemaps;
				return EType::Unknown;
			}

			static bool IsBinary(EType _type)
			{
				switch (_type)
				{
					case EType::Data:
					case EType::Data_LevelRootChunk:
					case EType::Data_LevelChunk:
					case EType::Data_Tilemaps:
						return true;
					default:
						return false;
				}
			}

		public:
			
			IFile(std::string_view _path);
			virtual ~IFile() = default;

			virtual bool Open(io::OpenMode _open_mode) = 0;
			virtual void Close() = 0;

			virtual size_t Seek(size_t _offset, io::Origin _origin) = 0;
			virtual size_t Tell() = 0;
			virtual size_t Size() = 0;
			virtual bool IsEOF() const = 0;

			virtual bool IsOpen() const = 0;
			virtual bool IsReadOnly() const { return m_IsReadOnly; };

			virtual std::string_view GetPath() const { return m_Path; };
			virtual EType GetType() const = 0;

			
			// TODO: Think of a better way than const & to shared-ptr.
			virtual void DeserializeJson(std::shared_ptr<ISerializable::JSON>& _json) = 0;
			virtual void SerializeJson(const std::shared_ptr<ISerializable::JSON>& _json) = 0;

			virtual void DeserializeBinary(std::shared_ptr<ISerializable::Binary>& _binary) = 0;
			virtual void SerializeBinary(const std::shared_ptr<ISerializable::Binary>& _binary) = 0;

			template<typename T>
			bool Read(T& _target, size_t _size)
			{
				static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

				if (!IsOpen())
					return false;

				std::vector<uint8_t> buffer;
				buffer.resize(_size);

				size_t read_bytes = Read_Impl(buffer);

				if (read_bytes != _size)
					return false;

				std::memcpy(_target, buffer.data(), _size);
				return true;
			}

			template<>
			bool Read(nlohmann::json& _json, size_t _size)
			{
				if (!IsOpen())
					return false;

				bool success = ReadJson_Impl(_json, _size);

				return success;
			}

			template<>
			bool Read(std::vector<uint8_t>& _buffer, size_t _size)
			{
				if (!IsOpen())
					return false;

				_buffer.reserve(_size);
				size_t read_bytes = Read_UnsignedBuffer_Impl(_buffer);

				return read_bytes == _size;
			}

			/////////////////////////////////////////////////////////
			template<typename T>
			bool Write(const T& _source, size_t _size)
			{
				if (!IsOpen() || IsReadOnly())
					return false;

				std::vector<uint8_t> buffer;
				buffer.resize(_size);
				std::memcpy(buffer.data(), _source, _size);

				size_t written_bytes = Write_Impl(buffer);
				return written_bytes == _size;
			}

			template<>
			bool Write(const nlohmann::json& json, size_t _size)
			{
				if (!IsOpen() || IsReadOnly())
					return false;

				bool success = WriteJson_Impl(json, _size);

				return success;
			}


		protected:

			virtual size_t Read_Impl(std::vector<uint8_t>& _buffer) = 0;
			virtual size_t Write_Impl(const std::vector<uint8_t>& _buffer) = 0;

			virtual size_t Read_UnsignedBuffer_Impl(std::vector<uint8_t>& _buffer) = 0;

			[[deprecated("Use DeserializeJson instead. To be removed in later versions.")]]
			virtual bool ReadJson_Impl(nlohmann::json& _json,  [[maybe_unused]] size_t _size) = 0;

			[[deprecated("Use SerializeJson instead. To be removed in later versions.")]]
			virtual bool WriteJson_Impl(const nlohmann::json& _json, [[maybe_unused]] size_t _size) = 0;


			bool m_IsReadOnly;
			io::OpenMode m_OpenMode;
			EType m_Type;

			std::string m_Path;
	};
}