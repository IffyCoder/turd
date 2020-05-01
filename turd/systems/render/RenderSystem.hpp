#pragma once

#include "systems/ISystem.hpp"
#include "dx12/DX12State.hpp"

namespace turd
{
    class Window;
    class EntityManager;

    class RenderSystem : public ISystem
    {
      public:
        RenderSystem(Window *pWindow);
        ~RenderSystem() override;

        std::string Name() override;

        void Update(const float dt) override;

        DX12State *DX();

      private:
        Window *mWindow;
        std::unique_ptr<DX12State> mDX;
    };
} // namespace turd
