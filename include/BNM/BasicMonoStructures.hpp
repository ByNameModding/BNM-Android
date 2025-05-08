#pragma once

#include <new>
#include <array>
#include <string_view>

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "DebugMessages.hpp"
#include "Utils.hpp"
#include "Defaults.hpp"

// NOLINTBEGIN
namespace BNM::Utils {
    /**
        @brief Helper struct for checking values that passed from Mono::Array.
        @tparam T Data type
    */
    template<typename T>
    struct DataIterator {
        T *value{};
        /// @cond
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
        /// @endcond
    };
}

/**
    @brief Namespace that holds some basic and the most commonly used C# classes.

    It has common C# classes: string, array, List, Dictionary, Action.
*/
namespace BNM::Structures::Mono {

    /**
        @brief Struct of decimal type implementation.
    */
    struct decimal {
        int flags{};
        int hi{};
        int lo{};
        int mid{};
    };

    template<typename T> struct List;

    /// @cond
    namespace PRIVATE_MonoListData {
        void *CompareExchange4List(void *syncRoot);
        template<typename T>
        void InitMonoListVTable(List<T> *list);
    }
    /// @endcond

    /**
        @brief String type implementation.
    */
    struct String : BNM::IL2CPP::Il2CppObject {
        int length{};
        IL2CPP::Il2CppChar chars[0];
        /**
            @brief Convert Mono::String to std::string.

            Converts C#'s UTF16 string to C++'s UTF8 string.

            @return Converted string if it's valid, otherwise "ERROR: Mono::String dead".
        */
        std::string str();

        /**
            @brief Calculate string hash.

            Calculates string's hash like it's done in il2cpp VM.

            @return Hash of string if it's valid, otherwise zero.
        */
        [[nodiscard]] unsigned int GetHash() const;

        /**
            @brief Get empty string.

            Gets C# empty string.

            @return Empty string if it's found, otherwise null.
        */
        static String *Empty();

#ifdef BNM_ALLOW_SELF_CHECKS
        /**
            @brief Check if string isn't null.

            Checks if string isn't null and logs if it's null.
            It's used mostly internally in debug builds to crash proof code.

            @return True if string isn't null.
        */
        [[nodiscard]] bool SelfCheck() const;
#endif

        /**
            @brief Check if string is not null or not empty.
            @return True if string is not null and not empty.
        */
        inline bool IsNullOrEmpty() const { return !BNM::CheckForNull(this) || !length; }
    private:
        inline constexpr String() : BNM::IL2CPP::Il2CppObject() {}
    };

    /// @cond
    namespace __Internal_Array {
        void *ArrayFromClass(IL2CPP::Il2CppClass *, IL2CPP::il2cpp_array_size_t);
    }
    /// @endcond

    /**
        @brief Array type implementation.
        @tparam T Element type
    */
    template<typename T>
    struct Array : BNM::IL2CPP::Il2CppObject  {
        IL2CPP::Il2CppArrayBounds *bounds{};
        IL2CPP::il2cpp_array_size_t capacity{};
        T m_Items[0];
        /**
            @brief Get array size.

            @return Array size if it's valid, otherwise zero.
        */
        [[nodiscard]] inline IL2CPP::il2cpp_array_size_t GetCapacity() const { BNM_CHECK_SELF(0); return capacity; }

        /**
            @brief Get array size.

            Alias of GetCapacity().

            @return Array size if it's valid, otherwise zero.
        */
        [[nodiscard]] inline IL2CPP::il2cpp_array_size_t GetSize() const { return GetCapacity(); }

        /**
            @brief Get array data pointer.
            @return Array data pointer if array is valid, otherwise null.
        */
        [[nodiscard]] inline T *GetData() const { BNM_CHECK_SELF(nullptr); return (T *const) &m_Items[0]; }

