#pragma once

namespace turd
{
    class Timer
    {
        typedef std::chrono::high_resolution_clock::time_point TimePoint;
        typedef std::chrono::high_resolution_clock Clock;
        typedef std::chrono::duration<float> Duration;

      public:
        Timer() = default;
        ~Timer() = default;

        void Reset();

        float DeltaTime();

        void Tick();

      private:
        TimePoint mLastTime;
        TimePoint mCurrTime;
        Duration mDeltaTime;
    };
} // namespace turd
