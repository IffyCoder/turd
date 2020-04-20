#pragma once

namespace turd
{
    class ByteBuffer;

    class IO
    {
    private:
        IO() = default;
    public:
        ~IO() = default;

        static std::unique_ptr<ByteBuffer> File(const std::string &path);
    };
}

