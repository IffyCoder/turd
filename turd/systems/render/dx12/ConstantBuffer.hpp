#pragma once

#include <d3d12shader.h>

namespace turd
{
    class Pipeline;
    struct ShaderVariable
    {
        D3D12_SHADER_VARIABLE_DESC VariableDesc;
        D3D12_SHADER_TYPE_DESC TypeDesc;
    };
    class ConstantBuffer
    {
        friend class Pipeline;

      public:
        struct Variable
        {
            std::string Name;
            uint32_t Size;
        };

        explicit ConstantBuffer(ID3D12ShaderReflectionConstantBuffer *reflect);
        ~ConstantBuffer();

        std::string Name() const;

        void Set(const std::string &var, void *ptr, size_t size);

        uint32_t Size() const;

        uint8_t *Ptr() const;

        Vector<ShaderVariable> Variables();

      private:
        std::string mName;
        uint32_t mSize;
        uint8_t *mPtr;

        Vector<std::string> mVariableNames;
        Map<std::string, ShaderVariable> mVariables;
        std::atomic<bool> mModified = false;
    };
} // namespace turd
