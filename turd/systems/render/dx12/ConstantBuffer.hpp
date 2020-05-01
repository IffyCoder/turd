#pragma once

#include <d3d12shader.h>

namespace turd
{
    class ConstantBuffer
    {
      public:
        explicit ConstantBuffer(ID3D12ShaderReflectionConstantBuffer *reflect);
        ~ConstantBuffer();

        std::string Name() const;

        void Set(const std::string &var, void *ptr, size_t size);

        uint32_t Size() const;

        uint8_t *Ptr() const;

      private:
        std::string mName;
        uint32_t mSize;
        uint8_t *mPtr;

        Map<std::string, D3D12_SHADER_VARIABLE_DESC> mVariables;
    };
} // namespace turd
