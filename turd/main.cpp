#include "pch.hpp"
#include "core/Environment.hpp"
#include "core/Application.hpp"
#include "core/Window.hpp"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR pCmdLine, _In_ int nCmdShow)
{
    auto &gSettings = turd::GetSettings();
    auto &gEnvironment = turd::GetEnvironment();
    
    auto pEventBus = std::make_unique<turd::EventBus>();
    gEnvironment.gEventBus = pEventBus.get();


    auto pWindow = std::make_unique<turd::Window>(hInstance);
    auto pEntityManager = std::make_unique<turd::EntityManager>();
    auto pRenderSystem = std::make_unique<turd::RenderSystem>(pWindow.get());
    
    
    gEnvironment.gEntityManager = pEntityManager.get();
    gEnvironment.gRenderSystem = pRenderSystem.get();
    
    auto pApplication = std::make_unique<turd::Application>(pWindow.get());

    return pApplication->Run();
}
