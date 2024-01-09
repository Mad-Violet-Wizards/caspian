#pragma once

#include "IFile.hpp"

#include <fstream>

#include <cereal/types/polymorphic.hpp>

namespace fs
{
	class NativeFile final : public IFile
	{
		public:

			NativeFile(std::string_view _path);
			~NativeFile();

			bool Open(io::OpenMode _open_mode) override;
			void Close() override;

			size_t Seek(size_t _offset, io::Origin _origin) override;;
			size_t Tell() override;
			size_t Size() override;
			bool IsEOF() const override;

			bool IsOpen() const override;
			EType GetType() const override;

		protected:

			size_t Read_Impl(std::vector<uint8_t>& _buffer) override;
			size_t Write_Impl(const std::vector<uint8_t>& _buffer) override;

			size_t Read_UnsignedBuffer_Impl(std::vector<uint8_t>& _buffer) override;

			bool ReadJson_Impl(nlohmann::json& _json, [[maybe_unused]] size_t _size) override;
			bool WriteJson_Impl(const nlohmann::json& _json, [[maybe_unused]] size_t _size) override;

			void DeserializeJson(std::shared_ptr<ISerializable::JSON>& _json) override;
			void SerializeJson(const std::shared_ptr<ISerializable::JSON>& _json) override;

			void DeserializeBinary(std::shared_ptr<ISerializable::Binary>& _binary) override {}
			void SerializeBinary(std::shared_ptr<ISerializable::Binary> _binary) override {}

		private:

			std::fstream m_FileStream;
	};
}
