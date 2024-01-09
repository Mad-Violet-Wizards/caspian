#include "engine/pch.hpp"

#include "tests/catch.hpp"

#include "engine/Filesystem/IFile.hpp"

namespace mock
{
	class MockFile final : public fs::IFile
	{
	public:
		MockFile(std::string_view _path) : IFile(_path) { }

		size_t Write_Impl(const std::vector<uint8_t>& _buffer) override
		{
			m_Data = _buffer;
			return _buffer.size();
		}

		size_t Read_Impl(std::vector<uint8_t>& _buffer) override
		{
			_buffer = std::vector<uint8_t>(m_Data.cbegin(), m_Data.cend());
			return _buffer.size();
		}

		size_t Read_UnsignedBuffer_Impl(std::vector<uint8_t>& _buffer) override { return 0; }

		bool ReadJson_Impl(nlohmann::json& _json, size_t _size) override { return false; }
		bool WriteJson_Impl(const nlohmann::json& _json, size_t _size) override { return false; }

		void DeserializeJson(std::shared_ptr<ISerializable::JSON>& _json) override {}
		void SerializeJson(const std::shared_ptr<ISerializable::JSON>& _json) override {}

		void DeserializeBinary(std::shared_ptr<ISerializable::Binary>& _binary) override {}
		void SerializeBinary(std::shared_ptr<ISerializable::Binary> _binary) override {}

		bool IsOpen() const override { return true; }
		void Close() override { }
		bool Open(fs::io::OpenMode _open_mode) override { return true; }
		size_t Seek(size_t _offset, fs::io::Origin _origin) override { return 0;  }
		size_t Tell() override { return 0; }
		size_t Size() override { return m_Data.size(); }
		bool IsEOF() const override { return true; }
		bool IsReadOnly() const override { return false; }
		EType GetType() const override { return EType::Unknown; }


	public:

		std::vector<uint8_t> m_Data;
	};
}

TEST_CASE("IFile - Read and Write.", "[IFilesystemTests]")
{
	auto* tmp_file = new mock::MockFile("temp.txt");

	const char data[] = "Ala ma kota";
	bool write_res = tmp_file->Write(data, sizeof(data));
	REQUIRE(write_res == true);
	REQUIRE(tmp_file->m_Data.size() == 12);

	// Reading.
	char read_data[12];
	bool read_res = tmp_file->Read(read_data, 12);
	REQUIRE(read_res == true);
	REQUIRE(std::memcmp(read_data, data, 12) == 0);

	delete tmp_file;
}