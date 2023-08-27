#include "engine/pch.hpp"

#include "tests/catch.hpp"

#include "engine/Filesystem/NativeFileSystem.hpp"
#include "engine/Filesystem/IFile.hpp"

const char* moc_native_path = ".\\tests\\Filesystem-Tests\\moc_native_path";

TEST_CASE("Initialization and shutdown", "[NativeFilesystem]")
{
	 fs::NativeFileSystem test_fs{ moc_native_path };

	 SECTION("Initialization of Native FS")
	 {
		 REQUIRE(test_fs.IsInitialized() == false);

		 test_fs.Initialize();

		 REQUIRE(test_fs.IsInitialized() == true);
	 }

	 SECTION("Shutdown of Native FS")
	 {
		 test_fs.Initialize();
		 REQUIRE(test_fs.Shutdown() == true);
	 }
}

TEST_CASE("File ops", "[NativeFilesystem]")
{
	fs::NativeFileSystem *test_fs = new fs::NativeFileSystem{ moc_native_path };
	test_fs->Initialize();

	SECTION("Open file that does not exists.")
	{
		REQUIRE(test_fs->OpenFile("does_not_exists.txt", fs::io::OpenMode::In) == nullptr);
	}

	SECTION("Create and remove file")
	{
		std::string_view tmp_file = "tmp_file.txt";

		REQUIRE(test_fs->CreateFile(tmp_file, fs::IFile::EType::Text) == true);
		REQUIRE(test_fs->FileExists(tmp_file) == true);
		REQUIRE(test_fs->RemoveFile(tmp_file) == true);
	}

	SECTION("Writing to file via FS.")
	{
		std::string_view tmp_data_file = "tmp_data_file.txt";
		
		test_fs->CreateFile(tmp_data_file, fs::IFile::EType::Text);

		auto tmp_file = test_fs->OpenFile(tmp_data_file, fs::io::OpenMode::Out);
		REQUIRE(tmp_file != nullptr);

		REQUIRE(tmp_file->IsOpen() == true);

		const char data[] = "writing_via_fs";

		bool write_res = tmp_file->Write(data, sizeof(data));
		REQUIRE(write_res == true);
		REQUIRE(test_fs->CloseFile(tmp_file) == true);
	}

	SECTION("Reading from file via FS.")
	{
		std::string_view tmp_data_file = "tmp_data_file.txt";
		auto tmp_file = test_fs->OpenFile(tmp_data_file, fs::io::OpenMode::In);

		REQUIRE(tmp_file != nullptr);
		char data_in[15];
		bool read_res = tmp_file->Read(data_in, sizeof(data_in));
		REQUIRE(read_res == true);
		test_fs->CloseFile(tmp_file);
	}

	SECTION("Copy & rename file.")
	{
		std::string_view tmp_data_file = "tmp_data_file.txt";
		std::string_view tmp_data_file_copy = "dir1\\tmp_data_file_copy.txt";

		REQUIRE(test_fs->CopyFile(tmp_data_file, tmp_data_file_copy) == true);
		REQUIRE(test_fs->RenameFile(tmp_data_file_copy, "dir1\\tmp_data_file_copy_renamed.txt") == true);
	}

	SECTION("Cleanup")
	{
		test_fs->RemoveFile("tmp_data_file.txt");
		test_fs->RemoveFile("dir1\\tmp_data_file_copy_renamed.txt");

		test_fs->Shutdown();
		delete test_fs;
	}
}
