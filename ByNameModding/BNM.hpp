#pragma once
#include <algorithm>
#include <android/log.h>
#include <codecvt>
#include <cstring>
#include <dlfcn.h>
#include <fcntl.h>
#include <map>
#include <random>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>
#include "BNM_settings.hpp"
using namespace std;
typedef unsigned long DWORD;
namespace BNM {
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
#elif UNITY_VER > 201 && UNITY_VER < 204
    #include "BNM_data/Il2CppHeaders/2020.3.h"
#elif UNITY_VER == 211
    #include "BNM_data/Il2CppHeaders/2021.1.h"
#elif UNITY_VER == 212
    #include "BNM_data/Il2CppHeaders/2021.2.h"
#elif UNITY_VER == 213
    #include "BNM_data/Il2CppHeaders/2021.3.h"
#else
    #include "BNM_data/Il2CppHeaders/2020.3.h"
#endif
}
typedef IL2CPP::Il2CppReflectionType MonoType;
typedef std::vector<IL2CPP::Il2CppAssembly*> AssemblyVector;
typedef std::vector<IL2CPP::Il2CppClass*> TypeVector;

/********** STRUCTS AND CLASSES **************/
namespace UNITY_STRUCTS {
#include "BNM_data/BasicStructs.h"
    }

    /********** BNM MACROS **************/
    auto isAllocated = [](auto x) -> bool {
        int fd = open(OBFUSCATE_BNM("/dev/random"), O_WRONLY);
        bool valid = true;
        if (write(fd, (void *) x, sizeof(x)) < 0)
            valid = false;
        close(fd);
        return valid;
    };
    template<typename T>
    T CheckObj(T obj) {
        static_assert(std::is_pointer<T>::value, OBFUSCATE_BNM("Expected a pointer in CheckObj"));
        if (obj && isAllocated(obj))
            return obj;
        return nullptr;
    }
    // Only if obj child of UnityEngine.Object or object is UnityEngine.Object
    [[maybe_unused]] auto IsUnityObjectAlive = [](auto o) {
        return o != 0 && *(intptr_t *)((uint64_t)o + 0x8) != 0;
    };
    // Only if objects children of UnityEngine.Object or objects are UnityEngine.Object
    [[maybe_unused]] auto IsSameUnityObject = [](auto o1, auto o2) {
        return (!IsUnityObjectAlive(o1) && !IsUnityObjectAlive(o2)) || (IsUnityObjectAlive(o1) && IsUnityObjectAlive(o2) && *(intptr_t *)((uint64_t)o1 + 0x8) == *(intptr_t *)((uint64_t)o2 + 0x8));
    };
    namespace MONO_STRUCTS {
        struct monoString {
            IL2CPP::Il2CppClass *klass;
            IL2CPP::MonitorData *monitor;
            int length;
            IL2CPP::Il2CppChar chars[0];
            std::string get_string();
            const char *get_const_char();
            const char *c_str();
            std::string str();
            [[maybe_unused]] std::string strO();
            operator std::string();
            operator const char *();
            std::string get_string_old();
            [[maybe_unused]] unsigned int getHash();
            static monoString *Create(const char *str);
            static monoString *Create(const std::string& str);
            [[maybe_unused]] static monoString* Empty();
        };
        template<typename T>
        struct monoArray {
            IL2CPP::Il2CppClass *klass;
            IL2CPP::MonitorData *monitor;
            IL2CPP::Il2CppArrayBounds *bounds;
            int32_t capacity;
            T m_Items[0];
            [[maybe_unused]] int getCapacity() { if (!this) return 0; return capacity; }
            T *getPointer() { if (!this) return nullptr; return m_Items; }
            std::vector<T> toCPPlist() {
                if (!this) return {};
                std::vector<T> ret;
                for (int i = 0; i < capacity; i++)
                    ret.push_back(m_Items[i]);
                return std::move(ret);
            }
            bool copyFrom(const std::vector<T> &vec) { if (!this) return false; return copyFrom((T*)vec.data(), (int)vec.size()); }
            [[maybe_unused]] bool copyFrom(T *arr, int size) {
                if (!this) return false;
                if (size < capacity)
                    return false;
                memcpy(m_Items, arr, size * sizeof(T));
                return true;
            }
            [[maybe_unused]] void copyTo(T *arr) { if (!this || !CheckObj(m_Items)) return; memcpy(arr, m_Items, sizeof(T) * capacity); }
            T operator[] (int index) { if (getCapacity() < index) return {}; return m_Items[index]; }
            T at(int index) { if (!this || getCapacity() <= index || empty()) return {}; return m_Items[index]; }
            bool empty() { if (!this) return false; return getCapacity() <= 0;}
            static monoArray<T> *Create(int capacity) {
                auto monoArr = (monoArray<T> *)malloc(sizeof(monoArray) + sizeof(T) * capacity);
                monoArr->capacity = capacity;
                return monoArr;
            }
            static monoArray<T> *Create(const std::vector<T> &vec) { return Create(vec.data(), vec.size()); }
            static monoArray<T> *Create(T *arr, int size) {
                monoArray<T> *monoArr = Create(size);
                monoArr->copyFrom(arr, size);
                return monoArr;
            }
        };
        template<typename T>
        struct monoList {
            IL2CPP::Il2CppClass *klass;
            IL2CPP::MonitorData *monitor;
            monoArray<T> *items;
            int size;
            int version;
            T* getItems() { return items->getPointer(); }
            int getSize() { return size; }
            [[maybe_unused]] int getVersion() { return version; }
            [[maybe_unused]] std::vector<T> toCPPlist() {
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
            [[maybe_unused]] void Remove(T val) { RemoveAt(IndexOf(val)); }
            bool Resize(int newCapacity) {
                if (!this) return false;
                if (newCapacity <= items->capacity) return false;
                auto nItems = monoArray<T>::Create(newCapacity);
                nItems->klass = items->klass;
                nItems->monitor = items->monitor;
                nItems->bounds = items->bounds;
                nItems->capacity = newCapacity;
                if (items->capacity > 0) // Don't copy if array empty
                    memcpy(nItems->m_Items, items->m_Items, items->capacity * sizeof(T));
                items = nItems;
                return true;
            }
            T operator[] (int index) { return items->m_Items[index]; }
        private:
            void GrowIfNeeded(int n) {
                if (size + n > items->capacity)
                    Resize(size + n);
            }
            void Shift(int start, int delta) {
                if (delta < 0)
                    start -= delta;
                if (start < size)
                    memcpy(items + start + delta, items + start, size - start);
                size += delta;
                if (delta < 0)
                    memset(items + size + delta, 0, -delta * sizeof(T));
            }
        };
    }


    template<typename T>
    struct Field {
        static bool CheckIfStatic(IL2CPP::FieldInfo *fieldInfo) {
            if (!fieldInfo || !fieldInfo->type)
                return false;
            if (fieldInfo->type->attrs & 0x0010u)
                return true;
            if (fieldInfo->offset == -1)
                LOGIBNM(OBFUSCATE_BNM("Thread static fields is not supported!"));
            return false;
        }
        IL2CPP::FieldInfo *myInfo;
        bool init;
        bool thread_static;
        void *instance;
        bool isStatic;
        Field() = default;
        Field(IL2CPP::FieldInfo *info, void *_instance = nullptr) {
            if (init = (info != nullptr)) {
                isStatic = CheckIfStatic(info);
                if (!isStatic) {
                    init = _instance != nullptr;
                    instance = _instance;
                }
                myInfo = info;
                thread_static = myInfo->offset == -1;
            }
        }
        DWORD GetOffset() {
            return myInfo->offset;
        }
        T* getPointer() {
            if (!init || thread_static || (isStatic && (!myInfo->parent || !myInfo->parent->static_fields)))
                return 0;
            if (isStatic)
                return (T *) ((uint64_t) myInfo->parent->static_fields + myInfo->offset);
            return (T *) ((uint64_t) instance + myInfo->offset);
        }
        T get() {
            if (!init || thread_static || (isStatic && (!myInfo->parent || !myInfo->parent->static_fields)))
                return {};
            return *getPointer();
        }
        void set(T val) {
            if (!init || thread_static || (isStatic && (!myInfo->parent || !myInfo->parent->static_fields)))
                return;
            *getPointer() = val;
        }
        operator T() {
            return get();
        }
        Field<T>& operator=(T val) {
            set(val);
            return *this;
        }
        Field<T>& setInstance(void *val) {
            init = val && myInfo != nullptr;
            instance = val;
            return *this;
        }
        Field<T>& setInstanceDanger(void *val) {
            init = true;
            instance = val;
            return *this;
        }
        T operator()() {
            return get();
        }
        Field<T>& operator()(void *val) {
            return setInstance(val);
        }
    };
    auto InitFunc = [](auto&& method, auto ptr) {
        if (ptr != 0)
            *(void **)(&method) = (void *)(ptr);
    };
    template<typename Ret, typename ... Args>
    Ret BetterCall(IL2CPP::MethodInfo *method, void *instance, Args... args);
    struct LoadClass {
        IL2CPP::Il2CppClass *klass = nullptr;
        LoadClass();
        LoadClass(IL2CPP::Il2CppClass *clazz);
        LoadClass(IL2CPP::Il2CppObject *obj);
        [[maybe_unused]] LoadClass(IL2CPP::Il2CppType *type);
        [[maybe_unused]] LoadClass(MonoType *type);
        LoadClass(const std::string& _namespace, const std::string& clazz);
        LoadClass(const std::string& _namespace, const std::string& _name, const std::string& dllName);
        IL2CPP::FieldInfo *GetFieldInfoByName(const std::string& name) const;
        DWORD GetFieldOffset(const std::string& name) const;
        IL2CPP::MethodInfo *GetMethodInfoByName(const std::string& name, int parameters) const;
        [[maybe_unused]] DWORD GetMethodOffsetByName(const std::string& name, int parameters = -1) const;
        IL2CPP::MethodInfo *GetMethodInfoByName(const std::string& name, const std::vector<std::string>& params_names) const;
        [[maybe_unused]] DWORD GetMethodOffsetByName(const std::string& name, const std::vector<std::string>& params_names) const;
        IL2CPP::MethodInfo *GetMethodInfoByName(const std::string& name, const std::vector<std::string>& params_names, const std::vector<IL2CPP::Il2CppType *>& params_types) const;
        [[maybe_unused]] DWORD GetMethodOffsetByName(const std::string& name, const std::vector<std::string>& params_names, const std::vector<IL2CPP::Il2CppType *>& params_types) const;
        IL2CPP::MethodInfo *GetMethodInfoByName(const std::string& name, const std::vector<IL2CPP::Il2CppType *>& params_types) const;
        [[maybe_unused]] DWORD GetMethodOffsetByName(const std::string& name, const std::vector<IL2CPP::Il2CppType *>& params_types) const;
        IL2CPP::Il2CppType *GetIl2CppType() const;
        [[maybe_unused]] MonoType *GetMonoType() const;
        operator IL2CPP::Il2CppType *() const { return GetIl2CppType(); };
        operator MonoType *() const { return GetMonoType(); };
        void *CreateNewInstance() const;
        IL2CPP::Il2CppClass *GetIl2CppClass() const;
        [[maybe_unused]] std::string GetClassName() const;
        template<typename T>
        MONO_STRUCTS::monoArray<T> *NewArray(IL2CPP::il2cpp_array_size_t length = 0) {
            if (!klass) return nullptr;
            TryInit();
            return (BNM::MONO_STRUCTS::monoArray<T> *) ArrayNew(klass, length);
        }
        template<typename T>
        [[maybe_unused]] MONO_STRUCTS::monoList<T> *NewList() {
            if (!klass) return nullptr;
            TryInit();
            BNM::MONO_STRUCTS::monoArray<T> *array = NewArray<T>();
            IL2CPP::Il2CppClass *ArrClass = array->klass;
            BNM::MONO_STRUCTS::monoList<T> *lst = ObjNew(ArrClass);
            lst->Items = array;
            return lst;
        }
        template<typename T>
        [[maybe_unused]] IL2CPP::Il2CppObject* BoxObject(T obj) {
            if (!klass) return nullptr;
            TryInit();
            return ObjBox(klass, (void *) obj);
        }
        template<typename T>
        Field<T> GetFieldByName(const std::string& name, void *instance = nullptr) {
            auto fieldInfo = GetFieldInfoByName(name);
            if (!fieldInfo) return Field<T>();
            return Field<T>(fieldInfo, instance);
        }
        template<typename ...Args>
        [[maybe_unused]] void *CreateNewObject(Args ... args) { return CreateNewObjectCtor(sizeof...(Args), {}, args...); }
        template<typename ...Args>
        void *CreateNewObjectCtor(int args_count, const std::vector<std::string>& arg_names, Args ... args) {
            if (!klass) return nullptr;
            TryInit();
            IL2CPP::MethodInfo *method = arg_names.empty() ? GetMethodInfoByName(OBFUSCATES_BNM(".ctor"), args_count) : GetMethodInfoByName(OBFUSCATES_BNM(".ctor"), arg_names);
            IL2CPP::Il2CppObject *instance = (IL2CPP::Il2CppObject *) CreateNewInstance();
            void (*ctor)(...);
            InitFunc(ctor, method->methodPointer);
            ctor(instance, args...);
            return (void *) instance;
        }
        static LoadClass WithMethodName(const std::string& _namespace, const std::string& _name, const std::string& methodName);
    private:
        void TryInit() const;
        static IL2CPP::Il2CppObject* ObjBox(IL2CPP::Il2CppClass*, void *);
        static IL2CPP::Il2CppObject* ObjNew(IL2CPP::Il2CppClass*);
        static IL2CPP::Il2CppArray* ArrayNew(IL2CPP::Il2CppClass*, IL2CPP::il2cpp_array_size_t);
    };
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
            void *compare;
            monoArray<TKey> *keys;
            monoArray<TValue> *values;
            [[maybe_unused]] void *syncRoot;
            std::map<TKey, TValue> toMap() {
                std::map<TKey, TValue> ret;
                auto lst = entries->template toCPPlist();
                for (auto enter : lst)
                    ret.insert(std::make_pair(enter.key, enter.value));
                return std::move(ret);
            }
            std::vector<TKey> getKeys() {
                std::vector<TKey> ret;
                auto lst = entries->template toCPPlist();
                for (auto enter : lst)
                    ret.push_back(enter.key);
                return std::move(ret);
            }
            std::vector<TValue> getValues() {
                std::vector<TValue> ret;
                auto lst = entries->template toCPPlist();
                for (auto enter : lst)
                    ret.push_back(enter.value);
                return std::move(ret);
            }
#endif
            int getSize() { return count; }
            [[maybe_unused]] int getVersion() { return version; }
            bool TryGet(TKey key, TValue &value) { return BetterCall<bool>(LoadClass((IL2CPP::Il2CppObject*)this).GetMethodInfoByName(OBFUSCATE_BNM("TryGetValue"), 2), this, key, value); }
            [[maybe_unused]] void Add(TKey key, TValue value) { return BetterCall<void>(LoadClass((IL2CPP::Il2CppObject*)this).GetMethodInfoByName(OBFUSCATE_BNM("Add"), 2), this, key, value); }
            [[maybe_unused]] void Insert(TKey key, TValue value) { return BetterCall<void>(LoadClass((IL2CPP::Il2CppObject*)this).GetMethodInfoByName(OBFUSCATE_BNM("set_Item"), 2), this, key, value); }
            [[maybe_unused]] bool Remove(TKey key) { return BetterCall<bool>(LoadClass((IL2CPP::Il2CppObject*)this).GetMethodInfoByName(OBFUSCATE_BNM("Remove"), 1), this, key); }
            [[maybe_unused]] bool ContainsKey(TKey key) { return BetterCall<bool>(LoadClass((IL2CPP::Il2CppObject*)this).GetMethodInfoByName(OBFUSCATE_BNM("ContainsKey"), 1), this, key); }
            [[maybe_unused]] bool ContainsValue(TValue value) { return BetterCall<bool>(LoadClass((IL2CPP::Il2CppObject*)this).GetMethodInfoByName(OBFUSCATE_BNM("ContainsValue"), 1), this, value); }
            TValue Get(TKey key) {
                TValue ret;
                if (TryGet(key, ret))
                    return ret;
                return {};
            }
            TValue operator [](TKey key)  { return Get(key); }
        };
    }
    struct TypeFinder {
        bool byNameOnly;
        const char *name;
        const char *_namespace;
        bool withMethodName;
        const char *methodName;
        LoadClass ToLC() const;
        IL2CPP::Il2CppType* ToIl2CppType() const;
        IL2CPP::Il2CppClass* ToIl2CppClass() const;
        operator IL2CPP::Il2CppType*() const;
        operator IL2CPP::Il2CppClass*() const;
        operator LoadClass() const;
    };
