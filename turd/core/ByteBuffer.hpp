#pragma once

namespace turd
{
    /*!
     * Wraps a bog standard array of bytes.
     *
     */
    class ByteBuffer
    {
      public:
        ByteBuffer(uint32_t size);
        ByteBuffer(uint8_t *buffer, uint32_t size);
        ~ByteBuffer();

        /*!
         * Reads bytes from this buffer and into the provided array.
         *
         * \param destionation Destination array.
         * \param size Number of bytes to read.
         * \return The actual number of bytes read, possibly less than requested.
         */
        uint32_t Read(uint8_t *destination, uint32_t size);

        /*!
         * Sets the contents of this buffer. The buffer will
         * be resized accordingly.
         *
         * \param ptr Pointer to memory.
         * \param size Number of bytes.
         * \param copy If true, the source memory is copied. Otherwise
         *             this buffer accepts ownership of the memory.
         */
        void Set(uint8_t *p, uint32_t size, bool copy = false);

        /*!
         * \return The size of this buffer.
         */
        uint32_t Size() const;

        /*!
         * \return A pointer to the memory in this buffer.
         */
        uint8_t *Pointer() const;

        /*!
         * \return The contents of this buffer as a string.
         */
        std::string ToString();

        /*!
         *
         */
        std::vector<uint8_t> ToVector();

        /*!
         * Moves the current read position forward n bytes.
         *
         * \param numBytes Number of bytes to skip.
         */
        void Skip(uint32_t numBytes);

        /*!
         * \return The current number of bytes available to read.
         */
        uint32_t Available() const;

        /*!
         * Check if it is possible to read certain amount of bytes.
         *
         * \param numBytes Number of bytes we want to read.
         * \return True if it is possible to read the requested amount of bytes, false otherwise.
         */
        bool CanRead(uint32_t numBytes);

        /*!
         * Resets the read position.
         */
        void Reset();

        /*!
         * Allocates memory.
         *
         * \param size Size of buffer.
         * \return Pointer to buffer.
         */
        static std::unique_ptr<ByteBuffer> Allocate(uint32_t size);

        /*!
         * Wraps previously allocated memory.
         *
         * \param ptr Pointer to memory.
         * \param size Size of memory.
         * \return Pointer to buffer.
         */
        static std::unique_ptr<ByteBuffer> Of(uint8_t *ptr, uint32_t size);

        /*!
         * Creates an empty buffer.
         * \return Pointer to buffer.
         */
        static std::unique_ptr<ByteBuffer> Empty();

      private:
        uint8_t *mBuffer;
        uint32_t mSize;
        uint32_t mPosition;
    };
} // namespace turd
