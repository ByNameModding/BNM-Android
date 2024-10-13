#pragma once

#include <new>
#include <array>
#include <string_view>

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "DebugMessages.hpp"
#include "Utils.hpp"
#include "Defaults.hpp"

namespace BNM::Utils {
    template<typename T>
    struct DataIterator {
        T *value{};
        constexpr DataIterator() = default;
        constexpr DataIterator(const T *value) : value((T *)value) {}
        inline T& operator *() {
            BNM_LOG_ERR_IF(!value, DBG_BNM_MSG_DataIterator_Error);
            return *value;
        }
        inline T& operator *() const {
            BNM_LOG_ERR_IF(!value, DBG_BNM_MSG_DataIterator_Error);
            return *value;
        }
        inline operator T&() {
            BNM_LOG_ERR_IF(!value, DBG_BNM_MSG_DataIterator_Error);
            return *value;
        }
        inline operator T&() const {
            BNM_LOG_ERR_IF(!value, DBG_BNM_MSG_DataIterator_Error);
            return *value;
        }
        inline T& operator ->() {
            BNM_LOG_ERR_IF(!value, DBG_BNM_MSG_DataIterator_Error);
            return *value;
        }
        inline T& operator ->() const {
            BNM_LOG_ERR_IF(!value, DBG_BNM_MSG_DataIterator_Error);
            return *value;
        }
        inline DataIterator &operator=(T t) {
            BNM_LOG_ERR_IF(!t, DBG_BNM_MSG_DataIterator_Error);
            *this->value = *(T*)&t;
            return *this;
        }
        inline DataIterator &operator=(T t) const {
            BNM_LOG_ERR_IF(!t, DBG_BNM_MSG_DataIterator_Error);
            *this->value = *(T*)&t;
            return *this;
        }
    };
}
// Common C# classes (string, array, list) (string, [], List)
namespace BNM::Structures::Mono {

