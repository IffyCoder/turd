#pragma once

#include "Settings.hpp"
#include "managers/EntityManager.hpp"
#include "systems/render/RenderSystem.hpp"
#include "EventBus.hpp"

namespace turd
{
    struct Environment
    {
        ISystem *gRenderSystem;
        EntityManager *gEntityManager;
        EventBus *gEventBus;
    };

    Environment &GetEnvironment();
} // namespace turd
