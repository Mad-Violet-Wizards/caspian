#pragma once

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
			Truncate = 0x08
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
	}

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
				Directory
			};

			static const char* S_UNKNOWN_ETYPE_STR;
			static const char* TypeToFileExtension(EType _type)
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
					default:							 return S_UNKNOWN_ETYPE_STR;
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

		protected:

			virtual size_t Read_Impl(std::vector<uint8_t>& _buffer) = 0;
			virtual size_t Write_Impl(const std::vector<uint8_t>& _buffer) = 0;

			bool m_IsReadOnly;
			io::OpenMode m_OpenMode;
			EType m_Type;

			std::string m_Path;
	};
}