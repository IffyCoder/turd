#pragma once

#include "Settings.hpp"
#include "managers/EntityManager.hpp"
#include "managers/PipelineManager.hpp"
#include "systems/input/InputSystem.hpp"
#include "systems/render/RenderSystem.hpp"
#include "EventBus.hpp"

namespace turd
{
    struct Environment
    {
        RenderSystem *gRenderSystem;
        InputSystem *gInputSystem;
        EntityManager *gEntityManager;
        PipelineManager *gPipelineManager;
        EventBus *gEventBus;
    };

    Environment &GetEnvironment();
} // namespace turd
