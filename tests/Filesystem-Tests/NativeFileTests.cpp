#include "engine/pch.hpp"

#include "tests/catch.hpp"

#include "engine/Filesystem/NativeFile.hpp"
#include <filesystem>

TEST_CASE("NativeFile operations", "[NativeFile]")
{
	const std::string path = "test_file.txt";
	auto* tmp_file = new fs::NativeFile{ path };
	
	SECTION("File opening & Writing")
	{
		if (const bool open_res = tmp_file->Open(fs::io::OpenMode::Out))
		{
			bool is_file_open = tmp_file->IsOpen();

			REQUIRE(open_res == true);
			REQUIRE(is_file_open == true);

			const char data[] = "basic_data1";
			bool write_res = tmp_file->Write(data, sizeof(data));
			REQUIRE(write_res == true);
			tmp_file->Close();
		}
	}

	SECTION("File reading")
	{
		if (const bool open_res = tmp_file->Open(fs::io::OpenMode::In))
		{
			bool is_file_open = tmp_file->IsOpen();

			REQUIRE(open_res == true);
			REQUIRE(is_file_open == true);

			char data[12];
			bool read_res = tmp_file->Read(data, sizeof(data));
			REQUIRE(read_res == true);
			tmp_file->Close();
		}
	}
	
	SECTION("Cleanup")
	{
		std::filesystem::remove(path);
		delete tmp_file;
	}
}
