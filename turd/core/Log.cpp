#include "pch.hpp"
#include "Log.hpp"

namespace turd
{
    static std::string LevelToString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::ERR:
            return "ERR";
        default:
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARN:
            return "WARN";
        case LogLevel::DBG:
            return "DBG";
        }
    }

    void Log::Write(LogLevel level, const char *format, ...)
    {
#ifdef NDEBUG
        if (level == LogLevel::DBG)
        {
            return;
        }
#endif
        char buffer[8192];
        va_list args;

        va_start(args, format);
        vsnprintf_s(buffer, sizeof(buffer), format, args);
        va_end(args);

        std::stringstream sstrm;
        sstrm << LevelToString(level) << ": " << buffer;

        auto s = sstrm.str();
#ifdef _DEBUG
        OutputDebugStringA(s.c_str());
        OutputDebugStringA("\n");
#endif
        std::cout << s << std::endl;
    }
} // namespace turd
