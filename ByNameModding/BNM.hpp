#pragma once
#include <map>
#include <jni.h>
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
    struct RuntimeTypeGetter;
    template<typename T> constexpr RuntimeTypeGetter GetType(bool isArray = false) noexcept;
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

    // Check is pointer valid
    template<typename T>
    bool IsAllocated(T x) {
        static jmp_buf jump;
        static sighandler_t handler = [](int) { longjmp(jump, 1); };
        [[maybe_unused]] char c;
        bool ok = true;
        sighandler_t old_handler = signal(SIGSEGV, handler);
        if (!setjmp (jump)) c = *(char *) (x); else ok = false;
        signal(SIGSEGV, old_handler);
        return ok;
    };
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    T CheckObj(T obj) {
        if (obj && IsAllocated(obj)) return obj;
        return nullptr;
    }
    namespace UnityEngine {
        // Can be used for NewClass if Base type is UnityEngine.Object, MonoBehaviour, ScriptableObject
        // For System.Object use BNM::IL2CPP::Il2CppObject
        struct Object : public BNM::IL2CPP::Il2CppObject {
            BNM_INT_PTR m_CachedPtr = 0;
            bool Alive() { return BNM::CheckObj((void *)this) && (BNM_PTR)m_CachedPtr; }
            bool Same(void *object) { return Same((Object *)object); }
            bool Same(Object *object) { return (!Alive() && !object->Alive()) || (Alive() && object->Alive() && m_CachedPtr == object->m_CachedPtr); }
        };
    }

    // Only if obj child of UnityEngine.Object or object is UnityEngine.Object
    template<typename T>
    [[maybe_unused]] bool IsUnityObjectAlive(T o) {
        return ((UnityEngine::Object *)o)->Alive();
    };
    // Only if objects children of UnityEngine.Object or objects are UnityEngine.Object
    template<typename T1, typename T2>
    [[maybe_unused]] bool IsSameUnityObject(T1 o1, T2 o2) {
        auto obj1 = (UnityEngine::Object *)o1;
        auto obj2 = (UnityEngine::Object *)o2;
        return obj1->Same(obj2);
    };
    template<typename MET_T, typename PTR_T>
    void InitFunc(MET_T&& method, PTR_T ptr) {
        if (ptr != 0) *(void **)(&method) = (void *)(ptr);
    };

    // Basic c# classes (string, array, list)
    namespace MONO_STRUCTS {
        template<typename T> struct monoList;
        void *CompareExchange4List(void *syncRoot);
        template<typename T>
        void InitMonoListVTable(monoList<T> *list);
        struct monoString : BNM::IL2CPP::Il2CppObject {
            int length;
            IL2CPP::Il2CppChar chars[0];
            std::string get_string();
            const char *get_const_char();
            const char *c_str();
            std::string str();
#ifdef BNM_DEPRECATED
            [[maybe_unused]] std::string strO();
            std::string get_string_old();
#endif
            [[maybe_unused]] unsigned int getHash();
            static monoString *Create(const char *str);
            [[maybe_unused]] static monoString *Create(const std::string &str);
            [[maybe_unused]] static monoString *Empty();
        };
        template<typename T>
        struct monoArray : BNM::IL2CPP::Il2CppObject  {
            IL2CPP::Il2CppArrayBounds *bounds;
            IL2CPP::il2cpp_array_size_t capacity;
            T m_Items[0];
            [[maybe_unused]] IL2CPP::il2cpp_array_size_t getCapacity() { monoArray<T> *me = this; if (!me) return 0; return capacity; }
            T *getPointer() { if (!this) return nullptr; return m_Items; }
            std::vector<T> toCPPlist() {
                monoArray<T> *me = this;
                if (!me) return {};
                std::vector<T> ret;
                for (int i = 0; i < capacity; i++)
                    ret.push_back(m_Items[i]);
                return std::move(ret);
            }
            bool copyFrom(const std::vector<T> &vec) { if (!this || vec.empty()) return false; return copyFrom((T *)vec.data(), (IL2CPP::il2cpp_array_size_t)vec.size()); }
            [[maybe_unused]] bool copyFrom(T *arr, IL2CPP::il2cpp_array_size_t size) {
                if (!this || size > capacity) return false;
                memcpy(&m_Items[0], arr, size * sizeof(T));
                return true;
            }
            [[maybe_unused]] void copyTo(T *arr) { if (!this || !CheckObj(m_Items)) return; memcpy(arr, m_Items, sizeof(T) * capacity); }
            T operator[] (IL2CPP::il2cpp_array_size_t index) { if (getCapacity() < index) return {}; return m_Items[index]; }
            T at(IL2CPP::il2cpp_array_size_t index) { if (!this || getCapacity() <= index || empty()) return {}; return m_Items[index]; }
            bool empty() { if (!this) return false; return getCapacity() <= 0;}
            static monoArray<T> *Create(size_t capacity) {
                auto monoArr = (monoArray<T> *)malloc(sizeof(monoArray) + sizeof(T) * capacity);
                monoArr->klass = nullptr;
                monoArr->capacity = capacity;
                return monoArr;
            }
            [[maybe_unused]] static monoArray<T> *Create(const std::vector<T> &vec) { return Create((T *)vec.data(), vec.size()); }
            static monoArray<T> *Create(T *arr, size_t size) {
                monoArray<T> *monoArr = Create(size);
                monoArr->klass = nullptr;
                monoArr->copyFrom(arr, size);
                return monoArr;
            }
            // Only for arrays created via BNM!
            void Destroy() { if (!klass) free(this); }
        };
        template<typename T>
        struct monoList : BNM::IL2CPP::Il2CppObject {
            struct Enumerator {
                monoList<T> *list;
                int index;
                int version;
                T current;
                Enumerator(monoList<T> *list) : list(list), index(0), version(list->version) { current = {}; }

                // The original C# code is not needed: it is useless in C++
                // That's why there's just code to support C++ foreach
                T* begin() { return &list->items[0]; }
                T* end() { return &list->items->m_Items[list->size]; }
                T* begin() const { return &list->items[0]; }
                T* end() const { return &list->items->m_Items[list->size]; }
            };
            monoArray<T> *items;
            int size;
            int version;
            void *syncRoot;
            T *getItems() { return items->getPointer(); }
            [[maybe_unused]] int getSize() { return size; }
            [[maybe_unused]] int getVersion() { return version; }
            [[maybe_unused]] std::vector<T> toCPPlist() {
                monoList<T> *me = this;
                if (!me || size == 0) return {};
                std::vector<T> ret;
                for (int i = 0; i < size; i++)
                    ret.push_back(getItems()[i]);
                return std::move(ret);
            }

            [[maybe_unused]] void Add(T val) {
                GrowIfNeeded(1);
                items->m_Items[size] = val;
                size++;
                version++;
            }
            int IndexOf(T val) {
                for (int i = 0; i < size; i++)
                    if (items->m_Items[i] == val)
                        return i;
                return -1;
            }
            void RemoveAt(int index) {
                if (index != -1) {
                    Shift(index, -1);
                    version++;
                }
            }
            [[maybe_unused]] bool Remove(T val) {
                int i = IndexOf(val);
                if (i == -1) return false;
                RemoveAt(i);
                return true;
            }
            bool Resize(int newCapacity) {
                if (!this) return false;
                if (newCapacity <= items->capacity) return false;
                auto nItems = monoArray<T>::Create(newCapacity);
                nItems->klass = items->klass;
                nItems->monitor = items->monitor;
                nItems->bounds = items->bounds;
                nItems->capacity = newCapacity;
                if (items->capacity > 0) // Don't copy if array empty
                    memcpy(&nItems->m_Items[0], &items->m_Items[0], items->capacity * sizeof(T));
                items = nItems;
                return true;
            }
            T operator[] (int index) { return items->m_Items[index]; }
            bool copyFrom(const std::vector<T> &vec) { if (!this) return false; return copyFrom((T *)vec.data(), (int)vec.size()); }
            [[maybe_unused]] bool copyFrom(T *arr, int arrSize) {
                if (!this) return false;
                Resize(arrSize);
                memcpy(items->m_Items, arr, arrSize * sizeof(T));
                return true;
            }
            void Clear() {
                if (size > 0) memset(items->m_Items, 0, size * sizeof(T));
                ++version; size = 0;
            }
            bool Contains(T item) { // Not quite like in С# because of its features
                for (int i = 0; i < size; i++) if (items->m_Items[i] == item) return true;
                return false;
            }
            Enumerator GetEnumerator() { return this; }
            T get_Item(int index) {
                if (index >= size) return {};
                return items->at(index);
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
                if (delta < 0)
                    start -= delta;
                if (start < size)
                    memcpy(items->m_Items + start + delta, items->m_Items + start, size - start);
                size += delta;
                if (delta < 0)
                    memset(items->m_Items + size + delta, 0, -delta * sizeof(T));
            }
        };
        template<typename T>
        void InitMonoListVTable(monoList<T> *list) {
            /*
             *  Replace the virtual methods table because the original one is empty.
             *  It is also necessary for types that are not in the game (not the fact that the game would be able to use them without, but code is here)
             */
            struct StaticData {void *ptr{}; IL2CPP::MethodInfo *info{};};
            constexpr auto RemoveAt = &monoList<T>::RemoveAt;constexpr auto getSize = &monoList<T>::RemoveAt;constexpr auto Clear = &monoList<T>::Clear;
            constexpr auto get_Item = &monoList<T>::get_Item;constexpr auto set_Item = &monoList<T>::set_Item;constexpr auto IndexOf = &monoList<T>::IndexOf;
            constexpr auto Insert = &monoList<T>::Insert;constexpr auto get_false = &monoList<T>::get_false;constexpr auto Add = &monoList<T>::Add;
            constexpr auto Contains = &monoList<T>::Contains;constexpr auto CopyTo = &monoList<T>::CopyTo;constexpr auto Remove = &monoList<T>::Remove;
            constexpr auto GetEnumerator = &monoList<T>::GetEnumerator;constexpr auto get_SyncRoot = &monoList<T>::get_SyncRoot;
            static std::map<std::string, StaticData> namesMap = {
                    {OBFUSCATE_BNM("RemoveAt"), {*(void **)&RemoveAt, nullptr}}, {OBFUSCATE_BNM("get_Count"), {*(void **)&getSize, nullptr}},
                    {OBFUSCATE_BNM("Clear"), {*(void **)&Clear, nullptr}}, {OBFUSCATE_BNM("get_Item"), {*(void **)&get_Item, nullptr}},
                    {OBFUSCATE_BNM("set_Item"), {*(void **)&set_Item, nullptr}}, {OBFUSCATE_BNM("IndexOf"), {*(void **)&IndexOf, nullptr}},
                    {OBFUSCATE_BNM("Insert"), {*(void **)&Insert, nullptr}}, {OBFUSCATE_BNM("get_IsReadOnly"), {*(void **)&get_false, nullptr}},
                    {OBFUSCATE_BNM("get_IsFixedSize"), {*(void **)&get_false, nullptr}}, {OBFUSCATE_BNM("get_IsSynchronized"), {*(void **)&get_false, nullptr}},
                    {OBFUSCATE_BNM("Add"), {*(void **)&Add, nullptr}}, {OBFUSCATE_BNM("Contains"), {*(void **)&Contains, nullptr}},
                    {OBFUSCATE_BNM("CopyTo"), {*(void **)&CopyTo, nullptr}}, {OBFUSCATE_BNM("Remove"), {*(void **)&Remove, nullptr}},
                    {OBFUSCATE_BNM("GetEnumerator"), {*(void **)&GetEnumerator, nullptr}}, {OBFUSCATE_BNM("get_SyncRoot"), {*(void **)&get_SyncRoot, nullptr}},
            };
            auto cls = list->klass;
            auto size = sizeof(IL2CPP::Il2CppClass) + cls->vtable_count * sizeof(IL2CPP::VirtualInvokeData);
            auto clsCpy = (IL2CPP::Il2CppClass *)malloc(size);
            memcpy(clsCpy, cls, size);
            for (uint16_t i = 4 /* Пропуск virtual методов от System.Object */; i < clsCpy->vtable_count; ++i) {
                auto &cur = clsCpy->vtable[i];
                auto name = std::string(cur.method->name);
                auto dot = name.rfind('.');
                if (dot != std::string::npos) name = name.substr(dot + 1);
                auto e = namesMap.find(name);
                if (e != namesMap.end()) {
                    auto &statics = e->second;
                    if (!statics.info) {
                        statics.info = (IL2CPP::MethodInfo *)malloc(sizeof(IL2CPP::MethodInfo));
                        memcpy((void *)statics.info, (void *)cur.methodPtr, sizeof(IL2CPP::MethodInfo));
                        statics.info->methodPointer = (decltype(statics.info->methodPointer)) statics.ptr;
                    }
                    cur.method = statics.info;
                    cur.methodPtr = cur.method->methodPointer;
                }
            }
            list->klass = clsCpy;
        }
    }

    // Just structs define for LoadClass
    template<typename T = int>
    struct Field;
    template<typename T>
    struct Property;
    template<typename Ret>
    struct Method;
    struct RuntimeTypeGetter;

    struct LoadClass {
        IL2CPP::Il2CppClass *klass{};

        constexpr LoadClass() noexcept = default; // Just default

        LoadClass(const IL2CPP::Il2CppClass *clazz); // From class
        LoadClass(const IL2CPP::Il2CppObject *obj); // From object
        [[maybe_unused]] LoadClass(const IL2CPP::Il2CppType *type); // From type
        [[maybe_unused]] LoadClass(const MonoType *type); // From c# vm type
        [[maybe_unused]] LoadClass(RuntimeTypeGetter type); // From runtime type

        LoadClass(const std::string &namespaze, const std::string &name); // From class name
        LoadClass(const std::string &namespaze, const std::string &name, const std::string &dllName); // From class name and dll

        [[maybe_unused]] std::vector<LoadClass> GetInnerClasses(bool includeParent = true) const; // Get all inner classes
        [[maybe_unused]] std::vector<IL2CPP::FieldInfo *> GetFieldsInfo(bool includeParent = true) const; // Get all fields
        [[maybe_unused]] std::vector<IL2CPP::MethodInfo *> GetMethodsInfo(bool includeParent = true) const; // Get all methods

        Method<void> GetMethodByName(const std::string &name, int parameters = -1) const; // Get method by name and args count
        Method<void> GetMethodByName(const std::string &name, const std::vector<std::string> &parametersName) const; // Get method by name and args name
        Method<void> GetMethodByName(const std::string &name, const std::vector<RuntimeTypeGetter> &parametersType) const; // Get method by name and args type
        [[maybe_unused]] Property<bool> GetPropertyByName(const std::string &name, bool warning = false); // Get property by name
        [[maybe_unused]] LoadClass GetInnerClass(const std::string &name) const; // Get inner by name
        Field<int> GetFieldByName(const std::string &name) const; // Get field by name

        [[maybe_unused]] LoadClass GetParent() const; // Get parent class

        [[maybe_unused]] LoadClass GetArrayClass() const; // To array class

        IL2CPP::Il2CppType *GetIl2CppType() const; // To il2cpp type

        [[maybe_unused]] MonoType *GetMonoType() const; // To c# vm type

        IL2CPP::Il2CppClass *GetIl2CppClass() const; // Just get klass field

        BNM::RuntimeTypeGetter GetRuntimeType() const; // To BNM type

        // Fast operators
        inline operator IL2CPP::Il2CppType *() const { return GetIl2CppType(); };
        inline operator MonoType *() const { return GetMonoType(); };
        inline operator IL2CPP::Il2CppClass *() const { return GetIl2CppClass(); };
        operator BNM::RuntimeTypeGetter() const;

        void *CreateNewInstance() const; // like c# new Object(), but without calling .ctor

        [[maybe_unused]] std::string str() const; // Get namespace and name

        // like c# new Object[]
        template<typename T>
        MONO_STRUCTS::monoArray<T> *NewArray(IL2CPP::il2cpp_array_size_t length = 0) {
            if (!klass) return nullptr;
            TryInit();
            return (BNM::MONO_STRUCTS::monoArray<T> *) ArrayNew(klass, length);
        }

        // like c# new List<Object>()
        template<typename T>
        [[maybe_unused]] MONO_STRUCTS::monoList<T> *NewList() {
            if (!klass) return nullptr;
            TryInit();
            BNM::MONO_STRUCTS::monoArray<T> *array = NewArray<T>(1);
            BNM::MONO_STRUCTS::monoList<T> *lst = (BNM::MONO_STRUCTS::monoList<T> *)NewListInstance();
            if (!lst) {
                LOGEBNM("Failed to create a List for the class: %s", str().c_str());
                return nullptr;
            }
            lst->items = array;
            BNM::MONO_STRUCTS::InitMonoListVTable(lst);
            return lst;
        }

        // Box object
        template<typename T, typename = std::enable_if<!std::is_pointer<T>::value>>
        [[maybe_unused]] IL2CPP::Il2CppObject *BoxObject(T obj) {
            if (!klass) return nullptr;
            TryInit();
            return ObjBox(klass, (void *) obj);
        }

        // like c# new Object() with finding .ctor with args by count
        template<typename ...Args>
        [[maybe_unused]] void *CreateNewObject(Args ...args) { return CreateNewObjectCtor(sizeof...(Args), {}, args...); }

        // like c# new Object() with finding .ctor with args by count and names
        template<typename ...Args>
        void *CreateNewObjectCtor(int args_count, const std::vector<std::string> &arg_names, Args ...args);

        // Check is LoadClass valid
        inline operator bool() const { return klass != nullptr; };
    private: // Private) Just call il2cpp methods
        void TryInit() const;
        static IL2CPP::Il2CppObject *ObjBox(IL2CPP::Il2CppClass*, void *);
        static IL2CPP::Il2CppArray *ArrayNew(IL2CPP::Il2CppClass*, IL2CPP::il2cpp_array_size_t);
        static void *NewListInstance();
    };
    
    // For thread static fields
    namespace PRIVATE_FILED_UTILS {
        void GetStaticValue(IL2CPP::FieldInfo *info, void *value);
        void SetStaticValue(IL2CPP::FieldInfo *info, void *value);
    }

    template<typename T>
    struct Field {
        IL2CPP::FieldInfo *myInfo{};
        bool init = false, thread_static = false, isStatic = false;
        IL2CPP::Il2CppObject *instance{};

        Field() noexcept = default;

        // Copy other field, just for auto casts
        template<typename otherT>
        [[maybe_unused]] Field(Field<otherT> f) : Field(f.myInfo) {
            if (f.Initialized() && !f.isStatic && !f.thread_static && BNM::CheckObj(f.instance))
                setInstance(f.instance);
        }

        // From info
        Field(IL2CPP::FieldInfo *info) {
            init = BNM::CheckObj(info);
            if (init) {
                isStatic = CheckIsStatic(info);
                myInfo = info;
                thread_static = myInfo->offset == -1;
            }
        }

        // Get offset
        BNM_PTR GetOffset() {
            if (!init) return 0;
            return myInfo->offset;
        }

        // Get pointer to field
        T *getPointer() {
            if (!init) return makeSafeRet();
            if (!isStatic && !CheckObj(instance)) {
                LOGEBNM(OBFUSCATE_BNM("Can't get non static %s pointer without instance! Please call setInstance before getting or setting field."), str().c_str());
                return makeSafeRet();
            } else if (isStatic && !CheckObj(myInfo->parent)) {
                LOGEBNM(OBFUSCATE_BNM("Something went wrong, %s field has null parent class."), str().c_str());
                return makeSafeRet();
            } else if (thread_static) {
                LOGEBNM(OBFUSCATE_BNM("Thread static pointer don't supported, %s."), str().c_str());
                return makeSafeRet();
            }
            if (isStatic) return (T *) ((BNM_PTR) myInfo->parent->static_fields + myInfo->offset);
            return (T *) ((BNM_PTR) instance + myInfo->offset);
        }

        // Get value from field
        T get() {
            if (!init) return {};
            if (thread_static) {
                T val{};
                PRIVATE_FILED_UTILS::GetStaticValue(myInfo, (void *)&val);
                return val;
            }
            if (auto v = getPointer()) return *v;
            return {};
        }
        operator T() { return get(); }
        T operator()() { return get(); }

        // Set value to field
        void set(T val) {
            if (!init) return;
            if (thread_static) {
                PRIVATE_FILED_UTILS::SetStaticValue(myInfo, (void *)&val);
                return;
            }
            if (auto v = getPointer()) *v = val;
        }
        Field<T> &operator=(T val) {
            set(val);
            return *this;
        }

        // Set instance
        Field<T> &setInstance(IL2CPP::Il2CppObject *val, bool doWarn = true) {
            if (init && isStatic) {
                if (doWarn) LOGWBNM(OBFUSCATE_BNM("Trying set instance of static field %s. Please remove setInstance in code."), str().c_str());
                return *this;
            }
            init = val && myInfo != nullptr;
            instance = val;
            return *this;
        }

        // Just fast set instance
        Field<T> &operator[](void *val) { return setInstance((IL2CPP::Il2CppObject *)val); }
        Field<T> &operator[](IL2CPP::Il2CppObject *val) { return setInstance((IL2CPP::Il2CppObject *)val); }
        Field<T> &operator[](UnityEngine::Object *val) { return setInstance((IL2CPP::Il2CppObject *)val); }

        // Copy
        template<typename otherT>
        Field<T> &operator=(Field<otherT> f) {
            init = BNM::CheckObj(f.myInfo);
            if (init) {
                isStatic = CheckIsStatic(f.myInfo);
                myInfo = f.myInfo;
                thread_static = myInfo->offset == -1;
            }
            if (f.Initialized() && !f.isStatic && !f.thread_static && BNM::CheckObj(f.instance))
                setInstance(f.instance);
            return *this;
        }

        // Cast field to other type
        template<typename NewT>
        Field<NewT> cast() { return Field<NewT>(myInfo).setInstance(instance, false); }

        // Get field name
        std::string str() {
            if (init) return LoadClass(myInfo->parent).str() + OBFUSCATE_BNM(".(") + myInfo->name + OBFUSCATE_BNM(")");
            return OBFUSCATE_BNM("Uninitialized field");
        }

        // Just get init
        bool Initialized() noexcept { return init; }
    private:
        static bool CheckIsStatic(IL2CPP::FieldInfo *field) {
            if (!field || !field->type) return false;
            return (field->type->attrs & 0x0010) != 0 && field->offset != -1 && (field->type->attrs & 0x0040) == 0;
        }
        [[maybe_unused]] T *makeSafeRet() { T ret{}; return &ret; }
    };

    // Converts offset in memory to offset in lib (work for any lib)
    void *offsetInLib(void *offsetInMemory);

    template<typename Ret = void>
    struct Method {
        IL2CPP::MethodInfo *myInfo{};
        IL2CPP::Il2CppObject *instance{};
        bool init = false, isStatic = false, isVirtual = false;

        Method() noexcept = default;

        // Copy other method, just for auto casts
        template<typename T = void>
        Method(Method<T> m) : Method(m.myInfo) {
            if (m.Initialized() && !m.isStatic && BNM::CheckObj(m.instance))
                setInstance(m.instance);
        }

        // From info
        Method(const IL2CPP::MethodInfo *info) {
            init = BNM::CheckObj(info);
            if (init) {
                isStatic = info->flags & 0x0010;
                isVirtual = info->slot != IL2CPP::kInvalidIl2CppMethodSlot;
                myInfo = (decltype(myInfo)) info;
            }
        }

        // Set instance
        Method<Ret> &setInstance(IL2CPP::Il2CppObject *val, bool doWarn = true) {
            if (!init) return *this;
            if (init && isStatic) {
                if (doWarn)
                    LOGWBNM(OBFUSCATE_BNM("Trying set instance of static method %s. Please remove setInstance in code."), str().c_str());
                return *this;
            }
            instance = val;
            return *this;
        }

        // Just fast set instance
        inline Method<Ret> &operator[](void *val) { return setInstance((IL2CPP::Il2CppObject *)val); }
        inline Method<Ret> &operator[](IL2CPP::Il2CppObject *val) { return setInstance((IL2CPP::Il2CppObject *)val); }
        inline Method<Ret> &operator[](UnityEngine::Object *val) { return setInstance((IL2CPP::Il2CppObject *)val); }

        // Call method
        template<typename ...Args>
        Ret call(Args...args) {
            if (!init) return SafeReturn<Ret>();
            bool canInfo = true;
            if (sizeof...(Args) != myInfo->parameters_count){
                canInfo = false;
                LOGWBNM(OBFUSCATE_BNM("Trying to call %s with wrong parameters count... I hope you know what you're doing. Just I can't add MethodInfo to args(. Please try fix this."), str().c_str());
            }
            if (!isStatic && !CheckObj(instance)) {
                LOGEBNM(OBFUSCATE_BNM("Can't call non static %s without instance! Please call setInstance before calling method."), str().c_str());
                return SafeReturn<Ret>();
            }
            auto method = myInfo;
            if (isVirtual && !isStatic) method = (IL2CPP::MethodInfo *)instance->klass->vtable[myInfo->slot].method;
            if (!method || strcmp(method->name, myInfo->name)) method = myInfo;
            if (isStatic) {
                if (canInfo) {
#if UNITY_VER > 174
                    return ((Ret(*)(Args...,IL2CPP::MethodInfo *))method->methodPointer)(args..., method);
#else
                    return ((Ret(*)(void*,Args...,IL2CPP::MethodInfo *))method->methodPointer)(nullptr, args..., method);
#endif
                }
#if UNITY_VER > 174
                return (((Ret(*)(Args...))method->methodPointer)(args...));
#else
                return (((Ret(*)(void*,Args...))method->methodPointer)(nullptr, args...));
#endif
            }
            if (canInfo)
                return (((Ret(*)(IL2CPP::Il2CppObject *,Args...,IL2CPP::MethodInfo *))method->methodPointer)(instance, args..., method));
            return (((Ret(*)(IL2CPP::Il2CppObject *,Args...))method->methodPointer)(instance, args...));
        }

        // Fast call method
        template<typename ...Args>
        inline Ret operator ()(Args ...args) { return call(args...); }

        // To string
        std::string str() {
#if UNITY_VER > 174
#define kls klass
#else
#define kls declaring_type
#endif
            if (init) {
                return LoadClass(myInfo->return_type).str() + OBFUSCATE_BNM(" ") +
                       LoadClass(myInfo->kls).str() + OBFUSCATE_BNM(".[") +
                       myInfo->name + OBFUSCATE_BNM("]{Args count:") +
                       std::to_string(myInfo->parameters_count) + OBFUSCATE_BNM("}") +
                       (isStatic ? OBFUSCATE_BNM("(static)") : OBFUSCATE_BNM(""));
            }
            return OBFUSCATE_BNM("Uninitialized method");
#undef kls
        }

        // Just get myInfo
        IL2CPP::MethodInfo *GetInfo() {
            if (init) return myInfo;
            return {};
        }

        // Get offset
        BNM_PTR GetOffset() {
            if (init) return (BNM_PTR) myInfo->methodPointer;
            return {};
        }

        // Just get init
        bool Initialized() noexcept { return init; }
        operator bool() noexcept { return Initialized(); }

        // Just cast method
        template<typename NewRet>
        [[maybe_unused]] Method<NewRet> cast() {
            return Method<NewRet>(myInfo).setInstance(instance, false);
        }

        // Copy method
        template<typename other>
        Method<Ret> &operator=(Method<other> m) {
            init = BNM::CheckObj(m.myInfo);
            if (init) {
                isStatic = m.myInfo->flags & 0x0010;
                isVirtual = m.myInfo->slot != IL2CPP::kInvalidIl2CppMethodSlot;
                myInfo = (decltype(myInfo)) m.myInfo;
            }
            if (m.Initialized() && !m.isStatic && BNM::CheckObj(m.instance)) setInstance(m.instance);
            return *this;
        }
    private: // Crutch)
        template<typename T>static T SafeReturn(){return{};}
        template<>static void SafeReturn(){}
    };


    template<typename T = bool>
    struct Property {
        // This is easier than finding PropertyInfo
        Method<T> getter;
        Method<void> setter;

        Property() noexcept = default;

        // From getter and setter, you can use non property methods if you want
        template<typename V>
        Property(Method<V> getter, Method<void> setter) {
            this->getter = getter;
            this->setter = setter;
        }

        // Set instance
        Property<T> &setInstance(IL2CPP::Il2CppObject *val, bool doWarn = true) {
            getter.setInstance(val, doWarn);
            setter.setInstance(val, doWarn);
            return *this;
        }

        // Just fast set instance
        inline Property<T> &operator[](void *val) { return setInstance((IL2CPP::Il2CppObject *)val); }
        inline Property<T> &operator[](IL2CPP::Il2CppObject *val) { return setInstance((IL2CPP::Il2CppObject *)val); }
        inline Property<T> &operator[](UnityEngine::Object *val) { return setInstance((IL2CPP::Il2CppObject *)val); }

        // Call getter
        T get() { return getter(); }
        operator T() { return get(); }
        T operator()() { return get(); }

        // Call setter
        void set(T v) { return setter(v); }
        Property<T> &operator=(T val) {
            set(val);
            return *this;
        }

        // Copy
        template<typename V>
        Property<T> &operator=(Property<V> val) { // We don't need to set Instance here because it saved in methods
            getter = val.getter;
            setter = val.setter;
            return *this;
        }

        // Just get init
        [[maybe_unused]] bool Initialized() noexcept { return getter.init || setter.init; }

        // Just cast property
        template<typename NewRet>
        [[maybe_unused]] Property<NewRet> cast() {
            return Property<NewRet>(Method<NewRet>(getter), setter).setInstance(getter.instance ? getter.instance : setter.instance, false);
        }
    };

    // Code of CreateNewObjectCtor
    template<typename ...Args>
    void *LoadClass::CreateNewObjectCtor(int args_count, const std::vector<std::string> &arg_names, Args ...args) {
        if (!klass) return nullptr;
        TryInit();
        auto method = arg_names.empty() ? GetMethodByName(OBFUSCATES_BNM(".ctor"), args_count) : GetMethodByName(OBFUSCATES_BNM(".ctor"), arg_names);
        auto instance = CreateNewInstance();
        method[instance](args...);
        return instance;
    }

    // Basic c# Dictionary
    namespace MONO_STRUCTS {
        template<typename TKey, typename TValue>
        struct [[maybe_unused]] monoDictionary {
#ifdef BNM_DOTNET35
            struct Link { [[maybe_unused]] int HashCode, Next; };
            IL2CPP::Il2CppClass *klass;
            IL2CPP::MonitorData *monitor;
            monoArray<int> *table;
            monoArray<Link> *linkSlots;
            monoArray<TKey> *keys;
            monoArray<TValue> *values;
            int touchedSlots;
            [[maybe_unused]] int emptySlot;
            int count;
            [[maybe_unused]] int threshold;
            [[maybe_unused]] void *hcp;
            [[maybe_unused]] void *serialization_info;
            int version;
            std::map<TKey, TValue> toMap() {
                std::map<TKey, TValue> ret;
                for (int i = 0; i < touchedSlots; i++)
                    if (((*linkSlots)[i].HashCode & -2147483648) != 0)
                        ret.insert(std::make_pair((*keys)[i], (*values)[i]));
                return std::move(ret);
            }
            std::vector<TKey> getKeys() {
                std::vector<TKey> ret;
                for (int i = 0; i < touchedSlots; i++)
                    if (((*linkSlots)[i].HashCode & -2147483648) != 0)
                        ret.push_back((*keys)[i]);
                return std::move(ret);
            }
            std::vector<TValue> getValues() {
                std::vector<TValue> ret;
                for (int i = 0; i < touchedSlots; i++)
                    if (((*linkSlots)[i].HashCode & -2147483648) != 0)
                        ret.push_back((*values)[i]);
                return std::move(ret);
            }
#else
            struct Entry {
                [[maybe_unused]] int hashCode, next;
                TKey key;
                TValue value;
            };
            IL2CPP::Il2CppClass *klass;
            IL2CPP::MonitorData *monitor;
            [[maybe_unused]] monoArray<int> *buckets;
            monoArray<Entry> *entries;
            int count;
            int version;
            [[maybe_unused]] int freeList;
            [[maybe_unused]] int freeCount;
            [[maybe_unused]] void *comparer;
            monoArray<TKey> *keys;
            monoArray<TValue> *values;
            [[maybe_unused]] void *syncRoot;
            std::map<TKey, TValue> toMap() {
                std::map<TKey, TValue> ret;
                for (auto it = (Entry *)&entries->m_Items; it != ((Entry *)&entries->m_Items + count); ++it) ret.emplace(std::make_pair(it->key, it->value));
                return std::move(ret);
            }
            std::vector<TKey> getKeys() {
                std::vector<TKey> ret;
                for (int i = 0; i < count; ++i) ret.emplace_back(entries->at(i).key);
                return std::move(ret);
            }
            std::vector<TValue> getValues() {
                std::vector<TValue> ret;
                for (int i = 0; i < count; ++i) ret.emplace_back(entries->at(i).value);
                return std::move(ret);
            }
#endif
            [[maybe_unused]] int getSize() { return count; }
            [[maybe_unused]] int getVersion() { return version; }
            bool TryGet(TKey key, TValue *value) { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("TryGetValue"), 2).template cast<bool>().setInstance(this)(key, value); }
            [[maybe_unused]] void Add(TKey key, TValue value) { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("Add"), 2).setInstance(this)(key, value); }
            [[maybe_unused]] void Insert(TKey key, TValue value) { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("set_Item"), 2).setInstance(this)(key, value); }
            [[maybe_unused]] bool Remove(TKey key) { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("Remove"), 1).template cast<bool>().setInstance(this)( key); }
            [[maybe_unused]] bool ContainsKey(TKey key) { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("ContainsKey"), 1).template cast<bool>().setInstance(this)(key); }
            [[maybe_unused]] bool ContainsValue(TValue value) { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("ContainsValue"), 1).template cast<bool>().setInstance(this)(value); }
            TValue Get(TKey key) {
                TValue ret;
                if (TryGet(key, &ret)) return ret;
                return {};
            }
            TValue operator[](TKey key) { return Get(key); }
        };
    }

    // Struct to save data for finding it at runtime
    struct RuntimeTypeGetter {
        constexpr inline RuntimeTypeGetter() noexcept {}
        constexpr inline RuntimeTypeGetter(const char *namespaze, const char *name, bool isArray) noexcept : namespaze(namespaze), name(name), isArray(isArray) {}
        constexpr inline RuntimeTypeGetter(const char *namespaze, const char *name, bool isArray, LoadClass loadedClass) noexcept : namespaze(namespaze), name(name), isArray(isArray), loadedClass(loadedClass) {}
        const char *namespaze{}, *name{};
        bool isArray = false;
        LoadClass loadedClass{};
        LoadClass ToLC();
        IL2CPP::Il2CppType *ToIl2CppType();
        IL2CPP::Il2CppClass *ToIl2CppClass();
        operator IL2CPP::Il2CppType*();
        operator IL2CPP::Il2CppClass*();
        operator LoadClass();
    };

