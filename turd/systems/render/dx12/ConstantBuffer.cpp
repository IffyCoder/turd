#include "pch.hpp"
#include "ConstantBuffer.hpp"

namespace turd
{
    ConstantBuffer::ConstantBuffer(ID3D12ShaderReflectionConstantBuffer *reflect)
    {
        D3D12_SHADER_BUFFER_DESC bufferDesc;
        reflect->GetDesc(&bufferDesc);
        mName = bufferDesc.Name;

        mSize = bufferDesc.Size;
        mPtr = new uint8_t[mSize];

        memset(mPtr, 0, mSize);

        for (auto i = 0; i < bufferDesc.Variables; i++)
        {
            D3D12_SHADER_VARIABLE_DESC varDesc;
            auto varReflect = reflect->GetVariableByIndex(i);
            varReflect->GetDesc(&varDesc);

            mVariables[varDesc.Name] = varDesc;
        }
    }

    ConstantBuffer::~ConstantBuffer() { delete[] mPtr; }

    std::string ConstantBuffer::Name() const { return mName; }

    void ConstantBuffer::Set(const std::string &varName, void *ptr, size_t size)
    {
        if (mVariables.Contains(varName))
        {
            auto var = mVariables[varName];
            if (var.StartOffset + size <= mSize)
            {
                ::memcpy(mPtr + var.StartOffset, ptr, size);
            }
        }
    }

    uint32_t ConstantBuffer::Size() const { return mSize; }
    
    uint8_t *ConstantBuffer::Ptr() const { return mPtr; }
} // namespace turd
