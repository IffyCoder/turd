#include "pch.hpp"
#include "ByteBuffer.hpp"

namespace turd
{
    ByteBuffer::ByteBuffer(uint32_t size) : mSize(size), mPosition(0)
    {
        if (mSize > 0)
        {
            mBuffer = new uint8_t[size];
            if (!mBuffer)
            {
                throw; // RuntimeError(L"Unable to allocate memory");
            }
        }
    }

    ByteBuffer::ByteBuffer(uint8_t *buffer, uint32_t size) : mBuffer(buffer), mSize(size), mPosition(0) {}

    ByteBuffer::~ByteBuffer()
    {
        if (mBuffer)
        {
            delete mBuffer;
            mSize = 0;
        }
    }

    uint32_t ByteBuffer::Read(uint8_t *destination, uint32_t size)
    {
        uint32_t bytesToRead = 0;

        if (Available() == 0)
        {
            return 0;
        }

        if (CanRead(size))
        {
            bytesToRead = size;
        }
        else
        {
            bytesToRead = Available();
        }

        memcpy(destination, mBuffer + mPosition, bytesToRead);

        mPosition += bytesToRead;
        return bytesToRead;
    }

    void ByteBuffer::Set(uint8_t *ptr, uint32_t size, bool copy)
    {
        if (mBuffer)
        {
            delete[] mBuffer;
        }

        mSize = size;

        if (copy)
        {
            mBuffer = new uint8_t[size];
            memcpy(mBuffer, ptr, size);
        }
        else
        {
            mBuffer = ptr;
        }
    }

    uint8_t *ByteBuffer::Pointer() const { return mBuffer; }

    uint32_t ByteBuffer::Size() const { return mSize; }

    void ByteBuffer::Skip(uint32_t numBytes)
    {
        if (mPosition + numBytes < mSize)
        {
            mPosition += numBytes;
        }
        else
        {
            mPosition = mSize;
        }
    }

    uint32_t ByteBuffer::Available() const { return mSize - mPosition; }

    bool ByteBuffer::CanRead(uint32_t numBytes) { return Available() >= numBytes; }

    std::string ByteBuffer::ToString()
    {
        char *ws = new char[mSize + 1];
        memset(ws, 0, mSize + 1);

        memcpy(ws, mBuffer, mSize);

        std::string ret = ws;
        delete[] ws;
        return ret;
    }

    std::vector<uint8_t> ByteBuffer::ToVector() { return std::vector<uint8_t>(mBuffer, mBuffer + mSize); }

    void ByteBuffer::Reset() { mPosition = 0; }

    std::unique_ptr<ByteBuffer> ByteBuffer::Allocate(uint32_t size) { return std::make_unique<ByteBuffer>(size); }

    std::unique_ptr<ByteBuffer> ByteBuffer::Of(uint8_t *p, uint32_t size)
    {
        return std::make_unique<ByteBuffer>(p, size);
    }

    std::unique_ptr<ByteBuffer> ByteBuffer::Empty() { return std::make_unique<ByteBuffer>(nullptr, 0); }
} // namespace turd