    template<typename T> struct List;
    namespace PRIVATE_MonoListData {
        void *CompareExchange4List(void *syncRoot);
        template<typename T>
        void InitMonoListVTable(List<T> *list);
    }
    struct String : BNM::IL2CPP::Il2CppObject {
        int length{};
        IL2CPP::Il2CppChar chars[0];
        std::string str();
        [[nodiscard]] unsigned int GetHash() const;
        static String *Empty();
#ifdef BNM_ALLOW_SELF_CHECKS
        [[nodiscard]] bool SelfCheck() const;
#endif
        inline bool IsNullOrEmpty() { return !BNM::CheckForNull(this) || !length; }
    private:
        inline constexpr String() : BNM::IL2CPP::Il2CppObject() {}
    };
    namespace __Internal_Array {
        void *ArrayFromClass(IL2CPP::Il2CppClass *, IL2CPP::il2cpp_array_size_t);
    }
    template<typename T>
    struct Array : BNM::IL2CPP::Il2CppObject  {
        IL2CPP::Il2CppArrayBounds *bounds{};
        IL2CPP::il2cpp_array_size_t capacity{};
        T m_Items[0];
        [[nodiscard]] inline IL2CPP::il2cpp_array_size_t GetCapacity() const { BNM_CHECK_SELF(0); return capacity; }
        [[nodiscard]] inline T *GetData() const { BNM_CHECK_SELF(nullptr); return (T * const) &m_Items[0]; }
        [[nodiscard]] std::vector<T> ToVector() const {
            std::vector<T> ret;
            BNM_CHECK_SELF(ret);
            for (IL2CPP::il2cpp_array_size_t i = 0; i < capacity; i++) ret.push_back(m_Items[i]);
            return ret;
        }
        inline bool CopyFrom(const std::vector<T> &vec) { BNM_CHECK_SELF(false); if (vec.empty()) return false; return CopyFrom((T *)vec.data(), (IL2CPP::il2cpp_array_size_t)vec.size()); }
        bool CopyFrom(T *arr, IL2CPP::il2cpp_array_size_t size) {
            BNM_CHECK_SELF(false);
            if (size > capacity) return false;
            memcpy(&m_Items[0], arr, size * sizeof(T));
            return true;
        }
        inline void CopyTo(T *arr) const { BNM_CHECK_SELF(); if (!CheckObj(m_Items)) return; memcpy(arr, m_Items, sizeof(T) * capacity); }
        inline Utils::DataIterator<T> operator[] (IL2CPP::il2cpp_array_size_t index) const { BNM_CHECK_SELF({}); if (GetCapacity() < index) return {}; return &m_Items[index]; }
        inline Utils::DataIterator<T> At(IL2CPP::il2cpp_array_size_t index) const { BNM_CHECK_SELF({}); if (GetCapacity() < index) return {}; return &m_Items[index]; }
        [[nodiscard]] inline bool Empty() const { BNM_CHECK_SELF(false); return GetCapacity() <= 0;}
        static Array<T> *Create(size_t capacity, bool _forceNonGC = false) {
            auto cls = _forceNonGC ? BNM::Defaults::DefaultTypeRef{} : BNM::Defaults::Get<T>();
            auto monoArr = (Array<T> *) (cls.Valid() ? __Internal_Array::ArrayFromClass(cls, capacity) : BNM_malloc(sizeof(Array) + sizeof(T) * capacity));
            memset(monoArr, 0, sizeof(Array) + sizeof(T) * capacity);
            if (!cls.Valid()) monoArr->klass = nullptr;
            monoArr->capacity = capacity;
            return monoArr;
        }
        static Array<T> *Create(const std::vector<T> &vec, bool _forceNonGC = false) { return Create((T *)vec.data(), vec.size(), _forceNonGC); }
        static Array<T> *Create(T *arr, size_t size, bool _forceNonGC = false) {
            Array<T> *monoArr = Create(size, _forceNonGC);
            monoArr->CopyFrom(arr, size);
            return monoArr;
        }
        // Only for arrays created via BNM!
        inline void Destroy() { if (!klass) BNM_free(this); }
#ifdef BNM_ALLOW_SELF_CHECKS
        [[nodiscard]] bool SelfCheck() const {
            if (CheckForNull(this)) return true;
            BNM_LOG_ERR(DBG_BNM_MSG_Array_SelfCheck_Error);
            return false;
        }
        inline constexpr Array() : BNM::IL2CPP::Il2CppObject() {
            klass = nullptr;
            monitor = nullptr;
        }
#endif
    };
    template<typename T>
    struct List : BNM::IL2CPP::Il2CppObject {
        struct Enumerator {
            List<T> *list{};
            int index{};
            int version{};
            T current{};
            constexpr Enumerator() = default;
            explicit Enumerator(List<T> *list) : Enumerator() { this->list = list; }

