#pragma once
#include <map>
#include <jni.h>
#include <array>
#include <string>
#include <vector>
#include <csetjmp>
#include <cstring>
#include <csignal>
#include <utility>
#include <unistd.h>
#include "BNM_settings.hpp"
#include "BNM_data/BasicStructs.h"
namespace BNM {
#if defined(__LP64__)
    typedef long BNM_INT_PTR;
    typedef unsigned long BNM_PTR;
#else
    typedef int BNM_INT_PTR;
    typedef unsigned int BNM_PTR;
#endif
    namespace IL2CPP {
#if UNITY_VER == 171
#include "BNM_data/Il2CppHeaders/2017.1.h"
#elif UNITY_VER > 171 && UNITY_VER < 181
#include "BNM_data/Il2CppHeaders/2017.4.h"
#elif UNITY_VER == 181
#include "BNM_data/Il2CppHeaders/2018.1.h"
#elif UNITY_VER == 182
#include "BNM_data/Il2CppHeaders/2018.2.h"
#elif UNITY_VER > 182 && UNITY_VER < 185
#include "BNM_data/Il2CppHeaders/2018.4.h"
#elif UNITY_VER > 190 && UNITY_VER < 193
#include "BNM_data/Il2CppHeaders/2019.2.h"
#elif UNITY_VER == 193
#include "BNM_data/Il2CppHeaders/2019.3.h"
#elif UNITY_VER == 194
#include "BNM_data/Il2CppHeaders/2019.4.h"
#elif UNITY_VER == 201
#include "BNM_data/Il2CppHeaders/2020.1.h"
#elif UNITY_VER == 202
#include "BNM_data/Il2CppHeaders/2020.2.h"
#elif UNITY_VER == 203
#include "BNM_data/Il2CppHeaders/2020.3.h"
#elif UNITY_VER == 211
#include "BNM_data/Il2CppHeaders/2021.1.h"
#elif UNITY_VER == 212
#include "BNM_data/Il2CppHeaders/2021.2.h"
#elif UNITY_VER == 213
#include "BNM_data/Il2CppHeaders/2021.3.h"
#elif UNITY_VER == 221
#include "BNM_data/Il2CppHeaders/2022.1.h"
#elif UNITY_VER >= 222 && UNITY_VER <= 223
#include "BNM_data/Il2CppHeaders/2022.2.h"
#elif UNITY_VER >= 231
#include "BNM_data/Il2CppHeaders/2023.1.h"
#else
#include "BNM_data/Il2CppHeaders/2021.3.h"
#endif
    }
    typedef IL2CPP::Il2CppReflectionType MonoType;
    typedef std::vector<IL2CPP::Il2CppAssembly *> AssemblyVector;
    typedef std::vector<IL2CPP::Il2CppClass *> ClassVector;

    // Проверить, действителен ли указатель
    template <typename T>
    inline bool IsAllocated(T x) {
#ifdef BNM_ALLOW_SAFE_IS_ALLOCATED
        static jmp_buf jump;
        static sighandler_t handler = [](int) { longjmp(jump, 1); };
        char c;
        bool ok = true;
        sighandler_t old_handler = signal(SIGSEGV, handler);
        if (!setjmp (jump)) c = *(char *) (x); else ok = false;
        signal(SIGSEGV, old_handler);
        return ok;
#else
        return (char *)x != nullptr;
#endif
    }
    template <typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    inline T CheckObj(T obj) {
        if (obj && IsAllocated(obj)) return obj;
        return nullptr;
    }
    namespace UnityEngine {
        // Должен быть использован для новых классов, если родитель: UnityEngine.Object, MonoBehaviour, ScriptableObject
        // Для System.Object нужно использовать BNM::IL2CPP::Il2CppObject
        struct Object : public BNM::IL2CPP::Il2CppObject {
            BNM_INT_PTR m_CachedPtr = 0;
            inline bool Alive() { return BNM::CheckObj((void *)this) && (BNM_PTR)m_CachedPtr; }
            inline bool Same(void *object) { return Same((Object *)object); }
            inline bool Same(Object *object) { return (!Alive() && !object->Alive()) || (Alive() && object->Alive() && m_CachedPtr == object->m_CachedPtr); }
        };
    }

    // Только если объект - дочерний элемент класса UnityEngine.Object или объект - это UnityEngine.Object
    template <typename T>
    inline bool IsUnityObjectAlive(T o) {
        return ((UnityEngine::Object *)o)->Alive();
    };
    // Только если объект - дочерний элемент класса UnityEngine.Object или объект - это UnityEngine.Object
    template <typename T1, typename T2>
    inline bool IsSameUnityObject(T1 o1, T2 o2) {
        auto obj1 = (UnityEngine::Object *)o1;
        auto obj2 = (UnityEngine::Object *)o2;
        return obj1->Same(obj2);
    };
    template<typename MET_T, typename PTR_T>
    inline void InitFunc(MET_T& method, PTR_T ptr) {
        *(void **)&method = (void *)ptr;
    };
    template<typename T>
    struct DataIterator {
        T *value{};
        constexpr DataIterator() = default;
        constexpr DataIterator(const T *value) : value((T *)value) {}
        inline T& operator *() {
            BNM_LOG_ERR_IF(!value, "Пустой объект в итераторе");
            return *value;
        }
        inline T& operator *() const {
            BNM_LOG_ERR_IF(!value, "Пустой объект в итераторе");
            return *value;
        }
        inline operator T&() {
            BNM_LOG_ERR_IF(!value, "Пустой объект в итераторе");
            return *value;
        }
        inline operator T&() const {
            BNM_LOG_ERR_IF(!value, "Пустой объект в итераторе");
            return *value;
        }
        inline T& operator ->() {
            BNM_LOG_ERR_IF(!value, "Пустой объект в итераторе");
            return *value;
        }
        inline T& operator ->() const {
            BNM_LOG_ERR_IF(!value, "Пустой объект в итераторе");
            return *value;
        }
        inline DataIterator &operator=(T t) {
            BNM_LOG_ERR_IF(!t, "Пустой объект в итераторе");
            *this->value = *(T*)&t;
            return *this;
        }
        inline DataIterator &operator=(T t) const {
            BNM_LOG_ERR_IF(!t, "Пустой объект в итераторе");
            *this->value = *(T*)&t;
            return *this;
        }
    };

    // Обычные классы C# (строка, массив, список) (string, [], List)
    namespace Structures::Mono {
        template<typename T> struct monoList;
        void *CompareExchange4List(void *syncRoot);
        template<typename T>
        void InitMonoListVTable(monoList<T> *list);
        struct monoString : BNM::IL2CPP::Il2CppObject {
            int length{};
            IL2CPP::Il2CppChar chars[0];
            std::string str();
#ifdef BNM_DEPRECATED
            [[deprecated("Используйте str вместо get_string")]] inline std::string get_string() { return str(); }
            [[deprecated("Потенциальная утечка памяти")]] inline const char *get_const_char() { return str().c_str(); }
            [[deprecated("Потенциальная утечка памяти")]] inline const char *c_str() { return str().c_str(); }
#endif
            unsigned int GetHash() const;
            static monoString *Create(const char *str);
            static monoString *Create(const std::string &str);
            static monoString *Empty();
#ifdef BNM_ALLOW_SELF_CHECKS
            bool SelfCheck() volatile const;
#endif
            void Destroy();
        };
        template<typename T>
        struct monoArray : BNM::IL2CPP::Il2CppObject  {
            IL2CPP::Il2CppArrayBounds *bounds{};
            IL2CPP::il2cpp_array_size_t capacity{};
            T m_Items[0];
            inline IL2CPP::il2cpp_array_size_t GetCapacity() const { BNM_CHECK_SELF(0); return capacity; }
            inline T *GetData() const { BNM_CHECK_SELF(nullptr); return (T * const) &m_Items[0]; }
            std::vector<T> ToVector() const {
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
            inline DataIterator<T> operator[] (IL2CPP::il2cpp_array_size_t index) const { BNM_CHECK_SELF({}); if (GetCapacity() < index) return {}; return &m_Items[index]; }
            inline DataIterator<T> At(IL2CPP::il2cpp_array_size_t index) const { BNM_CHECK_SELF({}); if (GetCapacity() < index) return {}; return &m_Items[index]; }
            inline bool Empty() const { BNM_CHECK_SELF(false); return GetCapacity() <= 0;}
            static monoArray<T> *Create(size_t capacity) {
                auto monoArr = (monoArray<T> *)malloc(sizeof(monoArray) + sizeof(T) * capacity);
                memset(monoArr, 0, sizeof(monoArray) + sizeof(T) * capacity);
                monoArr->klass = nullptr;
                monoArr->capacity = capacity;
                return monoArr;
            }
            static monoArray<T> *Create(const std::vector<T> &vec) { return Create((T *)vec.data(), vec.size()); }
            static monoArray<T> *Create(T *arr, size_t size) {
                monoArray<T> *monoArr = Create(size);
                monoArr->klass = nullptr;
                monoArr->CopyFrom(arr, size);
                return monoArr;
            }
            // Только для массивов созданных через BNM!
            inline void Destroy() { if (!klass) free(this); }
#ifdef BNM_ALLOW_SELF_CHECKS
            bool SelfCheck() volatile const {
                monoArray<T> *me = (monoArray<T> *)this;
                if (me) return true;
                BNM_LOG_ERR("[monoArray<>::SelfCheck] Попытка использовать мёртвый массив!");
                return false;
            }
#endif
        };
        template<typename T>
        struct monoList : BNM::IL2CPP::Il2CppObject {
            struct Enumerator {
                monoList<T> *list{};
                int index{};
                int version{};
                T current{};
                constexpr Enumerator() = default;
                Enumerator(monoList<T> *list) : Enumerator() { this->list = list; }