#if __cplusplus >= 201703
    namespace NEW_CLASSES {
        struct NewMethod {
            NewMethod();
            IL2CPP::MethodInfo *thisMethod{};
            const char* Name{};
            const IL2CPP::MethodInfo *virtualMethod{};
            TypeFinder ret_type{};
            std::vector<TypeFinder> *args_types{};
            bool isStatic = false;
        };
        struct NewField {
            NewField();
            const char* Name{};
            int32_t offset{};
            int32_t size{};
            unsigned int attributes : 16{};
            TypeFinder type{};
            int32_t cppOffset{};
        };
        struct NewClass {
            NewClass();
            size_t size{};
            IL2CPP::Il2CppClass *thisClass{};
            const char* myNamespace{};
            const char* Name{};
            const char* BaseNamespace = OBFUSCATE_BNM("");
            const char* BaseName = OBFUSCATE_BNM("");
            const char* DllName{};
            int classType = 0;
            std::vector<NewMethod *> Methods4Add{};
            std::vector<NewField *> Fields4Add{};
            std::vector<NewField *> StaticFields4Add{};
            std::vector<IL2CPP::Il2CppClass *> Interfaces;
            int32_t staticFieldOffset = 0x0;
            size_t staticFieldsAddress{};
            void AddNewField(NewField *field, bool isStatic = false);
            void AddNewMethod(NewMethod *method);
        };
        void AddNewClass(NEW_CLASSES::NewClass *klass);
        template<typename Q>
        static inline Q UnpackArg(void* arg) {
            if constexpr (std::is_pointer_v<Q>)
                return (Q)arg;
            else
                return *(Q *)arg;
        }
        template<typename> struct GetNewMethodCalls {};
        template<typename> struct GetNewStaticMethodCalls {};
        template<typename RetT, typename T, typename ...ArgsT>
        struct GetNewMethodCalls<RetT(T::*)(ArgsT...)> {
            template<RetT(T::* member)(ArgsT...)>
            static RetT get(T* self, ArgsT ...args) { return (self->*member)(args...); }
            template<std::size_t ...As>
            static void* InvokeMethod(RetT(*func)(T*, ArgsT...), T* instance, void** args, std::index_sequence<As...>) {
                if constexpr (std::is_same_v<RetT, void>) {
                    func(instance, UnpackArg<ArgsT>(args[As])...);
                    return nullptr;
                } else
                    return func(instance, UnpackArg<ArgsT>(args[As])...);
            }
            static void* invoke(IL2CPP::Il2CppMethodPointer ptr, [[maybe_unused]] IL2CPP::MethodInfo* m, void* obj, void** args) {
                auto func = (RetT(*)(T*, ArgsT...))(ptr);
                auto instance = (T*)(obj);
                std::index_sequence<sizeof...(ArgsT)> a;
                auto seq = std::make_index_sequence<sizeof...(ArgsT)>();
                return InvokeMethod(func, instance, args, seq);
            }
        };
#if UNITY_VER > 174
        template<typename RetT, typename ...ArgsT>
        struct GetNewStaticMethodCalls<RetT(*)(ArgsT...)> {
            template<std::size_t ...As>
            static void* InvokeMethod(RetT(*func)(ArgsT...), void** args, std::index_sequence<As...>) {
                if constexpr (std::is_same_v<RetT, void>) {
                    func(UnpackArg<ArgsT>(args[As])...);
                    return nullptr;
                } else
                    return func(UnpackArg<ArgsT>(args[As])...);
            }
            static void* invoke(IL2CPP::Il2CppMethodPointer ptr, [[maybe_unused]] IL2CPP::MethodInfo* m, [[maybe_unused]] void* obj, void** args) {
                auto func = (RetT(*)(ArgsT...))(ptr);
                std::index_sequence<sizeof...(ArgsT)> a;
                auto seq = std::make_index_sequence<sizeof...(ArgsT)>();
                return InvokeMethod(func, args, seq);
            }
        };
#else
        template<typename RetT, typename ...ArgsT>
        struct GetNewStaticMethodCalls<RetT(*)(void *, ArgsT...)> {
            template<std::size_t ...As>
            static void* InvokeMethod(RetT(*func)(void *, ArgsT...), void** args, std::index_sequence<As...>) {
                if constexpr (std::is_same_v<RetT, void>) {
                    func(nullptr, UnpackArg<ArgsT>(args[As])...);
                    return nullptr;
                } else
                    return func(nullptr, UnpackArg<ArgsT>(args[As])...);
            }
            static void* invoke(IL2CPP::Il2CppMethodPointer ptr, IL2CPP::MethodInfo* m, void* obj, void** args) {
                auto func = (RetT(*)(void *, ArgsT...))(ptr);
                std::index_sequence<sizeof...(ArgsT)> a;
                auto seq = std::make_index_sequence<sizeof...(ArgsT)>();
                return InvokeMethod(func, args, seq);
            }
        };
#endif
    }
