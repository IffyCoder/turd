#pragma once

#if !defined(NDEBUG) && !defined(_DEBUG)
#define _DEBUG 1
#endif

#define NODISCARD [[nodiscard]]

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdarg>
#include <cstdint>
#include <array>
#include <chrono>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <future>
#include <algorithm>
#include <any>
#include <shared_mutex>
#include <typeinfo>
#include <type_traits>
#include <sol/sol.hpp>

#include <boost/signals2.hpp>

#include <wrl.h>
#include <dxgi1_6.h>
#include "d3dx12.h"
#include <DirectXMath.h>
#include <DirectXTex.h>

#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "btBulletCollisionCommon.h"

#include "sqlite3.h"
#include "core/Utils.hpp"
#include "core/Log.hpp"

namespace turd
{
    using Microsoft::WRL::ComPtr;

    namespace fs = std::filesystem;

    class RuntimeError : public std::exception
    {
      public:
        explicit RuntimeError(const std::string &msg) : std::exception() {}

        ~RuntimeError() override = default;
    };

    class Scoped
    {
      public:
        explicit Scoped(std::function<void()> func) : mFunc(std::move(func)) {}

        ~Scoped() { mFunc(); }

      private:
        std::function<void()> mFunc;
    };

    static std::string format(const char *format, ...)
    {
        char buffer[8192];
        va_list args;

        va_start(args, format);
        vsnprintf_s(buffer, sizeof(buffer), format, args);
        va_end(args);

        return buffer;
    }

    template <typename T, class Fn>
    Fn ForEach(T &v, Fn f)
    {
        auto first = std::begin(v);
        auto last = std::end(v);
        for (; first != last; ++first)
        {
            f(*first);
        }
        return f;
    }

    struct Size
    {
        uint32_t Width;
        uint32_t Height;
    };
} // namespace turd