        /**
            @brief Convert array to std::vector.
            @return Vector of elements if array is valid, otherwise empty vector.
        */
        [[nodiscard]] std::vector<T> ToVector() const {
            std::vector<T> ret;
            BNM_CHECK_SELF(ret);
            for (IL2CPP::il2cpp_array_size_t i = 0; i < capacity; i++) ret.push_back(m_Items[i]);
            return ret;
        }

        /**
            @brief Copy data from vector to current array.
            @param vec Source vector
            @return True if vector size is the same or less than array's size.
        */
        inline bool CopyFrom(const std::vector<T> &vec) { BNM_CHECK_SELF(false); if (vec.empty()) return false; return CopyFrom((T *)vec.data(), (IL2CPP::il2cpp_array_size_t)vec.size()); }

        /**
            @brief Copy data from pointer to current array.

            @param arr Source pointer
            @param size Source pointer size

            @return True if size is the same or less than array's size.
        */
        bool CopyFrom(T *arr, IL2CPP::il2cpp_array_size_t size) {
            BNM_CHECK_SELF(false);
            if (size > capacity) return false;
            memcpy(&m_Items[0], arr, size * sizeof(T));
            return true;
        }

        /**
            @brief Copy data from current array to some pointer.
        */
        inline void CopyTo(T *arr) const { BNM_CHECK_SELF(); if (!IsAllocated(m_Items)) return; memcpy(arr, m_Items, sizeof(T) * capacity); }

        /**
            @brief Get element at index.
            @return DataIterator of target element if array is valid and index isn't out of bounds, otherwise empty DataIterator.
        */
        inline Utils::DataIterator<T> At(IL2CPP::il2cpp_array_size_t index) const { BNM_CHECK_SELF({}); if (GetCapacity() < index) return {}; return &m_Items[index]; }
        inline Utils::DataIterator<T> operator[] (IL2CPP::il2cpp_array_size_t index) const { return At(index); }

        /**
            @brief Check if array is empty.
            @return True if array isn't empty.
        */
        [[nodiscard]] inline bool Empty() const { BNM_CHECK_SELF(false); return GetCapacity() <= 0;}

        /**
            @brief Creates new empty array of set capacity.

            @param capacity Size of new array
            @param _forceUseAlloc Forces code to create array using BNM::Allocate instead of BNM::Class::NewArray (Advanced usage only).

            @return New array
        */
        static Array<T> *Create(size_t capacity, bool _forceUseAlloc = false) {
            auto cls = _forceUseAlloc ? BNM::Defaults::DefaultTypeRef{} : BNM::Defaults::Get<T>();
#ifndef  BNM_USE_IL2CPP_ALLOCATOR
            auto monoArr = (Array<T> *) (cls.IsValid() ? __Internal_Array::ArrayFromClass(cls, capacity) : BNM_malloc(sizeof(Array) + sizeof(T) * capacity));
#else
            auto monoArr = (Array<T> *) (cls.IsValid() ? __Internal_Array::ArrayFromClass(cls, capacity) : BNM::Allocate(sizeof(Array) + sizeof(T) * capacity));
#endif
            memset(monoArr, 0, sizeof(Array) + sizeof(T) * capacity);
            if (!cls.IsValid()) monoArr->klass = nullptr;
            monoArr->capacity = capacity;
            return monoArr;
        }

        /**
            @brief Creates new array using data from set vector.

            @param vec Source vector
            @param _forceUseAlloc Forces code to create array using BNM::Allocate instead of BNM::Class::NewArray (Advanced usage only).

            @return New array
        */
        static Array<T> *Create(const std::vector<T> &vec, bool _forceUseAlloc = false) { return Create((T *)vec.data(), vec.size(), _forceUseAlloc); }

        /**
            @brief Creates new array using data from set pointer.

            @param arr Source pointer
            @param size Source pointer size
            @param _forceUseAlloc Forces code to create array using BNM::Allocate instead of BNM::Class::NewArray (Advanced usage only).

            @return New array
        */
        static Array<T> *Create(T *arr, size_t size, bool _forceUseAlloc = false) {
            Array<T> *monoArr = Create(size, _forceUseAlloc);
            monoArr->CopyFrom(arr, size);
            return monoArr;
        }

