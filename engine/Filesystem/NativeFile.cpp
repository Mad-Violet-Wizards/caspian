#include "engine/pch.hpp"

#include "NativeFile.hpp"

#include <cereal/archives/json.hpp>


namespace fs
{
	NativeFile::NativeFile(std::string_view _path)
		: IFile(_path) 
	{ }

	NativeFile::~NativeFile()
	{
		Close();
	}

	bool NativeFile::Open(io::OpenMode _open_mode)
	{
		if (IsOpen() && m_OpenMode == _open_mode)
		{
			Seek(0, io::Origin::Begin);
			return true;
		}

		m_OpenMode = _open_mode;
		m_IsReadOnly = true;

		std::ios_base::openmode stream_open_mode = (std::ios_base::openmode) 0x00;

		if (_open_mode & io::OpenMode::In)
		{
			stream_open_mode |= std::ios_base::in;
		}
		if (_open_mode & io::OpenMode::Out)
		{
			m_IsReadOnly = false;
			stream_open_mode |= std::ios_base::out;
		}
		if (_open_mode & io::OpenMode::Append)
		{
			m_IsReadOnly = false;
			stream_open_mode |= std::ios_base::app;
		}
		if (_open_mode & io::OpenMode::Truncate)
		{
			stream_open_mode |= std::ios_base::trunc;
		}
		if (_open_mode & io::OpenMode::Binary)
		{
			stream_open_mode |= std::ios_base::binary;
		}

		m_FileStream.open(m_Path, stream_open_mode);
		return true;
	}

	void NativeFile::Close()
	{
		m_FileStream.close();
	}

	size_t NativeFile::Seek(size_t _offset, io::Origin _origin)
	{
		if (!IsOpen())
			return 0;

		std::ios_base::seekdir seek_dir = std::ios_base::beg;

		switch (_origin)
		{
			case io::Origin::Begin:
				seek_dir = std::ios_base::beg;
				break;
			case io::Origin::Current:
				seek_dir = std::ios_base::cur;
				break;
			case io::Origin::End:
				seek_dir = std::ios_base::end;
				break;
		}

		m_FileStream.seekg(_offset, seek_dir);
		m_FileStream.seekp(_offset, seek_dir);

		return Tell();
	}

	size_t NativeFile::Tell()
	{
		return m_FileStream.tellg();
	}

	size_t NativeFile::Size()
	{
		if (!IsOpen())
			return 0;

		auto current_pos = Tell();

		Seek(0, io::Origin::End);
		auto len = Tell();
		Seek(current_pos, io::Origin::Begin);

		return len;
	}

	bool NativeFile::IsEOF() const
	{
		return m_FileStream.eof();
	}

	bool NativeFile::IsOpen() const
	{
		return m_FileStream.is_open();
	}

	IFile::EType NativeFile::GetType() const
	{
		std::filesystem::path p{ m_Path };
		return StringExtToType(p.extension().string());
	}

	size_t NativeFile::Read_Impl(std::vector<uint8_t>& _buffer)
	{
		if (!IsOpen())
			return 0;

		m_FileStream.read(reinterpret_cast<char*>(_buffer.data()), static_cast<std::streamsize>(_buffer.size()));

		if (m_FileStream)
			return _buffer.size();

		return m_FileStream.gcount();
	}

	size_t NativeFile::Write_Impl(const std::vector<uint8_t>& _buffer)
	{
		if (!IsOpen() || IsReadOnly())
			return 0;

		m_FileStream.write(reinterpret_cast<const char*>(_buffer.data()), static_cast<std::streamsize>(_buffer.size()));

		if (m_FileStream)
			return _buffer.size();

		return m_FileStream.gcount();
	}

	size_t NativeFile::Read_UnsignedBuffer_Impl(std::vector<uint8_t>& _buffer)
	{
		if (!IsOpen())
			return 0;

		std::vector<uint8_t> buff( (std::istreambuf_iterator<char>(m_FileStream)), std::istreambuf_iterator<char>() );
		_buffer = std::move(buff);

		return _buffer.size();
	}

	bool NativeFile::ReadJson_Impl(nlohmann::json& _json, [[maybe_unused]] size_t _size)
	{
		if (!IsOpen())
			return false;

		m_FileStream >> _json;

		if (m_FileStream.good())
			return true;
	}

	bool NativeFile::WriteJson_Impl(const nlohmann::json& _json, [[maybe_unused]] size_t _size)
	{
		if (!IsOpen() || IsReadOnly())
			return false;

		m_FileStream << _json;

		if (m_FileStream.good())
			return true;
	}

	void NativeFile::DeserializeJson(std::shared_ptr<ISerializable::JSON>& _json)
	{
		cereal::JSONInputArchive archive(m_FileStream);
		archive(_json);
	}

	void NativeFile::SerializeJson(const std::shared_ptr<ISerializable::JSON>& _json)
	{
			cereal::JSONOutputArchive archive(m_FileStream);
			archive(_json);
	}
}