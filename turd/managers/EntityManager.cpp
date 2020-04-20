#include "pch.hpp"
#include "EntityManager.hpp"

#include "entity/Entity.hpp"

namespace turd
{
    constexpr uint32_t MAX_ENTITIES = 1000;

    bool HeapComp(uint32_t a, uint32_t b) { return a < b; }

    class IDPool
    {
      public:
        IDPool()
        {
            for (uint32_t i = 0; i < MAX_ENTITIES; i++)
            {
                mUnallocated.push_back(i);
                std::push_heap(std::begin(mUnallocated), std::end(mUnallocated), HeapComp);
            }
        }

        ~IDPool() = default;

        NODISCARD std::optional<uint32_t> Take()
        {
            if (mUnallocated.empty())
            {
                return std::nullopt;
            }

            auto id = mUnallocated.back();
            std::pop_heap(std::begin(mUnallocated), std::end(mUnallocated), HeapComp);
            mUnallocated.pop_back();

            mAllocated.push_back(id);
            std::push_heap(std::begin(mAllocated), std::end(mAllocated), HeapComp);

            return id;
        }

        void Release(uint32_t id)
        {
            auto found = std::find(std::begin(mAllocated), std::end(mAllocated), id);
            if (found != mAllocated.end())
            {
                auto ignored =
                    std::remove_if(std::begin(mAllocated), std::end(mAllocated), [&id](auto &i) { return i == id; });
                std::make_heap(std::begin(mAllocated), std::end(mAllocated), HeapComp);
            }
            mUnallocated.push_back(id);
            std::push_heap(std::begin(mUnallocated), std::end(mUnallocated), HeapComp);
        }

      private:
        std::vector<uint32_t> mAllocated = {};
        std::vector<uint32_t> mUnallocated = {};
    };

    static IDPool gIDPool;

    EntityManager::EntityManager() { mEntities.resize(MAX_ENTITIES, nullptr); }

    EntityManager::~EntityManager()
    {
        std::for_each(std::begin(mEntities), std::end(mEntities), [](auto e) { delete e; });
        mEntities.clear();
    }

    Entity *EntityManager::Create()
    {
        auto id = gIDPool.Take();

        if (!id.has_value())
        {
            return nullptr;
        }

        mEntities[id.value()] = new Entity();
        mEntities[id.value()]->mID = id.value();
        return mEntities[id.value()];
    }

    void EntityManager::Destroy(Entity *pEntity)
    {
        if (pEntity)
        {
            if (!mEntities[pEntity->mID])
            {
                return;
            }

            auto id = pEntity->mID;
            delete mEntities[id];
            mEntities[id] = nullptr;
            gIDPool.Release(id);
        }
    }

    void EntityManager::ForEach(std::function<void(Entity *)> &func)
    {
        std::for_each(std::begin(mEntities), std::end(mEntities), [&](auto &e) {
            if (e)
            {
                func(e);
            }
        });
    }

    std::vector<Entity *> EntityManager::Current()
    {
        std::vector<Entity *> list = {};
        std::for_each(std::begin(mEntities), std::end(mEntities), [&list](auto &e) {
            if (e)
            {
                list.push_back(e);
            }
        });
        return list;
    }

} // namespace turd
