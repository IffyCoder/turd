#pragma once

namespace turd
{
    class Settings
    {
      public:
        Settings();
        ~Settings();

        struct Renderer
        {
            uint32_t Samples = 0;
            bool VSync = true;
            bool Debug = true;
        };

        struct Window
        {
            uint32_t Width = 1024;
            uint32_t Height = 768;
            std::string Title = "turd";
            bool Maximized = false;
            bool Fullscreen = false;
        };

        Renderer &RendererSettings();
        Window &WindowSettings();

        void Save();

      private:
        Renderer mRendererSettings;
        Window mWindowSettings;
    };

    Settings &GetSettings();
} // namespace turd
