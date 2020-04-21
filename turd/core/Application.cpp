#include "pch.hpp"
#include "Application.hpp"

#include "Environment.hpp"
#include "Window.hpp"
#include "Timer.hpp"

namespace turd
{
    Application::Application(Window *window) : mWindow(window) { mTimer = std::make_unique<Timer>(); }

    Application::~Application() {}

    int Application::Run()
    {
        Environment &env = GetEnvironment();

        env.gInputSystem->OnKey(VK_ESCAPE, [&]() { PostQuitMessage(0); });

        std::vector<ISystem *> updateOrder = {env.gInputSystem, /*env.gScriptSystem,*/ env.gRenderSystem};

        D("Running systems in following order");
        std::for_each(std::begin(updateOrder), std::end(updateOrder), [](auto sys) {
            if (sys)
            {
                D(sys->Name().c_str());
            }
        });

        // Do an update before showing the window
        // to fill the pipeline avoiding possible garbage
        env.gRenderSystem->Update(0.0f);

        mWindow->Show();

        while (true)
        {
            MSG msg = {0};
            while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    return 0;
                }

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            mTimer->Tick();

            std::for_each(std::begin(updateOrder), std::end(updateOrder), [&](auto sys) {
                if (sys)
                {
                    sys->Update(mTimer->DeltaTime());
                }
            });
        }
        return 0;
    }
} // namespace turd
