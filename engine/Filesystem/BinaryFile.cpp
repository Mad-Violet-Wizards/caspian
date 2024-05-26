#include "engine/pch.hpp"

#include "BinaryFile.hpp"

#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/json.hpp>

namespace fs
{
	BinaryFile::BinaryFile(std::string_view _path)
		: IFile(_path)
	{ }

	BinaryFile::~BinaryFile()
	{
		Close();
	}

	bool BinaryFile::Open(io::OpenMode _open_mode)
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

	void BinaryFile::Close()
	{
		m_FileStream.close();
	}

	size_t BinaryFile::Seek(size_t _offset, io::Origin _origin)
	{
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
		return m_FileStream.tellg();
	}

	size_t BinaryFile::Tell()
	{
		return m_FileStream.tellg();
	}

	size_t BinaryFile::Size()
	{
		if (!IsOpen())
			return 0;

		auto current_pos = Tell();

		Seek(0, io::Origin::End);
		auto len = Tell();
		Seek(current_pos, io::Origin::Begin);

		return len;
	}

	bool BinaryFile::IsEOF() const
	{
		return m_FileStream.eof();
	}

	bool BinaryFile::IsOpen() const
	{
		return m_FileStream.is_open();
	}

	fs::IFile::EType BinaryFile::GetType() const
	{
		std::filesystem::path p{ m_Path };
		return StringExtToType(p.extension().string());
	}

	void BinaryFile::DeserializeJson(std::shared_ptr<ISerializable::JSON>& _json)
	{
		cereal::JSONInputArchive archive(m_FileStream);
		archive(_json);
	}

	void BinaryFile::SerializeJson(const std::shared_ptr<ISerializable::JSON>& _json)
	{
		cereal::JSONOutputArchive archive(m_FileStream);
		archive(_json);
	}

	void BinaryFile::DeserializeBinary(std::shared_ptr<ISerializable::Binary>& _binary)
	{
		cereal::PortableBinaryInputArchive archive(m_FileStream);
		archive(_binary);
	}

	void BinaryFile::SerializeBinary(const std::shared_ptr<ISerializable::Binary>& _binary)
	{
		cereal::PortableBinaryOutputArchive archive(m_FileStream);
		archive(_binary);
	}

	//bool BinaryFile::ReadBinary_CerealImpl(ISerializable::Binary& _binary_data, [[maybe_unused]] size_t _size)
	//{
	//	cereal::PortableBinaryInputArchive archive(m_FileStream);
	//	archive(_binary_data);
	//	return true;
	//}

	//bool BinaryFile::WriteBinary_CerealImpl(const ISerializable::Binary& _binary_data, [[maybe_unused]] size_t _size)
	//{
	//	cereal::PortableBinaryOutputArchive archive(m_FileStream);
	//	archive(_binary_data);
	//	return true;
	//}

}