#include "pch.hpp"
#include "Platform.hpp"

namespace turd
{
    static uint32_t NumberOfCores()
    {
        SYSTEM_INFO systemInfo;
        GetSystemInfo(&systemInfo);
        return systemInfo.dwNumberOfProcessors;
    }

    static uint64_t TotalMemory()
    {
        MEMORYSTATUSEX memoryStatus = {};
        memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
        if (GlobalMemoryStatusEx(&memoryStatus))
        {
            return memoryStatus.ullTotalPhys;
        }
        return 0;
    }

    void GetSystemInfo(SystemInfo *pSysInfo)
    {
        pSysInfo->NumberOfCores = NumberOfCores();
        pSysInfo->TotalMemory = TotalMemory();
    }

    fs::path ExeDir()
    {
        CHAR szFileName[MAX_PATH];
        GetModuleFileNameA(NULL, szFileName, MAX_PATH);
        return fs::path(szFileName).parent_path();
    }

    fs::path WorkingDirectory()
    {
        DWORD nBufferLength = MAX_PATH;
        char buffer[MAX_PATH];
        if (!GetCurrentDirectory(nBufferLength, buffer))
        {
            throw;
        }
        return fs::path(buffer);
    }

    fs::path DataDirectory()
    {
        std::vector<fs::path> paths = {ExeDir() / "data", WorkingDirectory() / "data"};

        auto p = std::find_if(paths.begin(), paths.end(), [](auto &p) { return fs::exists(p); });

        if (p == paths.end())
        {
            throw;
        }

        return *p;
    }
} // namespace turd
