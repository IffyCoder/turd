#pragma once

namespace turd
{
    enum class LogLevel
    {
        DBG = 0,
        WARN,
        INFO,
        ERR
    };

    class Log
    {
      private:
        Log() = default;

      public:
        ~Log() = default;

        static void Write(LogLevel level, const char *format, ...);
    };

#define E(str, ...) Log::Write(LogLevel::ERR, str, ##__VA_ARGS__)
#define D(str, ...) Log::Write(LogLevel::DBG, str, ##__VA_ARGS__)
#define I(str, ...) Log::Write(LogLevel::INFO, str, ##__VA_ARGS__)
#define W(str, ...) Log::Write(LogLevel::WARN, str, ##__VA_ARGS__)
} // namespace turd
