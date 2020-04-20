#pragma once

namespace turd
{
    struct SystemInfo
    {
        uint32_t NumberOfCores;
        uint64_t TotalMemory;
    };

    void GetSystemInfo(SystemInfo *pSysInfo);

    /**
     * @return Path to directory containing the executable
     */
    fs::path ExeDir();

    /**
     * @return Path to current working directory
     */
    fs::path WorkingDirectory();

    /**
     * @return Path to data directory
     */
    fs::path DataDirectory();
} // namespace turd