#endif
    /********** BNM METHODS **************/
    // Get game mono type name at compile time
    [[maybe_unused]] constexpr TypeFinder GetGameType(const char *_namespace, const char *name, bool withMethodName = false, const char *methodName = OBFUSCATE_BNM("")) { return TypeFinder{false, name, _namespace, withMethodName, methodName}; }
    DWORD GetOffset(IL2CPP::FieldInfo *field);
    DWORD GetOffset(IL2CPP::MethodInfo *methodInfo);
    char* str2char(const std::string& str);
    MONO_STRUCTS::monoString *CreateMonoString(const char *str);
    [[maybe_unused]] MONO_STRUCTS::monoString *CreateMonoString(const std::string& str);
    void *getExternMethod(const std::string& str);
    bool Il2cppLoaded();
    [[maybe_unused]] void AttachIl2Cpp();
    [[maybe_unused]] void DetachIl2Cpp();
    // Get Il2Cpp mono type name at compile time
    template<typename T>
    constexpr TypeFinder GetType() {
        if (std::is_same_v<T, void>)
            return {true, OBFUSCATE_BNM("Void")};
        else if (std::is_same_v<T, bool>)
            return {true, OBFUSCATE_BNM("Boolean")};
        else if (std::is_same_v<T, uint8_t> || std::is_same_v<T, unsigned char>)
            return {true, OBFUSCATE_BNM("Byte")};
        else if (std::is_same_v<T, int8_t>)
            return {true, OBFUSCATE_BNM("SByte")};
        else if (std::is_same_v<T, int16_t>)
            return {true, OBFUSCATE_BNM("Int16")};
        else if (std::is_same_v<T, uint16_t>)
            return {true, OBFUSCATE_BNM("UInt16")};
        else if (std::is_same_v<T, int32_t>)
            return {true, OBFUSCATE_BNM("Int32")};
        else if (std::is_same_v<T, uint32_t>)
            return {true, OBFUSCATE_BNM("UInt32")};
        else if (std::is_same_v<T, intptr_t>)
            return {true, OBFUSCATE_BNM("IntPtr")};
        else if (std::is_same_v<T, int64_t>)
            return {true, OBFUSCATE_BNM("Int64")};
        else if (std::is_same_v<T, uint64_t>)
            return {true, OBFUSCATE_BNM("UInt64")};
        else if (std::is_same_v<T, float>)
            return {true, OBFUSCATE_BNM("Single")};
        else if (std::is_same_v<T, double>)
            return {true, OBFUSCATE_BNM("Double")};
        else if (std::is_same_v<T, BNM::IL2CPP::Il2CppString *> || std::is_same_v<T, BNM::MONO_STRUCTS::monoString *>)
            return {true, OBFUSCATE_BNM("String")};
        else if (std::is_same_v<T, BNM::UNITY_STRUCTS::Vector3>)
            return {false, OBFUSCATE_BNM("Vector3"), OBFUSCATE_BNM("UnityEngine")};
        else if (std::is_same_v<T, BNM::UNITY_STRUCTS::Vector2>)
            return {false, OBFUSCATE_BNM("Vector2"), OBFUSCATE_BNM("UnityEngine")};
        else if (std::is_same_v<T, BNM::UNITY_STRUCTS::Color>)
            return {false, OBFUSCATE_BNM("Color"), OBFUSCATE_BNM("UnityEngine")};
        else if (std::is_same_v<T, BNM::UNITY_STRUCTS::Ray>)
            return {false, OBFUSCATE_BNM("Ray"), OBFUSCATE_BNM("UnityEngine")};
        else if (std::is_same_v<T, BNM::UNITY_STRUCTS::RaycastHit>)
            return {false, OBFUSCATE_BNM("RaycastHit"), OBFUSCATE_BNM("UnityEngine")};
        else
            return {true, OBFUSCATE_BNM("Object")};
    }
    template<typename T>
    [[maybe_unused]] static T UnboxObject(T obj) {
        return (T) (void *) (((char *) obj) + sizeof(BNM::IL2CPP::Il2CppObject));
    }
    template<typename Ret, typename ... Args>
    Ret BetterCall(IL2CPP::MethodInfo *method, void *instance, Args... args) {
        return ((Ret (*)(void *, Args..., BNM::IL2CPP::MethodInfo *))method->methodPointer)(instance, args..., method);
    }
    template<typename Ret, typename ... Args>
    [[maybe_unused]] Ret BetterStaticCall(IL2CPP::MethodInfo *method, Args... args) {
#if UNITY_VER > 174
        return ((Ret (*)(Args..., BNM::IL2CPP::MethodInfo *))method->methodPointer)(args..., method);
#else
        return ((Ret (*)(void *, Args..., BNM::IL2CPP::MethodInfo *))method->methodPointer)(nullptr, args..., method);
#endif
    }
    // Converts offset in memory to offset in libil2cpp.so
    [[maybe_unused]] auto offsetInLib = [](auto offsetInMemory) {
        Dl_info info;
        dladdr((void *)offsetInMemory, &info);
        return offsetInMemory - (DWORD)info.dli_fbase;
    };
}
#define InitResolveFunc(x, y) BNM::InitFunc(x, BNM::getExternMethod(y))