                // Оригинальный C# код не нужен: он бесполезен в C++
                // Поэтому тут просто код для поддержки C++ foreach
                inline T* begin() { return &list->items[0]; }
                inline T* end() { return &list->items->m_Items[list->size]; }
                inline T* begin() const { return &list->items[0]; }
                inline T* end() const { return &list->items->m_Items[list->size]; }
            };
            monoArray<T> *items{};
            int size{};
            int version{};
            void *syncRoot{};
            inline T *GetData() const { return items->GetData(); }
            inline int GetSize() const { return size; }
            inline int GetVersion() const { return version; }
            std::vector<T> ToVector() const {
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
            int IndexOf(T val) const {
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
                auto nItems = monoArray<T>::Create(newCapacity);
                nItems->klass = items->klass;
                nItems->monitor = items->monitor;
                nItems->bounds = items->bounds;
                nItems->capacity = newCapacity;
                if (items->capacity > 0) // Не копировать, если массив пустой
                    memcpy(&nItems->m_Items[0], &items->m_Items[0], items->capacity * sizeof(T));
                items = nItems;
                return true;
            }
            DataIterator<T> operator[] (int index) const { if (index >= size) return {}; return &items->m_Items[index]; }
            DataIterator<T> At(int index) const { if (index >= size) return {}; return &items->m_Items[index]; }
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
            bool Contains(T item) const { // Не совсем как в С# из-за его особенностей
                for (int i = 0; i < size; i++) if (items->m_Items[i] == item) return true;
                return false;
            }
            Enumerator GetEnumerator() { return this; }
            T get_Item(int index) const {
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
                if (index < size) memcpy(items->m_Items + index + 1, items->m_Items + index, (size - index) * sizeof(T));
                items->m_Items[index] = item;
                ++size;
                ++version;
            }
            void *get_SyncRoot() {
                if (!syncRoot) syncRoot = CompareExchange4List(syncRoot);
                return syncRoot;
            }
            bool get_false() { return false; }
            void CopyTo(monoArray<T>* arr, int arrIndex) {
                memcpy(items->m_Items, arr->m_Items + arrIndex, size * sizeof(T));
            }
            void GrowIfNeeded(int n) {
                if (size + n > items->capacity)
                    Resize(size + n);
            }
            void Shift(int start, int delta) {
                if (delta < 0) start -= delta;
                if (start < size) memcpy(items->m_Items + start + delta, items->m_Items + start, size - start);
                size += delta;
                if (delta < 0) memset(items->m_Items + size + delta, 0, -delta * sizeof(T));
            }
#ifdef BNM_ALLOW_SELF_CHECKS
            bool SelfCheck() volatile const {
                monoList<T> *me = (monoList<T> *)this;
                if (me) return true;
                BNM_LOG_ERR("[monoList<>::SelfCheck] Попытка использовать мёртвый список!");
                return false;
            }
#endif
        };

        // Основано на https://github.com/royvandam/rtti/tree/cf0dee6fb3999573f45b0726a8d5739022e3dacf
        namespace __PRIVATE_MonoListData {
            template <typename T> constexpr std::string_view WrappedTypeName() { return __PRETTY_FUNCTION__; }
            constexpr std::size_t WrappedTypeNamePrefixLength() { return WrappedTypeName<void>().find("void"); }
            constexpr std::size_t WrappedTypeNameSuffixLength() { return WrappedTypeName<void>().length() - WrappedTypeNamePrefixLength() - 4; }
            constexpr uint32_t FNV1a(const char* str, size_t n, uint32_t hash = 2166136261U) {
                return n == 0 ? hash : FNV1a(str + 1, n - 1, (hash ^ str[0]) * 19777619U);
            }
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
        }

        template<typename T>
        void InitMonoListVTable(monoList<T> *list) {
            using namespace __PRIVATE_MonoListData;
            /*
             *  Заменить таблицу виртуальных методов т. к. оригинальная - пустая.
             *  Так же это нужно для типов, которых нет в игре (не факт, что игра и без смогла бы их использовать, но код есть)
             */
            using Type = std::conditional_t<std::is_pointer_v<T>, void*, T>;
            constexpr auto RemoveAt = &monoList<Type>::RemoveAt; constexpr auto GetSize = &monoList<Type>::GetSize; constexpr auto Clear = &monoList<Type>::Clear;
            constexpr auto get_Item = &monoList<Type>::get_Item; constexpr auto set_Item = &monoList<Type>::set_Item; constexpr auto IndexOf = &monoList<Type>::IndexOf;
            constexpr auto Insert = &monoList<Type>::Insert; constexpr auto get_false = &monoList<Type>::get_false; constexpr auto Add = &monoList<Type>::Add;
            constexpr auto Contains = &monoList<Type>::Contains; constexpr auto CopyTo = &monoList<Type>::CopyTo; constexpr auto Remove = &monoList<Type>::Remove;
            constexpr auto GetEnumerator = &monoList<Type>::GetEnumerator; constexpr auto get_SyncRoot = &monoList<Type>::get_SyncRoot;
            static std::array<MethodData, 16> namesMap = {
                    MethodData{OBFUSCATE_BNM("RemoveAt"), *(void **)&RemoveAt}, MethodData{OBFUSCATE_BNM("get_Count"), *(void **)&GetSize},
                    MethodData{OBFUSCATE_BNM("Clear"), *(void **)&Clear}, MethodData{OBFUSCATE_BNM("get_Item"), *(void **)&get_Item},
                    MethodData{OBFUSCATE_BNM("set_Item"), *(void **)&set_Item}, MethodData{OBFUSCATE_BNM("IndexOf"), *(void **)&IndexOf},
                    MethodData{OBFUSCATE_BNM("Insert"), *(void **)&Insert}, MethodData{OBFUSCATE_BNM("get_IsReadOnly"), *(void **)&get_false},
                    MethodData{OBFUSCATE_BNM("get_IsFixedSize"), *(void **)&get_false}, MethodData{OBFUSCATE_BNM("get_IsSynchronized"), *(void **)&get_false},
                    MethodData{OBFUSCATE_BNM("Add"), *(void **)&Add}, MethodData{OBFUSCATE_BNM("Contains"), *(void **)&Contains},
                    MethodData{OBFUSCATE_BNM("CopyTo"), *(void **)&CopyTo}, MethodData{OBFUSCATE_BNM("Remove"), *(void **)&Remove},
                    MethodData{OBFUSCATE_BNM("GetEnumerator"), *(void **)&GetEnumerator}, MethodData{OBFUSCATE_BNM("get_SyncRoot"), *(void **)&get_SyncRoot}
            };
            list->klass = TryGetMonoListClass(HashedTypeName<Type>(), namesMap);
        }
    }
    // Просто определение структур для LoadClass
    struct FieldBase;
    struct MethodBase;
    struct PropertyBase;
    template<typename T = int>
    struct Field;
    template<typename T>
    struct Property;
    template<typename Ret>
    struct Method;
    struct RuntimeTypeGetter;

