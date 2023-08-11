#include "engine/pch.hpp"

#include "tests/catch.hpp"

#include "engine/Filesystem/NativeFile.hpp"

TEST_CASE("NativeFile operations", "[NativeFile]")
{
	const std::string path = "test_file.txt";
	auto* tmp_file = new fs::NativeFile{ path };
	
	if (tmp_file->Open(fs::io::OpenMode::Out))
	{
		bool is_file_open = tmp_file->IsOpen();

		REQUIRE(is_file_open == true);
	}

	//const char data[] = "abcdefg";
	//const bool write_res = tmp_file->Write(data);
	//REQUIRE(write_res == true);
}
