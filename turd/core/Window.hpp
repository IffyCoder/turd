#pragma once

namespace turd
{
    class RenderSystem;

    class Window
    {
        friend class RenderSystem;

      public:
        Window(HINSTANCE hInstance);
        ~Window();

        void Show();

        LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

      private:
        HWND mHandle;
        HINSTANCE mInstance;
    };
} // namespace turd