    struct LoadClass {
        IL2CPP::Il2CppClass *klass{};

        constexpr LoadClass() noexcept = default;

        LoadClass(const IL2CPP::Il2CppClass *clazz); // Из класса
        LoadClass(const IL2CPP::Il2CppObject *obj); // Из объекта
        LoadClass(const IL2CPP::Il2CppType *type); // Из типа
        LoadClass(const MonoType *type); // Из C#-типа
        LoadClass(RuntimeTypeGetter type); // Из BNM-типа

        LoadClass(const std::string_view &namespaze, const std::string_view &name); // Из имени класса
        LoadClass(const std::string_view &namespaze, const std::string_view &name, const std::string_view &dllName); // Из имён класса и dll
        LoadClass(const std::string_view &namespaze, const std::string_view &name, const IL2CPP::Il2CppImage *image); // Из имени класса и dll
        LoadClass(const std::string_view &namespaze, const std::string_view &name, const IL2CPP::Il2CppAssembly *assembly); // Из имени класса и dll

        std::vector<LoadClass> GetInnerClasses(bool includeParent = true) const; // Получить все вложенные классы
        std::vector<FieldBase> GetFields(bool includeParent = true) const; // Получить все поля
        std::vector<MethodBase> GetMethods(bool includeParent = true) const; // Получить все методы
        std::vector<PropertyBase> GetProperties(bool includeParent = true) const; // Получить все свойства

        MethodBase GetMethodByName(const std::string_view &name, int parameters = -1) const; // Получить метод по имени и кол-ву аргументов
        MethodBase GetMethodByName(const std::string_view &name, const std::vector<std::string_view> &parametersName) const; // Получить метод по имени и именам аргументов
        MethodBase GetMethodByName(const std::string_view &name, const std::vector<RuntimeTypeGetter> &parametersType) const; // Получить метод по имени и типам аргументов
        PropertyBase GetPropertyByName(const std::string_view &name); // Получить свойство по имени
        LoadClass GetInnerClass(const std::string_view &name) const; // Получить вложенный класс по имени
        FieldBase GetFieldByName(const std::string_view &name) const; // Получить поле по имени

        LoadClass GetParent() const; // Получить родительский класс

        LoadClass GetArray() const; // В класс массива (класс[])
        LoadClass GetPointer() const; // В адрес на класс (класс *)
        LoadClass GetReference() const; // В ссылку на класс (класс &)

        LoadClass GetGeneric(const std::vector<RuntimeTypeGetter> &templateTypes) const; // Класс <типы из списка>

        IL2CPP::Il2CppType *GetIl2CppType() const; // В il2cpp-тип

        MonoType *GetMonoType() const; // В C#-тип

        IL2CPP::Il2CppClass *GetIl2CppClass() const; // В il2cpp-класс

        BNM::RuntimeTypeGetter GetRuntimeType() const; // В BNM-тип

        // Быстрые операторы
        inline operator IL2CPP::Il2CppType *() const { return GetIl2CppType(); };
        inline operator MonoType *() const { return GetMonoType(); };
        inline operator IL2CPP::Il2CppClass *() const { return GetIl2CppClass(); };
        operator BNM::RuntimeTypeGetter() const;

        void *CreateNewInstance() const; // То же самое, что и new Object() в C#, но без вызова конструктора (.ctor)
#ifdef BNM_ALLOW_STR_METHODS
        std::string str() const; // Получить информацию о классе
#endif
        // То же самое, что и new Object[] в C#
        template<typename T>
        Structures::Mono::monoArray<T> *NewArray(IL2CPP::il2cpp_array_size_t length = 0) const {
            if (!klass) return nullptr;
            TryInit();
            return (BNM::Structures::Mono::monoArray<T> *) ArrayNew(klass, length);
        }

        // То же самое, что и List<Object>() в C#
        template<typename T>
        Structures::Mono::monoList<T> *NewList() const {
            if (!klass) return nullptr;
            TryInit();
            BNM::Structures::Mono::monoArray<T> *array = NewArray<T>(1);
            BNM::Structures::Mono::monoList<T> *lst = (BNM::Structures::Mono::monoList<T> *)NewListInstance();
            if (!lst) {
                BNM_LOG_ERR("Не удалось создать List для класса: %s", str().c_str());
                return nullptr;
            }
            lst->items = array;
            BNM::Structures::Mono::InitMonoListVTable(lst);
            return lst;
        }

        // Упаковать объект
        template<typename T, typename = std::enable_if<!std::is_pointer<T>::value>>
        IL2CPP::Il2CppObject *BoxObject(T obj) const {
            if (!klass) return nullptr;
            TryInit();
            return ObjBox(klass, (void *) obj);
        }

        // То же самое, что и new Object() в C# с вызовом конструктор по кол-ву аргументов
        template<typename ...Args>
        void *CreateNewObject(Args ...args) const { return CreateNewObjectArgs(sizeof...(Args), {}, args...); }

        // То же самое, что и new Object() в C# с вызовом конструктор по кол-ву аргументов или их именам
        template<typename ...Args>
        void *CreateNewObjectArgs(int args_count, const std::vector<std::string_view> &arg_names, Args ...args) const;

        // Проверка, жив ли LoadClass
        inline bool Valid() const noexcept { return klass != nullptr; }
        inline bool Alive() const noexcept { return Valid(); }
        inline operator bool() noexcept { return Valid(); }
        inline operator bool() const noexcept { return Valid(); }
    private: // Приватно) Просто вызовы il2cpp-методов
        void TryInit() const;
        static IL2CPP::Il2CppObject *ObjBox(IL2CPP::Il2CppClass*, void*);
        static IL2CPP::Il2CppArray *ArrayNew(IL2CPP::Il2CppClass*, IL2CPP::il2cpp_array_size_t);
        static void *NewListInstance();
    };

    // Для статических полей потоков
    namespace __PRIVATE_FieldUtils {
        void GetStaticValue(IL2CPP::FieldInfo *info, void *value);
        void SetStaticValue(IL2CPP::FieldInfo *info, void *value);
    }

#pragma pack(push, 1)
    struct FieldBase {
        IL2CPP::FieldInfo *myInfo{};
        IL2CPP::Il2CppObject *instance{};
        uint8_t init : 1 = false, isStatic : 1 = false, isThreadStatic : 1 = false;

        constexpr FieldBase() noexcept = default;
        FieldBase(IL2CPP::FieldInfo *info);
        FieldBase(const FieldBase &other);

        // Установить объект
        FieldBase &SetInstance(IL2CPP::Il2CppObject *val);

        // Получить информацию
        IL2CPP::FieldInfo *GetInfo() const;

        // Получить адрес (смещение)
        BNM_PTR GetOffset() const;

        // Получить указатель на поле
        void *GetFieldPointer() const;

#ifdef BNM_ALLOW_STR_METHODS
        // Получить данные о поле
        inline std::string str() const {
            if (init) return LoadClass(myInfo->parent).str() + OBFUSCATE_BNM(".(") + myInfo->name + OBFUSCATE_BNM(")");
            return OBFUSCATE_BNM("Мёртвое поле");
        }
#endif

        // Быстрая установка объекта
        inline FieldBase &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline FieldBase &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline FieldBase &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        // Проверить, живо ли поле
        inline bool Initialized() const noexcept { return init; }

        // Получить поле с указанным типом
        template<typename NewType> inline Field<NewType> &cast() const { return (Field<NewType> &)*this; }
    };
    template<typename T>
    struct Field : public FieldBase {
        constexpr Field() noexcept = default;
        template<typename OtherType>
        Field(const Field<OtherType> &other) : FieldBase(other) {}
        Field(IL2CPP::FieldInfo *info) : FieldBase(info) {}
        Field(const FieldBase &other) : FieldBase(other) {}