#if __cplusplus >= 201703
#define BNM_NewClassInit(_namespace, name, base_namespace, base_name, base_size)\
    private: \
    struct _BNMClass : BNM::NEW_CLASSES::NewClass { \
        _BNMClass() { \
            Name = OBFUSCATE_BNM(#name); \
            myNamespace = OBFUSCATE_BNM(_namespace); \
            BaseName = OBFUSCATE_BNM(base_name); \
            BaseNamespace = OBFUSCATE_BNM(base_namespace); \
            DllName = OBFUSCATE_BNM("ByNameModding"); \
            this->size = sizeof(Me_Type); \
            BNM::NEW_CLASSES::AddNewClass(this); \
        } \
    }; \
    public: \
    static inline _BNMClass BNMClass = _BNMClass(); \
    uint8_t _baseFields[base_size]{}; \
    using Me_Type = name

#define BNM_NewMethodInit(_type, _name, args, ...) \
    private: \
    struct _BNMMethod_##_name : BNM::NEW_CLASSES::NewMethod { \
        _BNMMethod_##_name() { \
            thisMethod = new BNM::IL2CPP::MethodInfo(); \
            thisMethod->parameters_count = args; \
            ret_type = _type; \
            thisMethod->methodPointer = (BNM::IL2CPP::Il2CppMethodPointer)&BNM::NEW_CLASSES::GetNewMethodCalls<decltype(&Me_Type::_name)>::get<&Me_Type::_name>; \
            thisMethod->invoker_method = (BNM::IL2CPP::InvokerMethod)&BNM::NEW_CLASSES::GetNewMethodCalls<decltype(&Me_Type::_name)>::invoke; \
            args_types = new std::vector<BNM::TypeFinder>({__VA_ARGS__}); \
            Name = OBFUSCATE_BNM(#_name); \
            BNMClass.AddNewMethod(this); \
        } \
    }; \
    public: \
    static inline _BNMMethod_##_name BNMMethod_##_name = _BNMMethod_##_name()