#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
    // Structures for addeding methods and fields to classes
    namespace MODIFY_CLASSES {

        // Data of new method
        struct AddableMethod {
            AddableMethod() noexcept;
            IL2CPP::MethodInfo *myInfo{};
            virtual uint8_t GetArgsCount() = 0;
            virtual void *GetAddress() = 0;
            virtual void *GetInvoker() = 0;
            virtual const char *GetName() = 0;
            virtual RuntimeTypeGetter GetRetType() = 0;
            virtual std::vector<RuntimeTypeGetter> GetArgTypes() = 0;
            virtual bool IsStatic() = 0;
        };

        // Data of new field
        struct AddableField {
            AddableField() noexcept;
            IL2CPP::FieldInfo *myInfo{};
            BNM_PTR offset{};
            virtual const char *GetName() = 0;
            virtual int GetSize() = 0;
            virtual RuntimeTypeGetter GetType() = 0;
        };

        struct TargetModifier {
            RuntimeTypeGetter (*newParentGetter)(){};
        };
        
        // Data of target class
        struct TargetClass {
            TargetClass() noexcept;
            virtual RuntimeTypeGetter GetTargetType() = 0;
            virtual std::vector<AddableField *> GetFields() = 0;
            virtual std::vector<AddableMethod *> GetMethods() = 0;
            virtual TargetModifier &GetTargetModifier() = 0;
        };

        // Добавить целевой класс в список
        void AddTargetClass(TargetClass *klass);
    }

    // Structures for creating new classes
    namespace NEW_CLASSES {

        // Data of new method
        struct NewMethod {
            NewMethod() noexcept;
            IL2CPP::MethodInfo *myInfo{};
            virtual uint8_t GetArgsCount() = 0;
            virtual void *GetAddress() = 0;
            virtual void *GetInvoker() = 0;
            virtual const char *GetName() = 0;
            virtual RuntimeTypeGetter GetRetType() = 0;
            virtual std::vector<RuntimeTypeGetter> GetArgTypes() = 0;
            virtual bool IsStatic() = 0;
        };

        // Data of new field
        struct NewField {
            NewField() noexcept;
            IL2CPP::FieldInfo *myInfo{};
            virtual const char *GetName() = 0;
            virtual int GetOffset() = 0;
            virtual int GetSize() = 0;
            virtual unsigned int GetAttributes() = 0;
            virtual RuntimeTypeGetter GetType() = 0;
            virtual BNM_PTR GetCppOffset() = 0;
        };

        // Data of new class
        struct NewClass {
            NewClass() noexcept;
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
        };

        // Add new class to list
        void AddNewClass(NEW_CLASSES::NewClass *klass);

        // Unpack arg
        template<typename Q>
        static inline Q UnpackArg(void *arg) {
            if constexpr (std::is_pointer<Q>::value) return (Q)arg;
            else return *(Q *)arg;
        }

        template<typename T>
        static inline void *PackArg(T arg) {
            if constexpr (std::is_pointer<T>::value) return arg;
            static auto cls = BNM::GetType<T>();
            return cls.ToLC().BoxObject(&arg);
        }

        template<typename> struct GetNewMethodCalls {};
        template<typename> struct GetNewStaticMethodCalls {};

        // Class for creating invoker for methods
        template<typename RetT, typename T, typename ...ArgsT>
        struct GetNewMethodCalls<RetT(T:: *)(ArgsT...)> {
            template<std::size_t ...As>
            static void *InvokeMethod(RetT(*func)(T*, ArgsT...), T *instance, void **args, std::index_sequence<As...>) {
                if constexpr (std::is_same<RetT, void>::value) {
                    func(instance, UnpackArg<ArgsT>(args[As])...);
                    return nullptr;
                } else return PackArg(func(instance, UnpackArg<ArgsT>(args[As])...));
            }
            static void *invoke(IL2CPP::Il2CppMethodPointer ptr, [[maybe_unused]] IL2CPP::MethodInfo *m, void *obj, void **args) {
#if UNITY_VER < 211
#if UNITY_VER > 174
#define kls klass
#else
#define kls declaring_type
#endif
                if (m->kls->valuetype) obj = static_cast<IL2CPP::Il2CppObject *>(obj) + 1;
#undef kls
#endif
                auto func = (RetT(*)(T*, ArgsT...))(ptr);
                auto instance = (T *)(obj);
                auto seq = std::make_index_sequence<sizeof...(ArgsT)>();
                return InvokeMethod(func, instance, args, seq);
            }
        };

#if UNITY_VER > 174
        // Class for creating invoker for static methods
        template<typename RetT, typename ...ArgsT>
        struct GetNewStaticMethodCalls<RetT(*)(ArgsT...)> {
            template<std::size_t ...As>
            static void *InvokeMethod(RetT(*func)(ArgsT...), void **args, std::index_sequence<As...>) {
                if constexpr (std::is_same<RetT, void>::value) {
                    func(UnpackArg<ArgsT>(args[As])...);
                    return nullptr;
                } else
                    return func(UnpackArg<ArgsT>(args[As])...);
            }
            static void *invoke(IL2CPP::Il2CppMethodPointer ptr, [[maybe_unused]] IL2CPP::MethodInfo *m, [[maybe_unused]] void *obj, void **args) {
#if UNITY_VER < 211
                if (m->klass->valuetype) obj = static_cast<IL2CPP::Il2CppObject *>(obj) + 1;
#endif
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
                } else
                    return func(nullptr, UnpackArg<ArgsT>(args[As])...);
            }
            static void *invoke(IL2CPP::Il2CppMethodPointer ptr, IL2CPP::MethodInfo *m, [[maybe_unused]] void *obj, void **args) {
                if (m->declaring_type->valuetype) obj = static_cast<IL2CPP::Il2CppObject *>(obj) + 1;
                auto func = (RetT(*)(void *, ArgsT...))(ptr);
                auto seq = std::make_index_sequence<sizeof...(ArgsT)>();
                return InvokeMethod(func, args, seq);
            }
        };
