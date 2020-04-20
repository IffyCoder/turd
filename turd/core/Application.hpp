#pragma once

namespace turd
{
    class Window;
    class Timer;

    class Application
    {
      public:
        Application(Window *window);
        ~Application();

        int Run();

      private:
        Window *mWindow = nullptr;
        std::unique_ptr<Timer> mTimer;
    };
} // namespace turd