#define BNM_NewStaticMethodInit(_type, _name, args, ...) \
    private: \
    struct _BNMStaticMethod_##_name : BNM::NEW_CLASSES::NewMethod { \
        _BNMStaticMethod_##_name() { \
            thisMethod = new BNM::IL2CPP::MethodInfo(); \
            thisMethod->parameters_count = args; \
            ret_type = _type; \
            thisMethod->methodPointer = (BNM::IL2CPP::Il2CppMethodPointer)&Me_Type::_name; \
            thisMethod->invoker_method = (BNM::IL2CPP::InvokerMethod)&BNM::NEW_CLASSES::GetNewStaticMethodCalls<decltype(&Me_Type::_name)>::invoke; \
            args_types = new std::vector<BNM::TypeFinder>({__VA_ARGS__}); \
            Name = OBFUSCATE_BNM(#_name); \
            isStatic = true; \
            BNMClass.AddNewMethod(this); \
        } \
    }; \
    public: \
    static inline _BNMStaticMethod_##_name BNMStaticMethod_##_name = _BNMStaticMethod_##_name()

#define BNM_NewFieldInit(_name, type) \
    private: \
    struct _BNMField_##_name : BNM::NEW_CLASSES::NewField { \
        _BNMField_##_name() { \
            Name = OBFUSCATE_BNM(#_name); \
            offset = offsetof(Me_Type, _name); \
            attributes |= 0x0006;      \
            MYtype = type; \
            BNMClass.AddNewField(this, false); \
        } \
    }; \
    static inline _BNMField_##_name BNMField_##_name = _BNMField_##_name()

