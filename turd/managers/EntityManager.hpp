#pragma once

namespace turd
{
    class Entity;

    class EntityManager
    {
      public:
        EntityManager();
        ~EntityManager();

        Entity *Create();

        void Destroy(Entity *pEntity);

        void ForEach(std::function<void(Entity *)> &func);

        std::vector<Entity*> Current();

      private:
        std::vector<Entity *> mEntities = {};
    };
} // namespace turd
