#pragma once

namespace turd
{
    struct ResizeEvent
    {
        uint32_t Width;
        uint32_t Height;

        ResizeEvent(uint32_t w, uint32_t h) : Width(w), Height(h) {}
    };

    class EventBus
    {
      public:
        EventBus() {};
        ~EventBus() {};

        void OnResize(std::function<void(ResizeEvent)> f);

        void Resize(ResizeEvent ev);

      private:
        std::vector<std::function<void(ResizeEvent)>> mResizeHandlers = {};
    };

} // namespace turd