#endif
    }
#endif

    // Get game mono type name at compile time
    [[maybe_unused]] constexpr RuntimeTypeGetter GetType(const char *namespaze, const char *name, bool isArray = false) noexcept { return RuntimeTypeGetter{namespaze, name, isArray}; }

    // Method for creating basic c# strings
    MONO_STRUCTS::monoString *CreateMonoString(const char *str);
    [[maybe_unused]] MONO_STRUCTS::monoString *CreateMonoString(const std::string &str);

    // Get extern methods (icall)
    void *GetExternMethod(const std::string &str);

    // True when il2cpp and BNM loaded
    bool Il2cppLoaded();

    // Il2cpp thread utils
    [[maybe_unused]] bool AttachIl2Cpp(); // Return true if need Detach
    [[maybe_unused]] IL2CPP::Il2CppThread *CurrentIl2CppThread();
    [[maybe_unused]] void DetachIl2Cpp();

    // Get path to libil2cpp.so
    [[maybe_unused]] std::string GetLibIl2CppPath();

    // Get offset of loaded libil2cpp.so
    [[maybe_unused]] BNM_PTR GetLibIl2CppOffset();

    // Don't close it! BNM will just crash without it.
    [[maybe_unused]] void *GetLibIl2CppDlInst();

    // Get Il2Cpp mono type name at compile time
    template<typename T>
    constexpr RuntimeTypeGetter GetType(bool isArray) noexcept {
        if (std::is_same<T, void>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Void"), isArray};
        else if (std::is_same<T, bool>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Boolean"), isArray};
        else if (std::is_same<T, uint8_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Byte"), isArray};
        else if (std::is_same<T, int8_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("SByte"), isArray};
        else if (std::is_same<T, int16_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Int16"), isArray};
        else if (std::is_same<T, uint16_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("UInt16"), isArray};
        else if (std::is_same<T, int32_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Int32"), isArray};
        else if (std::is_same<T, uint32_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("UInt32"), isArray};
        else if (std::is_same<T, intptr_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("IntPtr"), isArray};
        else if (std::is_same<T, int64_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Int64"), isArray};
        else if (std::is_same<T, uint64_t>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("UInt64"), isArray};
        else if (std::is_same<T, float>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Single"), isArray};
        else if (std::is_same<T, double>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Double"), isArray};
        else if (std::is_same<T, BNM::IL2CPP::Il2CppString *>::value || std::is_same<T, BNM::MONO_STRUCTS::monoString *>::value)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("String"), isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::Vector3>::value)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Vector3"), isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::Vector2>::value)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Vector2"), isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::Color>::value)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Color"), isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::Ray>::value)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Ray"), isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::RaycastHit>::value)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("RaycastHit"), isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::Quaternion>::value)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Quaternion"), isArray};
        else if (std::is_same<T, BNM::UnityEngine::Object *>::value)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Object"), isArray};
        return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Object"), isArray};
    }

    // Unbox object just copy of il2cpp method
    template<typename T>
    [[maybe_unused]] static T UnboxObject(T obj) { return (T)(void *)(((char *)obj) + sizeof(BNM::IL2CPP::Il2CppObject)); }

    // Hook method by changing MethodInfo
    bool InvokeHookImpl(IL2CPP::MethodInfo *m, void *newMet, void **oldMet);

    template<typename T_NEW, typename T_OLD>
    bool InvokeHook(BNM::Method<int> m, T_NEW newMet, T_OLD &&oldMet) {
        if (m) return InvokeHookImpl(m.myInfo, (void *)newMet, (void **)&oldMet);
        return false;
    }
    template<typename T_NEW, typename T_OLD>
    bool InvokeHook(IL2CPP::MethodInfo *m, T_NEW newMet, T_OLD &&oldMet) { return InvokeHookImpl(m, (void *)newMet, (void **)&oldMet); }

    // Method HOOK by changing the table of virtual methods of a class
    bool VirtualHookImpl(BNM::LoadClass targetClass, IL2CPP::MethodInfo *m, void *newMet, void **oldMet);

    template<typename T_NEW, typename T_OLD>
    bool VirtualHook(BNM::LoadClass targetClass, BNM::Method<int> m, T_NEW newMet, T_OLD &&oldMet) {
        if (targetClass && m) return VirtualHookImpl(targetClass, m.myInfo, (void *)newMet, (void **)&oldMet);
        return false;
    }
    template<typename T_NEW, typename T_OLD>
    bool VirtualHook(BNM::LoadClass targetClass, IL2CPP::MethodInfo *m, T_NEW newMet, T_OLD &&oldMet) {
        return VirtualHookImpl(targetClass, m, (void *)newMet, (void **)&oldMet);
    }


    // Method for checking class of object
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, IL2CPP::Il2CppClass *klass) { return IsA<BNM::IL2CPP::Il2CppObject *>((IL2CPP::Il2CppObject *)object, klass); }
    template<> bool IsA<IL2CPP::Il2CppObject *>(IL2CPP::Il2CppObject *object, IL2CPP::Il2CppClass *klass);
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, LoadClass klass) { return IsA(object, klass.klass); }
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, IL2CPP::Il2CppObject *klass) { if (!klass) return false; return IsA(object, klass->klass); }
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, MonoType *type) { return IsA(object, LoadClass(type)); }

    template<typename NEW_T, typename OLD_T>
    void HOOK(BNM::Method<void> m, NEW_T newMethod, OLD_T&& oldBytes) {
        if (m.Initialized())
            DobbyHook((void *)m.GetOffset(), (void *) newMethod, (void **) &oldBytes);
    };
    // Try bypass any protection by getting full lib path (INTERNAL ONLY)
    [[maybe_unused]] void HardBypass(JNIEnv *env, jobject context = nullptr);
    namespace External {
        [[maybe_unused]] bool TryInitDl(void *dl, const char *path = nullptr, bool external = true);
        // Try load BNM if you externally load BNM
        // Need call this from any unity thread
        // dl - dlopened libil2cpp.so
        [[maybe_unused]] void LoadBNM(void *dl);
        // Set dlopened libil2cpp.so without checking it and try load BNM
        // GetLibIl2CppOffset and GetLibIl2CppPath will be empty
        // Need call this from any unity thread
        // dl - dl opened libil2cpp.so
        [[maybe_unused]] void ForceLoadBNM(void *dl);
    }
    void AddOnLoadedEvent(void (*event)());
    void ClearOnLoadedEvents();
}
#define InitResolveFunc(x, y) BNM::InitFunc(x, BNM::GetExternMethod(y))