        /**
            @brief Destroy array.

            Frees allocated memory for array.

            @warning Only for arrays created via BNM!
        */
        inline void Destroy() {
#ifndef  BNM_USE_IL2CPP_ALLOCATOR
            if (!klass) BNM_free(this);
#else
            if (!klass) BNM::Free(this);
#endif
        }

#ifdef BNM_ALLOW_SELF_CHECKS
        /**
            @brief Check if array isn't null.

            Checks if array is not null and logs if it's null.
            It's used mostly internally in debug builds to crash proof code.

            @return True if string isn't null.
        */
        [[nodiscard]] bool SelfCheck() const {
            if (CheckForNull(this)) return true;
            BNM_LOG_ERR(DBG_BNM_MSG_Array_SelfCheck_Error);
            return false;
        }
#endif

        /**
            @brief Create empty array.
        */
        inline constexpr Array() : BNM::IL2CPP::Il2CppObject() {
            klass = nullptr;
            monitor = nullptr;
        }
    };

    /**
        @brief System.Generic.List type implementation
        @tparam T Element type
    */
    template<typename T>
    struct List : BNM::IL2CPP::Il2CppObject {

        /**
            @brief System.Generic.List.Enumerator struct implementation

            Can be used to iterate list in foreach.
        */
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

        /**
            @brief Get list data pointer.
            @return List data pointer if list is valid, otherwise null.
        */
        [[nodiscard]] inline T *GetData() const { return items->GetData(); }

        /**
            @brief Get list size.
            @return List size
        */
        [[nodiscard]] inline int GetSize() const { return size; }

        /**
            @brief Get list capacity.
            @return List capacity if list is valid, otherwise zero.
        */
        [[nodiscard]] inline int GetCapacity() const { return items ? items->GetCapacity() : 0; }

        /**
            @brief Get list version.
            @return List version
        */
        [[nodiscard]] inline int GetVersion() const { return version; }

        /**
            @brief Convert list to std::vector.
            @return Vector of elements if list is valid, otherwise empty vector.
        */
        [[nodiscard]] std::vector<T> ToVector() const {
            std::vector<T> ret{};
            BNM_CHECK_SELF(ret);
            for (int i = 0; i < size; i++) ret.push_back(GetData()[i]);
            return ret;
        }

        /**
            @brief Add element to list.
        */
        void Add(T val) {
            GrowIfNeeded(1);
            items->m_Items[size] = val;
            size++;
            version++;
        }

        /**
            @brief Get element index.
        */
        [[nodiscard]] int IndexOf(T val) const {
            for (int i = 0; i < size; i++) if (items->m_Items[i] == val) return i;
            return -1;
        }

        /**
            @brief Remove element at index.
        */
        void RemoveAt(int index) {
            if (index != -1) {
                Shift(index, -1);
                version++;
            }
        }

        /**
            @brief Remove element.
            @return True if element is removed.
        */
        bool Remove(T val) {
            int i = IndexOf(val);
            if (i == -1) return false;
            RemoveAt(i);
            return true;
        }

        /**
            @brief Resize list.
            @param newCapacity New capacity for target list.
            @return True if newCapacity is bigger than list's capacity.
        */
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

        /**
            @brief Get element at index.

            @param index Element index

            @return DataIterator of target element if index isn't out of bounds, otherwise empty DataIterator.
        */
        [[nodiscard]] Utils::DataIterator<T> At(int index) const { if (index >= size) return {}; return &items->m_Items[index]; }
        Utils::DataIterator<T> operator[] (int index) const { return At(index); }

        /**
            @brief Copy data from vector to current array.
            @param vec Source vector
            @return True if vector size is the same or less than array's size.
        */
        inline bool CopyFrom(const std::vector<T> &vec) { return CopyFrom((T *)vec.data(), (int)vec.size()); }

