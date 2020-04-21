#include "InputSystem.hpp"

namespace turd
{
    void InputSystem::OnKey(int key, KeyHandler h) { mKeyHandlers[key] = h; }

    void InputSystem::OnMouseMove(MouseHandler h) { mMouseHandlers.push_back(h); }

    std::string InputSystem::Name() { return "InputSystem"; }

    void InputSystem::Update(const float dt)
    {
        std::for_each(std::begin(mKeyHandlers), std::end(mKeyHandlers), [&](auto &e) {
            auto state = GetAsyncKeyState(e.first);
            if ((state >> 0x0F) & 0x1)
            {
                e.second();
            }
        });

        POINT p;
        if (GetCursorPos(&p))
        {
            std::for_each(std::begin(mMouseHandlers), std::end(mMouseHandlers), [&](auto &h) { h(p.x, p.y); });
        }
    }
} // namespace turd