        // Получить указатель на поле
        inline T *GetPointer() const {
            auto ptr = GetFieldPointer();
            BNM_LOG_ERR_IF(ptr == nullptr, "Пустой указатель на поле [%s]", init ? str().c_str() : "Мертво");
            return (T *)ptr;
        }

        // Получить значение из поля
        T Get() const {
            if (!init) return {};
            if (isThreadStatic) {
                T val{};
                __PRIVATE_FieldUtils::GetStaticValue(myInfo, (void *)&val);
                return val;
            }
            if (auto ptr = GetPointer(); ptr != nullptr) return *ptr;
            return {};
        }
        operator T() const { return Get(); }
        T operator()() const { return Get(); }

        // Изменить значение поля
        void Set(T val) const {
            if (!init) return;
            if (isThreadStatic) {
                __PRIVATE_FieldUtils::SetStaticValue(myInfo, (void *)&val);
                return;
            }
            if (auto ptr = GetPointer(); ptr != nullptr) *ptr = val;
        }
        inline Field<T> &operator=(T val) { Set(std::move(val)); return *this; }
        inline Field<T> &operator=(T val) const { Set(std::move(val)); return *this; }

        // Быстрая установка объекта
        inline Field<T> &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline Field<T> &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline Field<T> &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        // Скопировать другое поле, только для автоматического приведения типов
        Field<T> &operator =(const FieldBase &other) {
            myInfo = other.myInfo;
            instance = other.instance;
            init = other.init;
            isStatic = other.isStatic;
            isThreadStatic = other.isThreadStatic;
            return *this;
        }
    };

    struct MethodBase {
        IL2CPP::MethodInfo *myInfo{};
        IL2CPP::Il2CppObject *instance{};
        uint8_t init : 1 = false, isStatic : 1 = false, isVirtual : 1 = false;

        constexpr MethodBase() noexcept = default;
        MethodBase(const IL2CPP::MethodInfo *info);
        MethodBase(const MethodBase &other);

        // Установить объект
        MethodBase &SetInstance(IL2CPP::Il2CppObject *val);

        // Получить информацию
        IL2CPP::MethodInfo *GetInfo() const;

        // Получить адрес (смещение)
        BNM_PTR GetOffset() const;

        // Если метод является `generic`, то можно попытаться получить его с определённым набором типов
        MethodBase GetGeneric(const std::vector<RuntimeTypeGetter> &templateTypes) const;

        // Быстрая установка объекта
        inline MethodBase &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline MethodBase &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline MethodBase &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        // Проверить, жив ли метод
        inline bool Initialized() const noexcept { return init; }
        inline operator bool() noexcept { return Initialized(); }
        inline operator bool() const noexcept { return Initialized(); }

#ifdef BNM_ALLOW_STR_METHODS
        // Получить данные
        inline std::string str() const {
#if UNITY_VER > 174
#define kls klass
#else
#define kls declaring_type
#endif
            if (!init) return OBFUSCATE_BNM("Мёртвый метод");
            return LoadClass(myInfo->return_type).str() + OBFUSCATE_BNM(" ") +
                LoadClass(myInfo->kls).str() + OBFUSCATE_BNM(".(") +
                myInfo->name + OBFUSCATE_BNM("){кол-во аргументов: ") +
                std::to_string(myInfo->parameters_count) + OBFUSCATE_BNM("}") +
                (isStatic ? OBFUSCATE_BNM("(статический)") : OBFUSCATE_BNM(""));
#undef kls
        }
#endif

        // Получить метод с указанным типом
        template<typename NewType> inline Method<NewType> &cast() const { return (Method<NewType> &)*this; }
    };
    template<typename Ret = void>
    struct Method : public MethodBase {
        constexpr Method() noexcept = default;
        template<typename OtherType>
        Method(const Method<OtherType> &other) : MethodBase(other) {}
        Method(const IL2CPP::MethodInfo *info) : MethodBase(info) {}
        Method(const MethodBase &other) : MethodBase(other) {}

        // Быстрая установка объекта
        inline Method<Ret> &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline Method<Ret> &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline Method<Ret> &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        // Вызвать метод
        template<typename ...Args>
        Ret Call(Args...args) const {
            if (!init) { if constexpr (std::is_same<Ret, void>::value) return; else return {}; }
            bool canInfo = true;
            if (sizeof...(Args) != myInfo->parameters_count){
                canInfo = false;
                BNM_LOG_WARN("Попытка вызвать %s с неправильным кол-вом аргументов... Я надеюсь, вы знаете, что делаете. BNM не может добавить MethodInfo к аргументам :(. Пожалуйста, исправьте это.", str().c_str());
            }
            if (!isStatic && !CheckObj(instance)) {
                BNM_LOG_ERR("Нельзя вызвать нестатический метод %s без объекта! Пожалуйста, установите объект перед вызовом метода.", str().c_str());
                if constexpr (std::is_same_v<Ret, void>) return; else return {};
            }
            auto method = myInfo;
            if (isVirtual && !isStatic) method = (IL2CPP::MethodInfo *)instance->klass->vtable[myInfo->slot].method;
            if (!method || strcmp(method->name, myInfo->name)) method = myInfo;
            if (!isStatic) {
                if (canInfo) return (((Ret(*)(IL2CPP::Il2CppObject *, Args..., IL2CPP::MethodInfo *)) method->methodPointer)(instance, args..., method));
                return (((Ret(*)(IL2CPP::Il2CppObject *, Args...)) method->methodPointer)(instance, args...));
            }
#if UNITY_VER > 174
            if (canInfo) return ((Ret(*)(Args..., IL2CPP::MethodInfo *)) method->methodPointer)(args..., method);
            return (((Ret(*)(Args...)) method->methodPointer)(args...));
#else
            if (canInfo) return ((Ret(*)(void*,Args...,IL2CPP::MethodInfo *))method->methodPointer)(nullptr, args..., method);
            return (((Ret(*)(void*,Args...))method->methodPointer)(nullptr, args...));
#endif
        }

        // Быстро вызвать метод
        template<typename ...Args> inline Ret operator ()(Args ...args) { return Call(args...); }
        template<typename ...Args> inline Ret operator ()(Args ...args) const { return Call(args...); }

        // Скопировать другой метод, только для автоматического приведения типов
        Method<Ret> &operator =(const MethodBase &other) {
            myInfo = other.myInfo;
            instance = other.instance;
            init = other.init;
            isStatic = other.isStatic;
            isVirtual = other.isVirtual;
            return *this;
        }
    };
    struct PropertyBase {
        IL2CPP::PropertyInfo *myInfo{};
        MethodBase getter{}, setter{};
        uint8_t hasGetter : 1 = false, hasSetter : 1 = false;

        constexpr PropertyBase() noexcept= default;
        PropertyBase(const IL2CPP::PropertyInfo *info);
        PropertyBase(const PropertyBase &other);
        PropertyBase(const MethodBase &newGetter, const MethodBase &newSetter);

        // Установить объект
        PropertyBase &SetInstance(IL2CPP::Il2CppObject *val);
#ifdef BNM_ALLOW_STR_METHODS
        // Получить данные
        inline std::string str() const {
            if (!hasGetter && !hasSetter) return OBFUSCATE_BNM("Мёртвое свойство");
            auto isStatic = hasGetter ? getter.isStatic : setter.isStatic;
            return LoadClass(myInfo->parent).str() + OBFUSCATE_BNM(" ") +
                LoadClass(myInfo->parent).str() + OBFUSCATE_BNM(".(") +
                ((Structures::Mono::monoString *)myInfo->name)->str() + OBFUSCATE_BNM("){метод получения: ") + (hasGetter ? OBFUSCATE_BNM("есть") : OBFUSCATE_BNM("нет")) + OBFUSCATE_BNM(", метод установки: ") + (hasSetter ? OBFUSCATE_BNM("есть") : OBFUSCATE_BNM("нет")) + OBFUSCATE_BNM("}") +
                (isStatic ? OBFUSCATE_BNM("(статическое)") : OBFUSCATE_BNM(""));
        }
#endif