            inline T* begin() { return &list->items[0]; }
            inline T* end() { return &list->items->m_Items[list->size]; }
            [[nodiscard]] inline T* begin() const { return &list->items[0]; }
            [[nodiscard]] inline T* end() const { return &list->items->m_Items[list->size]; }
        };
        Array<T> *items{};
        int size{};
        int version{};
        void *syncRoot{};
        [[nodiscard]] inline T *GetData() const { return items->GetData(); }
        [[nodiscard]] inline int GetSize() const { return size; }
        [[nodiscard]] inline int GetVersion() const { return version; }
        [[nodiscard]] std::vector<T> ToVector() const {
            std::vector<T> ret{};
            BNM_CHECK_SELF(ret);
            for (int i = 0; i < size; i++) ret.push_back(GetData()[i]);
            return ret;
        }
        void Add(T val) {
            GrowIfNeeded(1);
            items->m_Items[size] = val;
            size++;
            version++;
        }
        [[nodiscard]] int IndexOf(T val) const {
            for (int i = 0; i < size; i++) if (items->m_Items[i] == val) return i;
            return -1;
        }
        void RemoveAt(int index) {
            if (index != -1) {
                Shift(index, -1);
                version++;
            }
        }
        bool Remove(T val) {
            int i = IndexOf(val);
            if (i == -1) return false;
            RemoveAt(i);
            return true;
        }
        bool Resize(int newCapacity) {
            BNM_CHECK_SELF(false);
            if (newCapacity <= items->capacity) return false;
            auto nItems = Array<T>::Create(newCapacity);
            nItems->klass = items->klass;
            nItems->monitor = items->monitor;
            nItems->bounds = items->bounds;
            nItems->capacity = newCapacity;
            // Do not copy if the array is empty
            if (items->capacity > 0)
                memcpy(&nItems->m_Items[0], &items->m_Items[0], items->capacity * sizeof(T));
            items = nItems;
            return true;
        }
        Utils::DataIterator<T> operator[] (int index) const { if (index >= size) return {}; return &items->m_Items[index]; }
        [[nodiscard]] Utils::DataIterator<T> At(int index) const { if (index >= size) return {}; return &items->m_Items[index]; }
        inline bool CopyFrom(const std::vector<T> &vec) { return CopyFrom((T *)vec.data(), (int)vec.size()); }
        bool CopyFrom(T *arr, int arrSize) {
            BNM_CHECK_SELF(false);
            Resize(arrSize);
            memcpy(items->m_Items, arr, arrSize * sizeof(T));
            return true;
        }
        void Clear() {
            if (size > 0) memset(items->m_Items, 0, size * sizeof(T));
            ++version; size = 0;
        }
        // Not quite like in C# because of its features
        [[nodiscard]] bool Contains(T item) const {
            for (int i = 0; i < size; i++) if (items->m_Items[i] == item) return true;
            return false;
        }
        Enumerator GetEnumerator() { return this; }
        [[nodiscard]] T get_Item(int index) const {
            if (index >= size) return {};
            return items->m_Items[index];
        }
        void set_Item(int index, T item) {
            if (index >= size) return;
            items->m_Items[index] = item;
            ++version;
        }
        void Insert(int index, T item) {
            if (index > size) return;
            if (size == items->capacity) GrowIfNeeded(1);
            if (index < size) memmove(items->m_Items + index + 1, items->m_Items + index, (size - index) * sizeof(T));
            items->m_Items[index] = item;
            ++size;
            ++version;
        }
        void *get_SyncRoot() {
            if (!syncRoot) syncRoot = PRIVATE_MonoListData::CompareExchange4List(syncRoot);
            return syncRoot;
        }
        bool get_false() { return false; }
        void CopyTo(Array<T>* arr, int arrIndex) {
            memcpy(items->m_Items, arr->m_Items + arrIndex, size * sizeof(T));
        }
        void GrowIfNeeded(int n) {
            if (size + n > items->capacity)
                Resize(size + n);
        }
        void Shift(int start, int delta) {
            if (delta < 0) start -= delta;
            if (start < size) memmove(items->m_Items + start + delta, items->m_Items + start, size - start);
            size += delta;
            if (delta < 0) memset(items->m_Items + size + delta, 0, -delta * sizeof(T));
        }
#ifdef BNM_ALLOW_SELF_CHECKS
        [[nodiscard]] bool SelfCheck() const {
            if (CheckForNull(this)) return true;
            BNM_LOG_ERR(DBG_BNM_MSG_List_SelfCheck_Error);
            return false;
        }
#endif
        inline constexpr List() : BNM::IL2CPP::Il2CppObject() {
            klass = nullptr;
            monitor = nullptr;
        }
    };

