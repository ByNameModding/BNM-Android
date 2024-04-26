#pragma once

#include <map>

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "BasicMonoStructures.hpp"
#include "Class.hpp"
#include "Method.hpp"
#include "Delegates.hpp"

// Обычный C#-словарь (Dictionary)
namespace BNM::Structures::Mono {
    template<typename TKey, typename TValue>
    struct Dictionary : BNM::IL2CPP::Il2CppObject {
#ifdef BNM_DOTNET35
        struct Link { int HashCode{}, Next{}; };
            Array<int> *table{};
            Array<Link> *linkSlots{};
            Array<TKey> *keys{};
            Array<TValue> *values{};
            int touchedSlots{};
            int emptySlot{};
            int count{};
            int threshold{};
            void *hcp{};
            void *serialization_info{};
            int version{};
            std::map<TKey, TValue> ToMap() const {
                std::map<TKey, TValue> ret{};
                for (int i = 0; i < touchedSlots; i++)
                    if ((linkSlots->m_Items[i].HashCode & -2147483648) != 0)
                        ret.insert(std::make_pair(keys->m_Items[i], values->m_Items[i]));
                return ret;
            }
            std::vector<TKey> GetKeys() const {
                std::vector<TKey> ret{};
                for (int i = 0; i < touchedSlots; i++)
                    if ((linkSlots->m_Items[i].HashCode & -2147483648) != 0)
                        ret.push_back(keys->m_Items[i]);
                return ret;
            }
            std::vector<TValue> GetValues() const {
                std::vector<TValue> ret{};
                for (int i = 0; i < touchedSlots; i++)
                    if ((linkSlots->m_Items[i].HashCode & -2147483648) != 0)
                        ret.push_back(values->m_Items[i]);
                return ret;
            }
#else
        struct Entry {
            int hashCode{}, next{};
            TKey key{};
            TValue value{};
        };
        Array<int> *buckets{};
        Array<Entry> *entries{};
        int count{};
        int version{};
        int freeList{};
        int freeCount{};
        void *comparer{};
        Array<TKey> *keys{};
        Array<TValue> *values{};
        void *syncRoot{};
        std::map<TKey, TValue> ToMap() const {
            std::map<TKey, TValue> ret{};
            for (auto it = (Entry *)&entries->m_Items; it != ((Entry *)&entries->m_Items + count); ++it) ret.emplace(std::make_pair(it->key, it->value));
            return ret;
        }
        std::vector<TKey> GetKeys() const {
            std::vector<TKey> ret{};
            for (int i = 0; i < count; ++i) ret.emplace_back(entries->m_Items[i].key);
            return ret;
        }
        std::vector<TValue> GetValues() const {
            std::vector<TValue> ret{};
            for (int i = 0; i < count; ++i) ret.emplace_back(entries->m_Items[i].value);
            return ret;
        }
#endif
        int GetSize() const { return count; }
        int GetVersion() const { return version; }
        bool TryGet(TKey key, TValue *value) const { return Class((IL2CPP::Il2CppObject *)this).GetMethod(OBFUSCATE_BNM("TryGetValue"), 2).template cast<bool>()[(void *)this](key, value); }
        void Add(TKey key, TValue value) { return Class((IL2CPP::Il2CppObject *)this).GetMethod(OBFUSCATE_BNM("Add"), 2).template cast<void>()[(void *)this](key, value); }
        void Insert(TKey key, TValue value) { return Class((IL2CPP::Il2CppObject *)this).GetMethod(OBFUSCATE_BNM("set_Item"), 2).template cast<void>()[(void *)this](key, value); }
        bool Remove(TKey key) { return Class((IL2CPP::Il2CppObject *)this).GetMethod(OBFUSCATE_BNM("Remove"), 1).template cast<bool>()[(void *)this](key); }
        bool ContainsKey(TKey key) const { return Class((IL2CPP::Il2CppObject *)this).GetMethod(OBFUSCATE_BNM("ContainsKey"), 1).template cast<bool>()[(void *)this](key); }
        bool ContainsValue(TValue value) const { return Class((IL2CPP::Il2CppObject *)this).GetMethod(OBFUSCATE_BNM("ContainsValue"), 1).template cast<bool>()[(void *)this](value); }
        TValue Get(TKey key) const {
            TValue ret;
            if (TryGet(key, &ret)) return ret;
            return {};
        }
        TValue operator[](TKey key) const { return Get(key); }
    };
    template <typename ...Params>
    struct Action : MulticastDelegate<void> {
        inline void Invoke(Params ...params) { (MulticastDelegate<void>(*this)).Invoke(params...); }
    };
}