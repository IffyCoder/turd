#include "pch.hpp"
#include "IO.hpp"
#include "ByteBuffer.hpp"
#include "Platform.hpp"

namespace turd
{
    std::unique_ptr<ByteBuffer> IO::File(const std::string &path)
    {
        fs::path root = DataDirectory();
        fs::path assetPath = root / path;

        FILE *fp;
        auto res = fopen_s(&fp, assetPath.string().c_str(), "rb");
        if (res != 0)
        {
            return ByteBuffer::Empty();
        }

        Scoped close([&fp] { fclose(fp); });

        auto size = fs::file_size(assetPath);
        auto ptr = new uint8_t[size];
        if (fread_s(ptr, size, 1, size, fp) != size)
        {
            return ByteBuffer::Empty();
        }

        return ByteBuffer::Of(ptr, uint32_t(size));
    }
} // namespace turd
