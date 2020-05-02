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
            D3D12_SHADER_TYPE_DESC typeDesc;

            auto varReflect = reflect->GetVariableByIndex(i);
            auto typeReflect = varReflect->GetType();

            varReflect->GetDesc(&varDesc);
            typeReflect->GetDesc(&typeDesc);

            mVariableNames.Add(varDesc.Name);
            mVariables[varDesc.Name] = {varDesc, typeDesc};
        }
    }

    ConstantBuffer::~ConstantBuffer() { delete[] mPtr; }

    std::string ConstantBuffer::Name() const { return mName; }

    void ConstantBuffer::Set(const std::string &varName, void *ptr, size_t size)
    {
        if (mVariables.Contains(varName))
        {
            auto &var = mVariables[varName].VariableDesc;
            if (var.StartOffset + size <= mSize)
            {
                ::memcpy(mPtr + var.StartOffset, ptr, size);
                mModified = true;
            }
        }
    }

    uint32_t ConstantBuffer::Size() const { return mSize; }

    uint8_t *ConstantBuffer::Ptr() const { return mPtr; }

    Vector<ShaderVariable> ConstantBuffer::Variables()
    {
        Vector<ShaderVariable> vars = {};
        auto it = mVariables.cbegin();
        for (;it != mVariables.cend(); ++it)
        {
            vars.Add(it->second);
        }
        return vars;
    }
} // namespace turd
