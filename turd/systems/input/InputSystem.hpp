#pragma once

#include "systems/ISystem.hpp"

namespace turd
{
    class InputSystem : public ISystem
    {
      public:
        using KeyHandler = std::function<void()>;
        using MouseHandler = std::function<void(long, long)>;

        InputSystem() {}

        ~InputSystem() {}

        void OnKey(int key, KeyHandler h);
        void OnMouseMove(MouseHandler h);

        std::string Name() override;

        void Update(const float dt) override;

      private:
        std::map<int, KeyHandler> mKeyHandlers;
        std::vector<MouseHandler> mMouseHandlers;
    };

} // namespace turd
