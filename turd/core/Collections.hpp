#pragma once

namespace turd
{
    template <typename K, typename V>
    class Map : public std::map<K, V>
    {
      public:
        bool Contains(const K &key) { return this->find(key) != this->end(); }
        void ForEach(std::function<void(const K &key, const V &value)> &func)
        {
            auto it = this->cbegin();
            for (; it != this->cend(); ++it)
            {
                func(*it->first, *it->second);
            }
        }
    };

    template <typename T>
    class Vector : public std::vector<T>
    {
      public:
        Vector() = default;

        Vector(std::initializer_list<T> l) { this->insert(this->end(), l.begin(), l.end()); }

        void Add(const T &v) { push_back(v); }
        void Add(const T &&v) { push_back(v); }

        void ForEach(std::function<void(const T &value)> &func)
        {
            auto it = this->cbegin();
            for (; it != this->cend(); ++it)
            {
                func(*it);
            }
        }
    };
} // namespace turd
