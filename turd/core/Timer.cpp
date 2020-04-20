#include "pch.hpp"
#include "Timer.hpp"

namespace turd
{
    void Timer::Reset() { mCurrTime = Clock::now(); }

    float Timer::DeltaTime() { return mDeltaTime.count(); }

    void Timer::Tick()
    {
        mCurrTime = Clock::now();
        mDeltaTime = (mCurrTime - mLastTime);
        mLastTime = mCurrTime;
    }
} // namespace turd
