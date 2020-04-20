#include "pch.hpp"
#include "Environment.hpp"

namespace turd
{
    Environment &GetEnvironment()
    {
        static Environment gEnvironment;
        return gEnvironment;
    }
}
