#pragma once

#include "IFile.hpp"

#include <fstream>

namespace fs
{
	class NativeFile final : public IFile
	{
		public:

			NativeFile(std::string_view _path);
			~NativeFile();

			virtual bool Open(io::OpenMode _open_mode) override;
			virtual void Close() override;

			virtual size_t Seek(size_t _offset, io::Origin _origin) override;;
			virtual size_t Tell() override;
			virtual size_t Size() override;
			virtual bool IsEOF() const override;

			virtual bool IsOpen() const override;
			virtual EType GetType() const override;

		protected:

			virtual size_t Read_Impl(std::vector<uint8_t>& _buffer) override;
			virtual size_t Write_Impl(const std::vector<uint8_t>& _buffer) override;

			virtual size_t Read_UnsignedBuffer_Impl(std::vector<uint8_t>& _buffer) override;

			virtual bool ReadJson_Impl(nlohmann::json& _json, [[maybe_unused]] size_t _size) override;
			virtual bool WriteJson_Impl(const nlohmann::json& _json, [[maybe_unused]] size_t _size) override;

		private:

			std::fstream m_FileStream;
	};
}
