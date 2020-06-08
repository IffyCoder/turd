#pragma once

namespace turd
{
    template <typename T>
    class UploadBuffer
    {
      public:
        UploadBuffer(ID3D12Device *pDevice, uint32_t elementCount, bool isConstantBuffer)
            : mIsConstantBuffer(isConstantBuffer)
        {
            mElementByteSize = sizeof(T);

            if (isConstantBuffer)
            {
                mElementByteSize = CalcConstantBufferByteSize(sizeof(T));
            }

            if (FAILED(pDevice->CreateCommittedResource(
                    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
                    &CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount), D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr, IID_PPV_ARGS(&mUploadBuffer))))
            {
                throw RuntimeError("Failed to create upload buffer");
            }

#ifdef _DEBUG
            mUploadBuffer->SetName(L"UploadBuffer");
#endif

            if (FAILED(mUploadBuffer->Map(0, nullptr, reinterpret_cast<void **>(&mMappedData))))
            {
                throw RuntimeError("Failed to map buffer data");
            }
        }

        ~UploadBuffer()
        {
            if (mUploadBuffer != nullptr)
            {
                mUploadBuffer->Unmap(0, nullptr);
            }

            mMappedData = nullptr;
        }

        UploadBuffer(const UploadBuffer &rhs) = delete;
        UploadBuffer &operator=(const UploadBuffer &rhs) = delete;

        ID3D12Resource *Resource() const { return mUploadBuffer.Get(); }

        void Set(int elementIndex, const T &data)
        {
            memcpy(&mMappedData[elementIndex * mElementByteSize], &data, sizeof(T));
        }

      private:
        uint32_t CalcConstantBufferByteSize(uint32_t byteSize) { return (byteSize + 255) & ~255; }
        ComPtr<ID3D12Resource> mUploadBuffer;
        bool mIsConstantBuffer;
        uint8_t *mMappedData;
        uint32_t mElementByteSize;
    };
} // namespace turd
