#pragma once

namespace turd
{
    class Pipeline;

    class PipelineManager
    {
      public:
        PipelineManager();
        ~PipelineManager() = default;

        void Reload();

        Pipeline *Get(const std::string &name);
      private:
        std::map<std::string, std::unique_ptr<Pipeline>> mPipelines;
    };
}
