#include "pch.hpp"
#include "Adapter.hpp"

namespace turd
{
    static ComPtr<IDXGIAdapter1> mAdapter;
    static ComPtr<IDXGIFactory4> mFactory;
    static D3D_FEATURE_LEVEL mFeatureLevel;

    struct AdapterHolder
    {
        ComPtr<IDXGIAdapter1> Adapter;
        D3D_FEATURE_LEVEL FeatureLevel;
        DXGI_ADAPTER_DESC1 Description;
    };

    IDXGIAdapter1 *Adapter::Get()
    {
        if (mAdapter)
        {
            return mAdapter.Get();
        }

        D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1,
                                             D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
                                             D3D_FEATURE_LEVEL_9_3,  D3D_FEATURE_LEVEL_9_2,  D3D_FEATURE_LEVEL_9_1};

        IDXGIFactory4 *pFactory = Factory();

        IDXGIAdapter1 *pAdapter;

        std::vector<AdapterHolder> foundAdapters;

        for (uint32_t i = 0; pFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; i++)
        {
            DXGI_ADAPTER_DESC1 desc;
            pAdapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }

            for (auto featureLevel : featureLevels)
            {
                if (SUCCEEDED(D3D12CreateDevice(pAdapter, featureLevel, __uuidof(ID3D12Device), nullptr)))
                {
                    foundAdapters.push_back({pAdapter, featureLevel, desc});
                    break;
                }
            }
        }

        if (foundAdapters.size() == 0)
        {
            throw; // RuntimeError(_T("No suitable adapters found"));
        }

        std::sort(foundAdapters.begin(), foundAdapters.end(),
                  [](AdapterHolder a, AdapterHolder b) { return b.FeatureLevel < a.FeatureLevel; });

        mAdapter = foundAdapters[0].Adapter;
        mFeatureLevel = foundAdapters[0].FeatureLevel;

        return mAdapter.Get();
    }

    D3D_FEATURE_LEVEL Adapter::GetFeatureLevel() { return mFeatureLevel; }

    IDXGIFactory4 *Adapter::Factory()
    {
        if (!mFactory)
        {
            if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&mFactory))))
            {
                throw; // RuntimeError(_T("Failed to create a DXGI factory"));
            }
        }
        return mFactory.Get();
    }
} // namespace turd
