#pragma once

namespace turd
{
    /*!
     * \interface ISystem
     * \brief Interface for systems
     *
     * \author IffyCoder
     */
    class ISystem
    {
      public:
        ISystem() = default;
        virtual ~ISystem() = default;

        virtual std::string Name() = 0;

        virtual void Update(const float dt) = 0;
    };
} // namespace turd
