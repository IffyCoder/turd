#include "pch.hpp"
#include "PipelineManager.hpp"
#include "systems/render/dx12/Pipeline.hpp"
#include "systems/render/dx12/ConstantBuffer.hpp"
#include "core/Platform.hpp"

namespace turd
{
    PipelineManager::PipelineManager() { Reload(); }

    void PipelineManager::Reload()
    {
        mPipelines.clear();

        auto dir = DataDirectory() / "pipelines";
        for (auto &it : fs::directory_iterator(dir))
        {
            if (!it.is_regular_file())
            {
                continue;
            }

            auto p = it.path();
            if (p.extension() == ".yml")
            {
                try
                {
                    auto pipeline = std::make_unique<Pipeline>(p.string());
                    mPipelines[pipeline->Name()] = std::move(pipeline);
                }
                catch (const RuntimeError &err)
                {
                    E(err.what());
                }
            }
        }
    }

    Pipeline *PipelineManager::Get(const std::string &name)
    {
        if (CONTAINS(mPipelines, name))
        {
            return mPipelines[name].get();
        }
        return nullptr;
    }
} // namespace turd