        // Быстрая установка объекта
        inline PropertyBase &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline PropertyBase &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline PropertyBase &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        // Изменить тип свойства
        template<typename NewRet> inline Property<NewRet> &cast() const { return (Property<NewRet> &)*this; }
    };

    template<typename T = bool>
    struct Property : PropertyBase {
        constexpr Property() noexcept = default;
        template<typename OtherType>
        Property(const Property<OtherType> &other) : PropertyBase(other) {}
        Property(const IL2CPP::PropertyInfo *info) : PropertyBase(info) {}
        Property(const PropertyBase &other) : PropertyBase(other) {}

        // Быстрая установка объекта
        inline Property<T> &operator[](void *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}
        inline Property<T> &operator[](IL2CPP::Il2CppObject *val) { SetInstance(val); return *this;}
        inline Property<T> &operator[](UnityEngine::Object *val) { SetInstance((IL2CPP::Il2CppObject *)val); return *this;}

        // Вызвать получатель
        inline T Get() const {
            if (hasGetter) return getter.cast<T>()();
            BNM_LOG_ERR("Попытка вызвать свойство %s в котором отсутствует метод получения", str().c_str());
            if constexpr (std::is_same_v<T, void>) return; else return {};
        }
        inline operator T() { return Get(); }
        inline T operator()() { return Get(); }
        inline operator T() const { return Get(); }
        inline T operator()() const { return Get(); }

        // Вызвать установщик
        inline void Set(T v) const {
            if (hasSetter) return setter.cast<void>()(v);
            BNM_LOG_ERR("Попытка вызвать свойство %s в котором отсутствует метод установки", str().c_str());
        }

        inline Property<T> &operator=(T val) { Set(std::move(val)); return *this; }
        inline Property<T> &operator=(T val) const { Set(std::move(val)); return *this; }

        // Проверить, живо ли свойство
        inline bool Initialized() const noexcept { return hasGetter || hasSetter; }

        // Скопировать другое свойство, только для автоматического приведения типов
        Property<T> &operator =(const PropertyBase &other) {
            myInfo = other.myInfo;
            getter = other.getter;
            setter = other.setter;
            hasGetter = other.hasGetter;
            hasSetter = other.hasSetter;
            return *this;
        }
    };
    // Ни в коем случае не трогать структуры il2cpp
#pragma pack(pop)

    namespace Operators {
        template<typename P, typename T, typename = std::enable_if<std::is_pointer_v<P>>>
        inline DataIterator<T> operator ->*(const P pointer, Field<T> &field) {
            field[(IL2CPP::Il2CppObject *)pointer];
            return field.GetPointer();
        }
        template<typename P, typename T, typename = std::enable_if<std::is_pointer_v<P>>>
        inline Method<T> &operator ->*(const P pointer, Method<T> &method) {
            method[(IL2CPP::Il2CppObject *)pointer];
            return method;
        }
        template<typename P, typename T, typename = std::enable_if<std::is_pointer_v<P>>>
        inline Property<T> &operator ->*(const P pointer, Property<T> &property) {
            property[(IL2CPP::Il2CppObject *)pointer];
            return property;
        }
        template<typename P, typename T, typename = std::enable_if<std::is_pointer_v<P>>>
        inline Field<T> &operator >>(const P pointer, Field<T> &field) {
            field[(IL2CPP::Il2CppObject *)pointer];
            return field;
        }
        template<typename P, typename T, typename = std::enable_if<std::is_pointer_v<P>>>
        inline Method<T> &operator >>(const P pointer, Method<T> &method) {
            method[(IL2CPP::Il2CppObject *)pointer];
            return method;
        }
        template<typename P, typename T, typename = std::enable_if<std::is_pointer_v<P>>>
        inline Property<T> &operator >>(const P pointer, Property<T> &property) {
            property[(IL2CPP::Il2CppObject *)pointer];
            return property;
        }
    }

    template<typename ...Args>
    void *LoadClass::CreateNewObjectArgs(int args_count, const std::vector<std::string_view> &arg_names, Args ...args) const {
        if (!klass) return nullptr;
        TryInit();
        auto name = OBFUSCATE_BNM(".ctor");
        auto method = arg_names.empty() ? GetMethodByName(name, args_count) : GetMethodByName(name, arg_names);
        auto instance = CreateNewInstance();
        method.cast<void>()[instance](args...);
        return instance;
    }

    // Обычный C#-словарь (Dictionary)
    namespace Structures::Mono {
        template<typename TKey, typename TValue>
        struct monoDictionary : BNM::IL2CPP::Il2CppObject {
#ifdef BNM_DOTNET35
            struct Link { int HashCode{}, Next{}; };
            monoArray<int> *table{};
            monoArray<Link> *linkSlots{};
            monoArray<TKey> *keys{};
            monoArray<TValue> *values{};
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
            monoArray<int> *buckets{};
            monoArray<Entry> *entries{};
            int count{};
            int version{};
            int freeList{};
            int freeCount{};
            void *comparer{};
            monoArray<TKey> *keys{};
            monoArray<TValue> *values{};
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
            bool TryGet(TKey key, TValue *value) const { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("TryGetValue"), 2).template cast<bool>()[(void *)this](key, value); }
            void Add(TKey key, TValue value) { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("Add"), 2).template cast<void>()[(void *)this](key, value); }
            void Insert(TKey key, TValue value) { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("set_Item"), 2).template cast<void>()[(void *)this](key, value); }
            bool Remove(TKey key) { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("Remove"), 1).template cast<bool>()[(void *)this](key); }
            bool ContainsKey(TKey key) const { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("ContainsKey"), 1).template cast<bool>()[(void *)this](key); }
            bool ContainsValue(TValue value) const { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("ContainsValue"), 1).template cast<bool>()[(void *)this](value); }
            TValue Get(TKey key) const {
                TValue ret;
                if (TryGet(key, &ret)) return ret;
                return {};
            }
            TValue operator[](TKey key) const { return Get(key); }
        };
    }

#pragma pack(push, 1)
    // Структура для сохранения данных для их поиска во время выполнения кода
    struct RuntimeTypeGetter {
        enum class RuntimeModifier : uint8_t {
            None, Array, Pointer, Reference
        };
        const char *namespaze{}, *name{};
        RuntimeModifier modifier = RuntimeModifier::None;
        LoadClass loadedClass{};
        LoadClass ToLC();
        IL2CPP::Il2CppType *ToIl2CppType();
        IL2CPP::Il2CppClass *ToIl2CppClass();
        LoadClass ToLC() const;
        IL2CPP::Il2CppType *ToIl2CppType() const;
        IL2CPP::Il2CppClass *ToIl2CppClass() const;
        operator IL2CPP::Il2CppType*();
        operator IL2CPP::Il2CppClass*();
        operator LoadClass();
        operator IL2CPP::Il2CppType*() const;
        operator IL2CPP::Il2CppClass*() const;
        operator LoadClass() const;
    };
