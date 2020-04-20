#include "pch.hpp"
#include "EventBus.hpp"

namespace turd
{
    void EventBus::OnResize(std::function<void(ResizeEvent)> f) { mResizeHandlers.push_back(f); }
    void EventBus::Resize(ResizeEvent ev)
    {
        std::for_each(std::begin(mResizeHandlers), std::end(mResizeHandlers), [&](auto &h) { h(ev); });
    }
} // namespace turd