    // Based on https://github.com/royvandam/rtti/tree/cf0dee6fb3999573f45b0726a8d5739022e3dacf
    namespace PRIVATE_MonoListData {
        template <typename T> constexpr std::string_view WrappedTypeName() { return __PRETTY_FUNCTION__; }
        constexpr std::size_t WrappedTypeNamePrefixLength() { return WrappedTypeName<void>().find("void"); }
        constexpr std::size_t WrappedTypeNameSuffixLength() { return WrappedTypeName<void>().length() - WrappedTypeNamePrefixLength() - 4; }
        constexpr uint32_t FNV1a(const char* str, size_t n, uint32_t hash = 2166136261U) {
            return n == 0 ? hash : FNV1a(str + 1, n - 1, (hash ^ str[0]) * 19777619U);
        }
        constexpr uint32_t FNV1a(const std::string_view &str) { return FNV1a(str.data(), str.size()); }
        template <typename T>
        constexpr uint32_t HashedTypeName() {
            constexpr auto wrappedTypeName = WrappedTypeName<T>();
            constexpr auto prefixLength = WrappedTypeNamePrefixLength();
            constexpr auto suffixLength = WrappedTypeNameSuffixLength();
            constexpr auto typeNameLength = wrappedTypeName.length() - prefixLength - suffixLength;
            constexpr auto typeName = wrappedTypeName.substr(prefixLength, typeNameLength);
            return FNV1a(typeName.data(), typeName.size());
        }
        struct MethodData { std::string_view methodName{}; void *ptr{}; };
        IL2CPP::Il2CppClass *TryGetMonoListClass(uint32_t typeHash, std::array<MethodData, 16> &data);

        template<typename T>
        void InitMonoListVTable(List<T> *list) {
            using namespace PRIVATE_MonoListData;
            /*
             * Replace the virtual method table because the original one is empty.
             * It is also necessary for types that are not in the game (not the fact that the game would be able to use them without, but the code is there)
             */
            using Type = std::conditional_t<std::is_pointer_v<T>, void*, T>;
            constexpr auto RemoveAt = &List<Type>::RemoveAt; constexpr auto GetSize = &List<Type>::GetSize; constexpr auto Clear = &List<Type>::Clear;
            constexpr auto get_Item = &List<Type>::get_Item; constexpr auto set_Item = &List<Type>::set_Item; constexpr auto IndexOf = &List<Type>::IndexOf;
            constexpr auto Insert = &List<Type>::Insert; constexpr auto get_false = &List<Type>::get_false; constexpr auto Add = &List<Type>::Add;
            constexpr auto Contains = &List<Type>::Contains; constexpr auto CopyTo = &List<Type>::CopyTo; constexpr auto Remove = &List<Type>::Remove;
            constexpr auto GetEnumerator = &List<Type>::GetEnumerator; constexpr auto get_SyncRoot = &List<Type>::get_SyncRoot;
            static std::array<MethodData, 16> namesMap = {
                    MethodData{BNM_OBFUSCATE("RemoveAt"), *(void **)&RemoveAt}, MethodData{BNM_OBFUSCATE("get_Count"), *(void **)&GetSize},
                    MethodData{BNM_OBFUSCATE("Clear"), *(void **)&Clear}, MethodData{BNM_OBFUSCATE("get_Item"), *(void **)&get_Item},
                    MethodData{BNM_OBFUSCATE("set_Item"), *(void **)&set_Item}, MethodData{BNM_OBFUSCATE("IndexOf"), *(void **)&IndexOf},
                    MethodData{BNM_OBFUSCATE("Insert"), *(void **)&Insert}, MethodData{BNM_OBFUSCATE("get_IsReadOnly"), *(void **)&get_false},
                    MethodData{BNM_OBFUSCATE("get_IsFixedSize"), *(void **)&get_false}, MethodData{BNM_OBFUSCATE("get_IsSynchronized"), *(void **)&get_false},
                    MethodData{BNM_OBFUSCATE("Add"), *(void **)&Add}, MethodData{BNM_OBFUSCATE("Contains"), *(void **)&Contains},
                    MethodData{BNM_OBFUSCATE("CopyTo"), *(void **)&CopyTo}, MethodData{BNM_OBFUSCATE("Remove"), *(void **)&Remove},
                    MethodData{BNM_OBFUSCATE("GetEnumerator"), *(void **)&GetEnumerator}, MethodData{BNM_OBFUSCATE("get_SyncRoot"), *(void **)&get_SyncRoot}
            };
            list->klass = TryGetMonoListClass(HashedTypeName<Type>(), namesMap);
        }
    }

}
