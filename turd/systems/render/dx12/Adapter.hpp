#pragma once

namespace turd
{
    class Adapter
    {
      public:
        Adapter() = default;
        ~Adapter() = default;

        /*!
         * Retreive the adapter.
         *
         * \return A hardware adapter with the highest feature level.
         */
        static IDXGIAdapter1 *Get();

        /*!
         * \return The feature level of the adapter
         */
        static D3D_FEATURE_LEVEL GetFeatureLevel();

        /*!
         * \return The DXGI factory used.
         */
        static IDXGIFactory4 *Factory();
    };
} // namespace turd