#pragma pack(pop)

    // Сохранить название типа во время компиляции
    constexpr RuntimeTypeGetter GetType(const char *namespaze, const char *name, RuntimeTypeGetter::RuntimeModifier modifier = RuntimeTypeGetter::RuntimeModifier::None) noexcept { return {.namespaze = namespaze, .name = name, .modifier = modifier}; }

    // Сохранить имя базовых типов во время компиляции
    template<typename T>
    constexpr RuntimeTypeGetter GetType(RuntimeTypeGetter::RuntimeModifier modifier = RuntimeTypeGetter::RuntimeModifier::None) noexcept {
        using namespace Structures::Unity;
        using namespace Structures::Mono;
        if (std::is_same<T, void>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Void"), modifier};
        else if (std::is_same<T, bool>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Boolean"), modifier};
        else if (std::is_same<T, uint8_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Byte"), modifier};
        else if (std::is_same<T, int8_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("SByte"), modifier};
        else if (std::is_same<T, int16_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Int16"), modifier};
        else if (std::is_same<T, uint16_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("UInt16"), modifier};
        else if (std::is_same<T, int32_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Int32"), modifier};
        else if (std::is_same<T, uint32_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("UInt32"), modifier};
        else if (std::is_same<T, intptr_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("IntPtr"), modifier};
        else if (std::is_same<T, int64_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Int64"), modifier};
        else if (std::is_same<T, uint64_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("UInt64"), modifier};
        else if (std::is_same<T, float>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Single"), modifier};
        else if (std::is_same<T, double>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Double"), modifier};
        else if (std::is_same<T, BNM::IL2CPP::Il2CppString *>::value || std::is_same<T, monoString *>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("String"), modifier};
        else if (std::is_same<T, Vector3>::value)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Vector3"), modifier};
        else if (std::is_same<T, Vector2>::value)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Vector2"), modifier};
        else if (std::is_same<T, Color>::value)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Color"), modifier};
        else if (std::is_same<T, Ray>::value)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Ray"), modifier};
        else if (std::is_same<T, RaycastHit>::value)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("RaycastHit"), modifier};
        else if (std::is_same<T, Quaternion>::value)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Quaternion"), modifier};
        else if (std::is_same<T, BNM::UnityEngine::Object *>::value)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Object"), modifier};
        return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Object"), modifier};
    }

#if !BNM_DISABLE_NEW_CLASSES
    // Структуры для добавления методов и полей в классы
    namespace MODIFY_CLASSES {
        // Данные о новых методах
        struct AddableMethod {
            constexpr AddableMethod() noexcept = default;
            IL2CPP::MethodInfo *myInfo{};
            virtual uint8_t GetArgsCount() = 0;
            virtual void *GetAddress() = 0;
            virtual void *GetInvoker() = 0;
            virtual const char *GetName() = 0;
            virtual RuntimeTypeGetter GetRetType() = 0;
            virtual std::vector<RuntimeTypeGetter> GetArgTypes() = 0;
            virtual bool IsStatic() = 0;
        };

        // Данные о новых полях
        struct AddableField {
            constexpr AddableField() noexcept = default;
            IL2CPP::FieldInfo *myInfo{};
            BNM_PTR offset{};
            virtual const char *GetName() = 0;
            virtual size_t GetSize() = 0;
            virtual RuntimeTypeGetter GetType() = 0;
        };

        struct TargetModifier {
            RuntimeTypeGetter (*newParentGetter)(){};
            RuntimeTypeGetter (*newOwnerGetter)(){};
        };

        // Данные о целевом классе
        struct TargetClass {
            constexpr TargetClass() noexcept = default;
            virtual RuntimeTypeGetter GetTargetType() = 0;
            virtual std::vector<AddableField *> GetFields() = 0;
            virtual std::vector<AddableMethod *> GetMethods() = 0;
            virtual TargetModifier &GetTargetModifier() = 0;
        };

        // Добавить целевой класс в список
        void AddTargetClass(TargetClass *klass);
    }

    // Структуры для создания новых классов
    namespace NEW_CLASSES {
        // Данные о новых методах
        struct NewMethod {
            constexpr NewMethod() noexcept = default;
            IL2CPP::MethodInfo *myInfo{};
            virtual uint8_t GetArgsCount() = 0;
            virtual void *GetAddress() = 0;
            virtual void *GetInvoker() = 0;
            virtual const char *GetName() = 0;
            virtual RuntimeTypeGetter GetRetType() = 0;
            virtual std::vector<RuntimeTypeGetter> GetArgTypes() = 0;
            virtual bool IsStatic() = 0;
        };

        // Данные о новых полях
        struct NewField {
            constexpr NewField() noexcept = default;
            IL2CPP::FieldInfo *myInfo{};
            virtual const char *GetName() = 0;
            virtual size_t GetOffset() = 0;
            virtual size_t GetSize() = 0;
            virtual unsigned int GetAttributes() = 0;
            virtual RuntimeTypeGetter GetType() = 0;
            virtual BNM_PTR GetCppOffset() = 0;
        };

        typedef RuntimeTypeGetter(*OwnerGetter)();

        // Данные о новых классах
        struct NewClass {
            constexpr NewClass() noexcept = default;
            IL2CPP::Il2CppClass *myClass{};
            MonoType *type{};
            virtual size_t GetSize() = 0;
            virtual const char *GetName() = 0;
            virtual const char *GetNamespace() = 0;
            virtual const char *GetDllName() = 0;
            virtual RuntimeTypeGetter GetBaseType() = 0;
            virtual std::vector<RuntimeTypeGetter> GetInterfaces() = 0;
            virtual std::vector<NewField *> GetFields() = 0;
            virtual std::vector<NewMethod *> GetMethods() = 0;
            virtual OwnerGetter GetOwnerGetter() = 0;
        };

        // Добавить новый класс в список
        void AddNewClass(NEW_CLASSES::NewClass *klass);

        // Распаковать аргумент
        template<typename T>
        constexpr inline T UnpackArg(void *arg) {
            if constexpr (std::is_pointer<T>::value) return (T) arg;
            else return *(T *)arg;
        }

        template<typename T>
        inline void *PackArg(T arg) {
            if constexpr (std::is_pointer<T>::value) return arg;
            static auto cls = BNM::GetType<T>();
            return cls.ToLC().BoxObject(&arg);
        }

        template<typename> struct GetNewMethodCalls {};
        template<typename> struct GetNewStaticMethodCalls {};

        // Класс для создания инициатора вызовов для методов
        template<typename RetT, typename T, typename ...ArgsT>
        struct GetNewMethodCalls<RetT(T:: *)(ArgsT...)> {
            template<std::size_t ...As>
            static void *InvokeMethod(RetT(*func)(T*, ArgsT...), T *instance, void **args, std::index_sequence<As...>) {
                if constexpr (std::is_same<RetT, void>::value) {
                    func(instance, UnpackArg<ArgsT>(args[As])...);
                    return nullptr;
                } else return PackArg(func(instance, UnpackArg<ArgsT>(args[As])...));
            }
            static void *invoke(IL2CPP::Il2CppMethodPointer ptr, IL2CPP::MethodInfo *m, void *obj, void **args) {
                auto func = (RetT(*)(T*, ArgsT...))(ptr);
                auto instance = (T *)(obj);
                auto seq = std::make_index_sequence<sizeof...(ArgsT)>();
                return InvokeMethod(func, instance, args, seq);
            }
        };

#if UNITY_VER > 174
        // Класс для создания инициатора вызовов для статических методов
        template<typename RetT, typename ...ArgsT>
        struct GetNewStaticMethodCalls<RetT(*)(ArgsT...)> {
            template<std::size_t ...As>
            static void *InvokeMethod(RetT(*func)(ArgsT...), void **args, std::index_sequence<As...>) {
                if constexpr (std::is_same<RetT, void>::value) {
                    func(UnpackArg<ArgsT>(args[As])...);
                    return nullptr;
                } else return func(UnpackArg<ArgsT>(args[As])...);
            }
            static void *invoke(IL2CPP::Il2CppMethodPointer ptr, IL2CPP::MethodInfo *m, void *obj, void **args) {
                auto func = (RetT(*)(ArgsT...))(ptr);
                auto seq = std::make_index_sequence<sizeof...(ArgsT)>();
                return InvokeMethod(func, args, seq);
            }
        };
#else
        template<typename RetT, typename ...ArgsT>
        struct GetNewStaticMethodCalls<RetT(*)(ArgsT...)> {
            template<std::size_t ...As>
            static void *InvokeMethod(RetT(*func)(void *, ArgsT...), void **args, std::index_sequence<As...>) {
                if constexpr (std::is_same_v<RetT, void>) {
                    func(nullptr, UnpackArg<ArgsT>(args[As])...);
                    return nullptr;
                } else return func(nullptr, UnpackArg<ArgsT>(args[As])...);
            }
            static void *invoke(IL2CPP::Il2CppMethodPointer ptr, IL2CPP::MethodInfo *m, void *obj, void **args) {
                auto func = (RetT(*)(void *, ArgsT...))(ptr);
                auto seq = std::make_index_sequence<sizeof...(ArgsT)>();
                return InvokeMethod(func, args, seq);
            }
        };
#endif
    }
#endif

    // Метод создания C#-строк собираемых сборщиком мусора, если он включён в игре
    Structures::Mono::monoString *CreateMonoString(const char *str);
    Structures::Mono::monoString *CreateMonoString(const std::string_view &str);

    // Получить внешние методы (icall)
    void *GetExternMethod(const std::string_view &str);

    // Истина, когда il2cpp и BNM загружены
    bool IsLoaded();

#ifdef BNM_DEPRECATED
    // Утилиты потоков il2cpp
    bool AttachIl2Cpp();
    IL2CPP::Il2CppThread *CurrentIl2CppThread();
    void DetachIl2Cpp();
#endif

    // Получить путь к libil2cpp.so
    std::string_view GetIl2CppLibraryPath();

    // Получить адрес (смещение) загруженного libil2cpp.so
    BNM_PTR GetIl2CppLibraryOffset();

    // Не закрывать его! BNM вызовет сбой из-за этого.
    void *GetIl2CppLibraryHandle();

    // Распаковка объекта, просто копия метода из il2cpp
    template<typename T>
    static T UnboxObject(T obj) { return (T)(void *)(((char *)obj) + sizeof(BNM::IL2CPP::Il2CppObject)); }

    // Подмена метода путем изменения MethodInfo
    bool InvokeHookImpl(IL2CPP::MethodInfo *m, void *newMet, void **oldMet);

    template<typename T_NEW, typename T_OLD>
    bool InvokeHook(const BNM::MethodBase &targetMethod, T_NEW newMet, T_OLD &oldMet) {
        if (targetMethod.Initialized()) return InvokeHookImpl(targetMethod.myInfo, (void *)newMet, (void **)&oldMet);
        return false;
    }
    template<typename T_NEW, typename T_OLD>
    bool InvokeHook(IL2CPP::MethodInfo *m, T_NEW newMet, T_OLD &oldMet) { return InvokeHookImpl(m, (void *)newMet, (void **)&oldMet); }

    // Подмена метода путем изменения таблицы виртуальных методов класса
    bool VirtualHookImpl(BNM::LoadClass targetClass, IL2CPP::MethodInfo *m, void *newMet, void **oldMet);

    template<typename T_NEW, typename T_OLD>
    bool VirtualHook(BNM::LoadClass targetClass, const BNM::MethodBase &targetMethod, T_NEW newMet, T_OLD &oldMet) {
        if (targetClass && targetMethod.Initialized()) return VirtualHookImpl(targetClass, targetMethod.myInfo, (void *)newMet, (void **)&oldMet);
        return false;
    }
    template<typename T_NEW, typename T_OLD>
    bool VirtualHook(BNM::LoadClass targetClass, IL2CPP::MethodInfo *m, T_NEW newMet, T_OLD &oldMet) {
        return VirtualHookImpl(targetClass, m, (void *)newMet, (void **)&oldMet);
    }


    // Методы проверки класса объекта
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, IL2CPP::Il2CppClass *klass) { return IsA<BNM::IL2CPP::Il2CppObject *>((IL2CPP::Il2CppObject *)object, klass); }
    template<> bool IsA<IL2CPP::Il2CppObject *>(IL2CPP::Il2CppObject *object, IL2CPP::Il2CppClass *klass);
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, LoadClass klass) { return IsA(object, klass.klass); }
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, IL2CPP::Il2CppObject *klass) { if (!klass) return false; return IsA(object, klass->klass); }
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, MonoType *type) { return IsA(object, LoadClass(type)); }

    template<typename NEW_T, typename T_OLD>
    void HOOK(const BNM::MethodBase &targetMethod, NEW_T newMethod, T_OLD &oldBytes) {
        if (targetMethod.Initialized()) ::HOOK<void *, NEW_T, T_OLD>((void *) targetMethod.myInfo->methodPointer, newMethod, oldBytes);
    };
    // Попробовать предварительно загрузить библиотеку, получив полный путь до неё (ТОЛЬКО ВНУТРЕННЕЕ ИСПОЛЬЗОВАНИЕ)
    void TryForceLoadIl2CppByPath(JNIEnv *env, jobject context = nullptr);
    namespace External {
        bool TryInitHandle(void *handle, const char *path = nullptr, bool external = true);
        // Попробовать загрузить BNM, если вы используете BNM извне
        // Нужно вызвать это из любого потока unity
        // handle - дескриптор загруженной libil2cpp.so
        void TryLoad(void *handle);
        // Установить дескриптор libil2cpp.so, не проверяя его, и попробовать загрузить BNM
        // GetIl2CppLibraryOffset и GetIl2CppLibraryPath будут пустыми
        // Нужно вызвать это из любого потока unity
        // handle - дескриптор загруженной libil2cpp.so
        void ForceLoad(void *handle);
    }
    void AddOnLoadedEvent(void (*event)());
    void ClearOnLoadedEvents();

#ifdef BNM_DEBUG
    namespace Utils {
        // Преобразует адрес (смещение) в памяти в адрес (смещение) в библиотеке (работает для любой библиотеки)
        void *OffsetInLib(void *offsetInMemory);
    }
#endif
}
#define InitResolveFunc(x, y) BNM::InitFunc(x, BNM::GetExternMethod(y))

#if !BNM_DISABLE_NEW_CLASSES

#define BNM_ModClassInit(_cppCls, _getTargetClass) \
    private: \
    struct _BNMModClass : BNM::MODIFY_CLASSES::TargetClass { \
        _BNMModClass() noexcept { \
            BNM::MODIFY_CLASSES::AddTargetClass(this); \
        } \
        static inline BNM::MODIFY_CLASSES::TargetModifier targetModifier{}; \
        static inline std::vector<BNM::MODIFY_CLASSES::AddableField *> myFields{}; \
        static inline std::vector<BNM::MODIFY_CLASSES::AddableMethod *> myMethods{}; \
        BNM::RuntimeTypeGetter GetTargetType() override _getTargetClass \
        std::vector<BNM::MODIFY_CLASSES::AddableField *> GetFields() override { return myFields; } \
        std::vector<BNM::MODIFY_CLASSES::AddableMethod *> GetMethods() override { return myMethods; } \
        BNM::MODIFY_CLASSES::TargetModifier &GetTargetModifier() override { return targetModifier; } \
        static void AddField(BNM::MODIFY_CLASSES::AddableField *f) { myFields.push_back(f); } \
        static void AddMethod(BNM::MODIFY_CLASSES::AddableMethod *f) { myMethods.push_back(f); } \
        static void SetParrentGetter(BNM::RuntimeTypeGetter (*newParentGetter)()) { targetModifier.newParentGetter = newParentGetter; } \
        static void SetOwnerGetter(BNM::RuntimeTypeGetter (*newOwnerGetter)()) { targetModifier.newOwnerGetter = newOwnerGetter; } \
    }; \
    public: \
    static inline _BNMModClass BNMModClass{}; \
    using ModBNMType = _cppCls

#define BNM_ModAddMethod(_retType, _name, _argsCount, ...) \
    private: \
    struct _BNMModMethod_##_name : BNM::MODIFY_CLASSES::AddableMethod { \
        _BNMModMethod_##_name() noexcept { \
            _BNMModClass::AddMethod(this); \
        } \
        uint8_t GetArgsCount() override { return _argsCount; } \
        void *GetAddress() override { auto p = &ModBNMType::_name; return *(void **)&p; } \
        void *GetInvoker() override { return (void *)&BNM::NEW_CLASSES::GetNewMethodCalls<decltype(&ModBNMType::_name)>::invoke; } \
        const char *GetName() override { return OBFUSCATE_BNM(#_name); } \
        BNM::RuntimeTypeGetter GetRetType() override { return _retType; } \
        std::vector<BNM::RuntimeTypeGetter> GetArgTypes() override { return {__VA_ARGS__}; } \
        bool IsStatic() override { return false; } \
    }; \
    public: \
    static inline _BNMModMethod_##_name BNMModMethod_##_name{}

#define BNM_ModAddStaticMethod(_retType, _name, _argsCount, ...) \
    private: \
    struct _BNMModMethod_##_name : BNM::MODIFY_CLASSES::AddableMethod { \
        _BNMModMethod_##_name() noexcept { \
            _BNMModClass::AddMethod(this); \
        } \
        uint8_t GetArgsCount() override { return _argsCount; } \
        void *GetAddress() override { return (void *)&ModBNMType::_name; } \
        void *GetInvoker() override { return (void *)&BNM::NEW_CLASSES::GetNewStaticMethodCalls<decltype(&ModBNMType::_name)>::invoke; } \
        const char *GetName() override { return OBFUSCATE_BNM(#_name); } \
        BNM::RuntimeTypeGetter GetRetType() override { return _retType; } \
        std::vector<BNM::RuntimeTypeGetter> GetArgTypes() override { return {__VA_ARGS__}; } \
        bool IsStatic() override { return true; } \
    }; \
    public: \
    static inline _BNMModMethod_##_name BNMModMethod_##_name{}

#define BNM_ModAddField(_name, _type) \
    private: \
    struct _BNMModField_##_name : BNM::MODIFY_CLASSES::AddableField { \
        _BNMModField_##_name() noexcept { \
            _BNMModClass::AddField(this); \
        } \
        const char *GetName() override { return OBFUSCATE_BNM(#_name); } \
        size_t GetSize() override { return sizeof(decltype(_name)); } \
        BNM::RuntimeTypeGetter GetType() override { return _type; } \
    }; \
    public: \
    static inline _BNMModField_##_name BNMModField_##_name{}

#define BNM_ModNewParent(_getParentCode) \
    private: \
    struct _BNMModNewParent { \
        _BNMModNewParent() noexcept { \
            _BNMModClass::SetParrentGetter(GetNewParent); \
        } \
        static BNM::RuntimeTypeGetter GetNewParent() _getParentCode \
    }; \
    public: \
    static inline _BNMModNewParent BNMModNewParent{}

#define BNM_ModNewOwner(_getOwnerCode) \
    private: \
    struct _BNMModNewOwner { \
        _BNMModNewOwner() noexcept { \
            _BNMModClass::SetOwnerGetter(GetNewOwner); \
        } \
        static BNM::RuntimeTypeGetter GetNewOwner() _getOwnerCode \
    }; \
    public: \
    static inline _BNMModNewOwner BNMModNewOwner{}

#define BNM_NewClassInit(_namespace, _name, _getBaseCode, ...) BNM_NewClassWithDllInit("Assembly-CSharp", _namespace, _name, _getBaseCode, __VA_ARGS__)
#define BNM_NewMethodInit(_retType, _name, _argsCount, ...) BNM_NewMethodInitCustomName(_retType, _name, #_name, _argsCount, __VA_ARGS__)
#define BNM_NewStaticMethodInit(_retType, _name, _argsCount, ...) BNM_NewStaticMethodInitCustomName(_retType, _name, #_name, _argsCount, __VA_ARGS__)
#define BNM_NewConstructorInit(_name, _argsCount, ...) BNM_NewMethodInitCustomName(BNM::GetType<void>(), _name, ".ctor", _argsCount, __VA_ARGS__)

#define BNM_NewMethodInitCustomName(_retType, _met, _name, _argsCount, ...) \
    private: \
    struct _BNMMethod_##_met : BNM::NEW_CLASSES::NewMethod { \
        _BNMMethod_##_met() noexcept { \
            _BNMClass::AddNewMethod(this); \
        } \
        uint8_t GetArgsCount() override { return _argsCount; } \
        void *GetAddress() override { auto p = &NewBNMType::_met; return *(void **)&p; } \
        void *GetInvoker() override { return (void *)&BNM::NEW_CLASSES::GetNewMethodCalls<decltype(&NewBNMType::_met)>::invoke; } \
        const char *GetName() override { return OBFUSCATE_BNM(_name); } \
        BNM::RuntimeTypeGetter GetRetType() override { return _retType; } \
        std::vector<BNM::RuntimeTypeGetter> GetArgTypes() override { return {__VA_ARGS__}; } \
        bool IsStatic() override { return false; } \
    }; \
    public: \
    static inline _BNMMethod_##_met BNMMethod_##_met{}