#define BNM_NewStaticFieldInit(_name, type, cppType) \
    private: \
    struct _BNMStaticField_##_name : BNM::NEW_CLASSES::NewField { \
        _BNMStaticField_##_name() { \
            Name = OBFUSCATE_BNM(#_name); \
            size = sizeof(cppType); \
            cppOffset = (size_t)&(_name); \
            attributes |= 0x0006 | 0x0010; \
            MYtype = type; \
            BNMClass.AddNewField(this, true); \
        } \
    }; \
    static inline _BNMStaticField_##_name BNMStaticField_##_name = _BNMStaticField_##_name()

// Add class to exist or to new dll. Write dll name without '.dll'!
#define BNM_NewClassWithDllInit(dll, _namespace, name, base_namespace, base_name, base_size, type)\
    private: \
    struct _BNMClass : BNM::NEW_CLASSES::NewClass { \
        _BNMClass() { \
            Name = OBFUSCATE_BNM(#name); \
            myNamespace = OBFUSCATE_BNM(_namespace); \
            BaseName = OBFUSCATE_BNM(base_name); \
            BaseNamespace = OBFUSCATE_BNM(base_namespace); \
            DllName = OBFUSCATE_BNM(dll); \
            classType = type;\
            this->size = sizeof(Me_Type); \
            BNM::NEW_CLASSES::AddNewClass(this); \
        } \
    }; \
    public: \
    static inline _BNMClass BNMClass = _BNMClass(); \
    uint8_t _baseFields[base_size]{}; \
    using Me_Type = name
#endif