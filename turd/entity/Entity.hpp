#pragma once

namespace turd
{
    class EntityManager;

    class Entity
    {
        friend class EntityManager;
      public:
        Entity() = default;
        ~Entity() = default;

        uint32_t ID();

      private:
        uint32_t mID;
    };
} // namespace turd