#define BNM_NewStaticMethodInitCustomName(_retType, _met, _name, _argsCount, ...) \
    private: \
    struct _BNMStaticMethod_##_met : BNM::NEW_CLASSES::NewMethod { \
        _BNMStaticMethod_##_met() noexcept { \
            _BNMClass::AddNewMethod(this); \
        } \
        uint8_t GetArgsCount() override { return _argsCount; } \
        void *GetAddress() override { return (void *)&NewBNMType::_met; } \
        void *GetInvoker() override { return (void *)&BNM::NEW_CLASSES::GetNewStaticMethodCalls<decltype(&NewBNMType::_met)>::invoke; } \
        const char *GetName() override { return OBFUSCATE_BNM(_name); } \
        BNM::RuntimeTypeGetter GetRetType() override { return _retType; } \
        std::vector<BNM::RuntimeTypeGetter> GetArgTypes() override { return {__VA_ARGS__}; } \
        bool IsStatic() override { return true; } \
    }; \
    public: \
    static inline _BNMStaticMethod_##_met BNMStaticMethod_##_met{}

#define BNM_NewFieldInit(_name, _type) \
    private: \
    struct _BNMField_##_name : BNM::NEW_CLASSES::NewField { \
        _BNMField_##_name() noexcept { \
            _BNMClass::AddNewField(this); \
        } \
        const char *GetName() override { return OBFUSCATE_BNM(#_name); } \
        size_t GetOffset() override { return offsetof(NewBNMType, _name); } \
        size_t GetSize() override { return sizeof(decltype(_name)); } \
        unsigned int GetAttributes() override { return 0x0006; } \
        BNM::RuntimeTypeGetter GetType() override { return _type; } \
        BNM::BNM_PTR GetCppOffset() override { return 0; } \
    }; \
    public: \
    static inline _BNMField_##_name BNMField_##_name{}

#define BNM_NewOwnerInit(_getOwnerCode) \
    private: \
    struct _BNMOwner { \
        _BNMOwner() noexcept { \
            _BNMClass::SetOwnerGetter(GetOwner); \
        } \
        static BNM::RuntimeTypeGetter GetOwner() _getOwnerCode \
    }; \
    public: \
    static inline _BNMOwner BNMOwner = _BNMOwner()

// Добавьте класс в существующую или в новую библиотеку dll. Имя библиотеки dll без '.dll'!
#define BNM_NewClassWithDllInit(_dllName, _namespace, _name, _getBaseCode, ...)\
    private: \
    struct _BNMClass : BNM::NEW_CLASSES::NewClass { \
        _BNMClass() noexcept { \
            BNM::NEW_CLASSES::AddNewClass(this); \
        } \
        static inline std::vector<BNM::NEW_CLASSES::NewField *> myFields{}; \
        static inline std::vector<BNM::NEW_CLASSES::NewMethod *> myMethods{}; \
        static inline BNM::NEW_CLASSES::OwnerGetter myOwnerGetter{nullptr}; \
        size_t GetSize() override { return sizeof(_name); } \
        const char *GetName() override { return OBFUSCATE_BNM(#_name); } \
        const char *GetNamespace() override { return OBFUSCATE_BNM(_namespace); } \
        const char *GetDllName() override { return OBFUSCATE_BNM(_dllName); } \
        BNM::RuntimeTypeGetter GetBaseType() override _getBaseCode \
        std::vector<BNM::RuntimeTypeGetter> GetInterfaces() override { return {__VA_ARGS__}; } \
        std::vector<BNM::NEW_CLASSES::NewField *> GetFields() override { return myFields; } \
        std::vector<BNM::NEW_CLASSES::NewMethod *> GetMethods() override { return myMethods; } \
        BNM::NEW_CLASSES::OwnerGetter GetOwnerGetter() override { return myOwnerGetter; } \
        static void AddNewField(BNM::NEW_CLASSES::NewField *f) { myFields.push_back(f); } \
        static void AddNewMethod(BNM::NEW_CLASSES::NewMethod *f) { myMethods.push_back(f); } \
        static void SetOwnerGetter(BNM::NEW_CLASSES::OwnerGetter newOwnerGetter) { myOwnerGetter = newOwnerGetter; } \
    }; \
    public: \
    static inline _BNMClass BNMClass{}; \
    using NewBNMType = _name

#endif