        /**
            @brief Copy data from pointer to current list.

            @param arr Source pointer
            @param size Source pointer size

            @return True if list is valid.
        */
        bool CopyFrom(T *arr, int arrSize) {
            BNM_CHECK_SELF(false);
            Resize(arrSize);
            memcpy(items->m_Items, arr, arrSize * sizeof(T));
            return true;
        }

        /**
            @brief Remove all list elements.
        */
        void Clear() {
            if (size > 0) memset(items->m_Items, 0, size * sizeof(T));
            ++version; size = 0;
        }

        /**
            @brief Check if element is in list.

            It's not quite like in C# because of its features.

            @param item Element to check

            @return True if element was found.
        */
        [[nodiscard]] bool Contains(T item) const {
            for (int i = 0; i < size; i++) if (items->m_Items[i] == item) return true;
            return false;
        }
        Enumerator GetEnumerator() { return this; }

        /**
            @brief Get element at index.

            @param index Element index

            @return Element if index isn't out of bounds, otherwise default value.
        */
        [[nodiscard]] T get_Item(int index) const {
            if (index >= size) return {};
            return items->m_Items[index];
        }

        /**
            @brief Set element at index.
            @param index Element index
            @param item New element value
        */
        void set_Item(int index, T item) {
            if (index >= size) return;
            items->m_Items[index] = item;
            ++version;
        }

        /**
            @brief Insert element to index.
            @param index Element index
            @param item New element value
        */
        void Insert(int index, T item) {
            if (index > size) return;
            if (size == items->capacity) GrowIfNeeded(1);
            if (index < size) memmove(items->m_Items + index + 1, items->m_Items + index, (size - index) * sizeof(T));
            items->m_Items[index] = item;
            ++size;
            ++version;
        }

        /// @cond
        void *get_SyncRoot() {
            if (!syncRoot) syncRoot = PRIVATE_MonoListData::CompareExchange4List(syncRoot);
            return syncRoot;
        }
        bool get_false() const { return false; }
        /// @endcond

        /**
            @brief Copy data from current list to some array.
        */
        void CopyTo(Array<T>* arr, int arrIndex) const {
            memcpy(items->m_Items, arr->m_Items + arrIndex, size * sizeof(T));
        }

        /**
            @brief Check if list has capacity for n elements.
            @param n Number of additional elements
        */
        void GrowIfNeeded(int n) {
            if (size + n > items->capacity)
                Resize(size + n);
        }

        /**
            @brief Shift data in list.
            @param start Start index
            @param delta Move delta
        */
        void Shift(int start, int delta) {
            if (delta < 0) start -= delta;
            if (start < size) memmove(items->m_Items + start + delta, items->m_Items + start, size - start);
            size += delta;
            if (delta < 0) memset(items->m_Items + size + delta, 0, -delta * sizeof(T));
        }

#ifdef BNM_ALLOW_SELF_CHECKS
        /**
            @brief Check if list isn't null.

            Checks if list is not null and logs if it's null.
            It's used mostly internally in debug builds to crash proof code.

            @return True if list isn't null.
        */
        [[nodiscard]] bool SelfCheck() const {
            if (CheckForNull(this)) return true;
            BNM_LOG_ERR(DBG_BNM_MSG_List_SelfCheck_Error);
            return false;
        }
#endif
        /**
            @brief Create empty list.
        */
        inline constexpr List() : BNM::IL2CPP::Il2CppObject() {
            klass = nullptr;
            monitor = nullptr;
        }
    };

    // Based on https://github.com/royvandam/rtti/tree/cf0dee6fb3999573f45b0726a8d5739022e3dacf
    /// @cond
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
             * Replaces the virtual method table because the original one is empty.
             * It is also necessary for types that are not in the game (it's not a fact that the game would be able to use them without that types, but the code is there)
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
    /// @endcond
}
// NOLINTEND