#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES

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
    }; \
    public: \
    static inline _BNMModClass BNMModClass = _BNMModClass(); \
    using ModBNMType = _cppCls

#define BNM_ModAddMethod(_retType, _name, _argsCount, ...) \
    private: \
    struct _BNMModMethod_##_name : BNM::MODIFY_CLASSES::AddableMethod { \
        _BNMModMethod_##_name() noexcept { \
            BNMModClass.AddMethod(this); \
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
    static inline _BNMModMethod_##_name BNMModMethod_##_name = _BNMModMethod_##_name()

#define BNM_ModAddStaticMethod(_retType, _name, _argsCount, ...) \
    private: \
    struct _BNMModMethod_##_name : BNM::MODIFY_CLASSES::AddableMethod { \
        _BNMModMethod_##_name() noexcept { \
            BNMModClass.AddMethod(this); \
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
    static inline _BNMModMethod_##_name BNMModMethod_##_name = _BNMModMethod_##_name()

#define BNM_ModAddField(_name, _type) \
    private: \
    struct _BNMModField_##_name : BNM::MODIFY_CLASSES::AddableField { \
        _BNMModField_##_name() noexcept { \
            BNMModClass.AddField(this); \
        } \
        const char *GetName() override { return OBFUSCATE_BNM(#_name); } \
        int GetSize() override { return sizeof(decltype(_name)); } \
        BNM::RuntimeTypeGetter GetType() override { return _type; } \
    }; \
    public: \
    static inline _BNMModField_##_name BNMModField_##_name = _BNMModField_##_name()

#define BNM_ModNewParent(_getParentCode) \
    private: \
    struct _BNMModNewParent { \
        _BNMModNewParent() noexcept { \
            BNMModClass.SetParrentGetter(GetNewParent); \
        } \
        static BNM::RuntimeTypeGetter GetNewParent() _getParentCode \
    }; \
    public: \
    static inline _BNMModNewParent BNMModNewParent = _BNMModNewParent()

#define BNM_NewClassInit(_namespace, _name, _getBaseCode, ...) BNM_NewClassWithDllInit("Assembly-CSharp", _namespace, _name, _getBaseCode, __VA_ARGS__)
#define BNM_NewMethodInit(_retType, _name, _argsCount, ...) BNM_NewMethodInitCustomName(_retType, _name, #_name, _argsCount, __VA_ARGS__)
#define BNM_NewStaticMethodInit(_retType, _name, _argsCount, ...) BNM_NewStaticMethodInitCustomName(_retType, _name, #_name, _argsCount, __VA_ARGS__)
#define BNM_NewConstructorInit(_name, _argsCount, ...) BNM_NewMethodInitCustomName(BNM::GetType<void>(), _name, ".ctor", _argsCount, __VA_ARGS__)

#define BNM_NewMethodInitCustomName(_retType, _met, _name, _argsCount, ...) \
    private: \
    struct _BNMMethod_##_met : BNM::NEW_CLASSES::NewMethod { \
        _BNMMethod_##_met() noexcept { \
            BNMClass.AddNewMethod(this); \
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
    static inline _BNMMethod_##_met BNMMethod_##_met = _BNMMethod_##_met()

#define BNM_NewStaticMethodInitCustomName(_retType, _met, _name, _argsCount, ...) \
    private: \
    struct _BNMStaticMethod_##_met : BNM::NEW_CLASSES::NewMethod { \
        _BNMStaticMethod_##_met() noexcept { \
            BNMClass.AddNewMethod(this); \
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
    static inline _BNMStaticMethod_##_met BNMStaticMethod_##_met = _BNMStaticMethod_##_met()

#define BNM_NewFieldInit(_name, _type) \
    private: \
    struct _BNMField_##_name : BNM::NEW_CLASSES::NewField { \
        _BNMField_##_name() noexcept { \
            BNMClass.AddNewField(this); \
        } \
        const char *GetName() override { return OBFUSCATE_BNM(#_name); } \
        int GetOffset() override { return offsetof(NewBNMType, _name); } \
        int GetSize() override { return sizeof(decltype(_name)); } \
        unsigned int GetAttributes() override { return 0x0006; } \
        BNM::RuntimeTypeGetter GetType() override { return _type; } \
        BNM::BNM_PTR GetCppOffset() override { return 0; } \
    }; \
    public: \
    static inline _BNMField_##_name BNMField_##_name = _BNMField_##_name()

// Add class to exist or to new dll. Write dll name without '.dll'!
#define BNM_NewClassWithDllInit(_dllName, _namespace, _name, _getBaseCode, ...)\
    private: \
    struct _BNMClass : BNM::NEW_CLASSES::NewClass { \
        _BNMClass() noexcept { \
            BNM::NEW_CLASSES::AddNewClass(this); \
        } \
        static inline std::vector<BNM::NEW_CLASSES::NewField *> myFields{}; \
        static inline std::vector<BNM::NEW_CLASSES::NewMethod *> myMethods{}; \
        size_t GetSize() override { return sizeof(_name); } \
        const char *GetName() override { return OBFUSCATE_BNM(#_name); } \
        const char *GetNamespace() override { return OBFUSCATE_BNM(_namespace); } \
        const char *GetDllName() override { return OBFUSCATE_BNM(_dllName); } \
        BNM::RuntimeTypeGetter GetBaseType() override _getBaseCode \
        std::vector<BNM::RuntimeTypeGetter> GetInterfaces() override { return {__VA_ARGS__}; } \
        std::vector<BNM::NEW_CLASSES::NewField *> GetFields() override { return myFields; } \
        std::vector<BNM::NEW_CLASSES::NewMethod *> GetMethods() override { return myMethods; } \
        static void AddNewField(BNM::NEW_CLASSES::NewField *f) { myFields.push_back(f); } \
        static void AddNewMethod(BNM::NEW_CLASSES::NewMethod *f) { myMethods.push_back(f); } \
    }; \
    public: \
    static inline _BNMClass BNMClass = _BNMClass(); \
    using NewBNMType = _name

#endif