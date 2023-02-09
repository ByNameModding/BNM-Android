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
#include <typeinfo>
#include <csetjmp>
#include <signal.h>
#ifndef _WIN32
#include <jni.h>
#endif
#include "BNM_settings.hpp"
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
#elif UNITY_VER >= 221
#include "BNM_data/Il2CppHeaders/2022.1.h"
#else
#include "BNM_data/Il2CppHeaders/2020.3.h"
#endif
    }
    typedef IL2CPP::Il2CppReflectionType MonoType;
    typedef std::vector<IL2CPP::Il2CppAssembly *> AssemblyVector;
    typedef std::vector<IL2CPP::Il2CppClass *> TypeVector;

    namespace UNITY_STRUCTS {
#include "BNM_data/BasicStructs.h"
    }
    auto isAllocated = [](auto x) -> bool {
        static jmp_buf jump;
	    static sighandler_t handler = [](int) { longjmp(jump, 1); };
	    [[maybe_unused]] volatile char c;
	    volatile bool ok = true;
	    volatile sighandler_t old_handler = signal(SIGSEGV, handler);
	    if (!setjmp (jump)) c = *(char *) (x); else ok = false;
	    signal(SIGSEGV, old_handler);
	    return ok;
    };
    template<typename T>
    T CheckObj(T obj) {
        static_assert(std::is_pointer<T>::value, "Expected a pointer in CheckObj");
        if (obj && isAllocated(obj)) return obj;
        return nullptr;
    }
    namespace UnityEngine {
        // Can be used for NewClass if Base type is UnityEngine.Object, MonoBehaviour, ScriptableObject
        // For System.Object use BNM::IL2CPP::Il2CppObject
        struct Object : public BNM::IL2CPP::Il2CppObject {
            BNM_INT_PTR m_CachedPtr = 0;
            bool Alive() { return BNM::CheckObj((void *)this) && (BNM_PTR)m_CachedPtr; }
        };
    }
    char *str2char(const std::string &str);
    // Only if obj child of UnityEngine.Object or object is UnityEngine.Object
    [[maybe_unused]] auto IsUnityObjectAlive = [](auto o) {
        return ((UnityEngine::Object *)o)->Alive();
    };
    // Only if objects children of UnityEngine.Object or objects are UnityEngine.Object
    [[maybe_unused]] auto IsSameUnityObject = [](auto o1, auto o2) {
        auto obj1 = (UnityEngine::Object *)o1;
        auto obj2 = (UnityEngine::Object *)o2;
        return (!obj1->Alive() && !obj2->Alive()) || (obj1->Alive() && obj2->Alive() && obj1->m_CachedPtr == obj2->m_CachedPtr);
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
            [[maybe_unused]] static monoString *Create(const std::string &str);
            [[maybe_unused]] static monoString *Empty();
        };
        template<typename T>
        struct monoArray {
            IL2CPP::Il2CppClass *klass;
            IL2CPP::MonitorData *monitor;
            IL2CPP::Il2CppArrayBounds *bounds;
            IL2CPP::il2cpp_array_size_t capacity;
            T m_Items[0];
            [[maybe_unused]] IL2CPP::il2cpp_array_size_t getCapacity() { if (!this) return 0; return capacity; }
            T *getPointer() { if (!this) return nullptr; return m_Items; }
            std::vector<T> toCPPlist() {
                if (!this) return {};
                std::vector<T> ret;
                for (int i = 0; i < capacity; i++)
                    ret.push_back(m_Items[i]);
                return std::move(ret);
            }
            bool copyFrom(const std::vector<T> &vec) { if (!this) return false; return copyFrom((T *)vec.data(), (int)vec.size()); }
            [[maybe_unused]] bool copyFrom(T *arr, int size) {
                if (!this) return false;
                if (size < capacity)
                    return false;
                memcpy(m_Items, arr, size  *sizeof(T));
                return true;
            }
            [[maybe_unused]] void copyTo(T *arr) { if (!this || !CheckObj(m_Items)) return; memcpy(arr, m_Items, sizeof(T)  *capacity); }
            T operator[] (int index) { if (getCapacity() < index) return {}; return m_Items[index]; }
            T at(int index) { if (!this || getCapacity() <= index || empty()) return {}; return m_Items[index]; }
            bool empty() { if (!this) return false; return getCapacity() <= 0;}
            static monoArray<T> *Create(int capacity) {
                auto monoArr = (monoArray<T> *)malloc(sizeof(monoArray) + sizeof(T)  *capacity);
                monoArr->capacity = capacity;
                return monoArr;
            }
            [[maybe_unused]] static monoArray<T> *Create(const std::vector<T> &vec) { return Create(vec.data(), vec.size()); }
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
            T *getItems() { return items->getPointer(); }
            [[maybe_unused]] int getSize() { return size; }
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
                    memcpy(nItems->m_Items, items->m_Items, items->capacity  *sizeof(T));
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
                    memset(items + size + delta, 0, -delta  *sizeof(T));
            }
        };
    }
    template<typename T = int>
    struct Field;
    template<typename T>
    struct Property;
    auto InitFunc = [](auto&& method, auto ptr) {
        if (ptr != 0)
            *(void * *)(&method) = (void *)(ptr);
    };
    template<typename Ret>
    struct Method;
    struct LoadClass {
        IL2CPP::Il2CppClass *klass{};
        LoadClass() noexcept;
        LoadClass(const IL2CPP::Il2CppClass *clazz);
        LoadClass(const IL2CPP::Il2CppObject *obj);
        [[maybe_unused]] LoadClass(const IL2CPP::Il2CppType *type);
        [[maybe_unused]] LoadClass(const MonoType *type);
        LoadClass(const std::string &_namespace, const std::string &_name);
        LoadClass(const std::string &_namespace, const std::string &_name, const std::string &dllName);
        [[maybe_unused]] std::vector<LoadClass> GetInnerClasses(bool includeParent = true) const;
        [[maybe_unused]] std::vector<IL2CPP::FieldInfo *> GetFieldsInfo(bool includeParent = true) const;
        [[maybe_unused]] std::vector<IL2CPP::MethodInfo *> GetMethodsInfo(bool includeParent = true) const;
        Method<void> GetMethodByName(const std::string &name, int parameters = -1) const;
        Method<void> GetMethodByName(const std::string &name, const std::vector<std::string> &params_names) const;
        Method<void> GetMethodByName(const std::string &name, const std::vector<std::string> &params_names, const std::vector<IL2CPP::Il2CppType *> &params_types) const;
        Method<void> GetMethodByName(const std::string &name, const std::vector<IL2CPP::Il2CppType *> &params_types) const;
        [[maybe_unused]] Property<bool> GetPropertyByName(const std::string &name, bool warning = false); // warning if property without get or set
        [[maybe_unused]] LoadClass GetInnerClass(const std::string &_name) const;
        [[maybe_unused]] LoadClass GetArrayClass() const;
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
            BNM::MONO_STRUCTS::monoList<T> *lst = (decltype(lst)) ObjNew(ArrClass);
            lst->items = array;
            return lst;
        }
        template<typename T>
        [[maybe_unused]] IL2CPP::Il2CppObject *BoxObject(T obj) {
            if (!klass) return nullptr;
            TryInit();
            return ObjBox(klass, (void *) obj);
        }
        Field<int> GetFieldByName(const std::string &name) const;
        template<typename ...Args>
        [[maybe_unused]] void *CreateNewObject(Args ... args) { return CreateNewObjectCtor(sizeof...(Args), {}, args...); }
        template<typename ...Args>
        void *CreateNewObjectCtor(int args_count, const std::vector<std::string> &arg_names, Args ... args);
        static bool ClassExists(const std::string &_namespace, const std::string &_name, const std::string &dllName);
    private:
        void TryInit() const;
        static IL2CPP::Il2CppObject *ObjBox(IL2CPP::Il2CppClass*, void *);
        static IL2CPP::Il2CppObject *ObjNew(IL2CPP::Il2CppClass *);
        static IL2CPP::Il2CppArray *ArrayNew(IL2CPP::Il2CppClass*, IL2CPP::il2cpp_array_size_t);
    };
    // For thread static fields
    namespace PRIVATE_FILED_UTILS {
        void GetStaticValue(IL2CPP::FieldInfo *info, void *value);
        void SetStaticValue(IL2CPP::FieldInfo *info, void *value);
    }
    template<typename T>
    struct Field {
        static bool CheckIsStatic(IL2CPP::FieldInfo *field) {
            if (!field || !field->type)
                return false;
            return (field->type->attrs & 0x0010) != 0 && field->offset != -1 && (field->type->attrs & 0x0040) == 0;
        }
        IL2CPP::FieldInfo *myInfo{};
        bool init = false, thread_static = false, isStatic = false;
        IL2CPP::Il2CppObject *instance{};
        Field() noexcept = default;
        template<typename otherT>
        [[maybe_unused]] Field(Field<otherT> f) : Field(f.myInfo) {
            if (f.Initialized() && !f.isStatic && !f.thread_static && BNM::CheckObj(f.instance))
                setInstance(f.instance);
        }
        Field(IL2CPP::FieldInfo *info) {
            init = BNM::CheckObj(info);
            if (init) {
                isStatic = CheckIsStatic(info);
                myInfo = info;
                thread_static = myInfo->offset == -1;
            }
        }
        BNM_PTR GetOffset() {
            return myInfo->offset;
        }
        T *getPointer() {
            if (!init) return makeSafeRet();
            if (!isStatic && !CheckObj(instance)) {
                LOGEBNM(OBFUSCATE_BNM("Can't get non static %s pointer without instance! Please call setInstance before getting or setting field."), BNM::str2char(str()));
                return makeSafeRet();
            } else if (isStatic && !CheckObj(myInfo->parent)) {
                LOGEBNM(OBFUSCATE_BNM("Something went wrong, %s field has null parent class."), BNM::str2char(str()));
                return makeSafeRet();
            } else if (thread_static) {
                LOGEBNM(OBFUSCATE_BNM("Thread static pointer don't supported, %s."), BNM::str2char(str()));
                return makeSafeRet();
            }
            if (isStatic)
                return (T *) ((BNM_PTR) myInfo->parent->static_fields + myInfo->offset);
            return (T *) ((BNM_PTR) instance + myInfo->offset);
        }
        T get() {
            if (!init) return {};
            if (thread_static) {
                T val{};
                PRIVATE_FILED_UTILS::GetStaticValue(myInfo, (void *)&val);
                return val;
            }
            return *getPointer();
        }
        void set(T val) {
            if (!init) return;
            if (thread_static) {
                PRIVATE_FILED_UTILS::SetStaticValue(myInfo, (void *)&val);
                return;
            }
            *getPointer() = val;
        }
        Field<T> &setInstance(IL2CPP::Il2CppObject *val, bool doWarn = true) {
            if (init && isStatic) {
                if (doWarn) LOGWBNM(OBFUSCATE_BNM("Trying set instance of static field %s. Please remove setInstance in code."), BNM::str2char(str()));
                return *this;
            }
            init = val && myInfo != nullptr;
            instance = val;
            return *this;
        }
        Field<T> &operator=(T val) {
            set(val);
            return *this;
        }
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
        operator T() {
            return get();
        }
        T operator()() {
            return get();
        }
        Field<T> &operator[](void *val) {
            return setInstance((IL2CPP::Il2CppObject *)val);
        }
        Field<T> &operator[](IL2CPP::Il2CppObject *val) {
            return setInstance((IL2CPP::Il2CppObject *)val);
        }
        Field<T> &operator[](UnityEngine::Object *val) {
            return setInstance((IL2CPP::Il2CppObject *)val);
        }
        template<typename NewT>
        Field<NewT> setType() {
            return Field<NewT>(myInfo).setInstance(instance, false);
        }
        std::string str() {
            if (init) return LoadClass(myInfo->parent).GetClassName() + OBFUSCATE_BNM(".(") + myInfo->name + OBFUSCATE_BNM(")");
            return OBFUSCATE_BNM("Uninitialized field");
        }
        bool Initialized() noexcept { return init; }
    private:
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
        template<typename T = void>
        Method(Method<T> m) : Method(m.myInfo) {
            if (m.Initialized() && !m.isStatic && BNM::CheckObj(m.instance))
                setInstance(m.instance);
        }
        Method(const IL2CPP::MethodInfo *info) {
            init = BNM::CheckObj(info);
            if (init) {
                isStatic = info->flags & 0x0010;
                isVirtual = info->slot != IL2CPP::kInvalidIl2CppMethodSlot;
                myInfo = (decltype(myInfo)) info;
            }
        }
        Method<Ret> &setInstance(IL2CPP::Il2CppObject *val, bool doWarn = true) {
            if (!init) return *this;
            if (init && isStatic) {
                if (doWarn)
                    LOGWBNM(OBFUSCATE_BNM("Trying set instance of static method %s. Please remove setInstance in code."), BNM::str2char(str()));
                return *this;
            }
            instance = val;
            return *this;
        }
        inline Method<Ret> &operator[](void *val) {
            return setInstance((IL2CPP::Il2CppObject *)val);
        }
        inline Method<Ret> &operator[](IL2CPP::Il2CppObject *val) {
            return setInstance((IL2CPP::Il2CppObject *)val);
        }
        inline Method<Ret> &operator[](UnityEngine::Object *val) {
            return setInstance((IL2CPP::Il2CppObject *)val);
        }
        template<typename ...Args>
        Ret call(Args...args) {
            if (!init) return SafeReturn<Ret>();
            bool canInfo = true;
            if (sizeof...(Args) != myInfo->parameters_count){
                canInfo = false;
                LOGWBNM(OBFUSCATE_BNM("Trying to call %s with wrong parameters count... I hope you know what you're doing. Just I can't add MethodInfo to args(. Please try fix this."), BNM::str2char(str()));
            }
            if (!isStatic && !CheckObj(instance)) {
                LOGEBNM(OBFUSCATE_BNM("Can't call non static %s without instance! Please call setInstance before calling method."), BNM::str2char(str()));
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

        template<typename ...Args>
        inline Ret operator ()(Args...args) {
            return call(args...);
        }
        std::string str() {
#if UNITY_VER > 174
#define kls klass
#else
#define kls declaring_type
#endif
            if (init) {
                return LoadClass(myInfo->return_type).GetClassName() + OBFUSCATE_BNM(" ") +
                       LoadClass(myInfo->kls).GetClassName() + OBFUSCATE_BNM(".[") +
                       myInfo->name + OBFUSCATE_BNM("]{Args count:") +
                       std::to_string(myInfo->parameters_count) + OBFUSCATE_BNM("}") +
                       (isStatic ? OBFUSCATE_BNM("(static)") : OBFUSCATE_BNM(""));
            }
            return OBFUSCATE_BNM("Uninitialized method");
#undef kls
        }
        IL2CPP::MethodInfo *GetInfo() {
            if (init) return myInfo;
            return {};
        }
        BNM_PTR GetOffset() {
            if (init) return (BNM_PTR) myInfo->methodPointer;
            return {};
        }
        bool Initialized() noexcept {
            return init;
        }
        operator bool() noexcept {
            return Initialized();
        }
        template<typename NewRet>
        [[maybe_unused]] Method<NewRet> setRet() {
            return Method<NewRet>(myInfo).setInstance(instance, false);
        }
        template<typename other>
        Method<Ret> &operator=(Method<other> m) {
            init = BNM::CheckObj(m.myInfo);
            if (init) {
                isStatic = m.myInfo->flags & 0x0010;
                isVirtual = m.myInfo->slot != IL2CPP::kInvalidIl2CppMethodSlot;
                myInfo = (decltype(myInfo)) m.myInfo;
            }
            if (m.Initialized() && !m.isStatic && BNM::CheckObj(m.instance))
                setInstance(m.instance);
            return *this;
        }
    private: // Crutch)
        template<typename T>static T SafeReturn(){return{};}
        template<>static void SafeReturn(){}
    };
    template<typename T = bool>
    struct Property {
        Property() noexcept = default;
        template<typename V>
        Property(Method<V> getter, Method<void> setter) {
            this->getter = getter;
            this->setter = setter;
        }
        Method<T> getter;
        Method<void> setter;
        Property<T> &setInstance(IL2CPP::Il2CppObject *val, bool doWarn = true) {
            getter.setInstance(val, doWarn);
            setter.setInstance(val, doWarn);
            return *this;
        }
        inline Property<T> &operator[](void *val) {
            return setInstance((IL2CPP::Il2CppObject *)val);
        }
        inline Property<T> &operator[](IL2CPP::Il2CppObject *val) {
            return setInstance((IL2CPP::Il2CppObject *)val);
        }
        inline Property<T> &operator[](UnityEngine::Object *val) {
            return setInstance((IL2CPP::Il2CppObject *)val);
        }
        T get() {
            return getter();
        }
        void set(T v) {
            return setter(v);
        }
        operator T() {
            return get();
        }
        T operator()() {
            return get();
        }
        template<typename V>
        Property<T> &operator=(Property<V> val) { // We don't need to set Instance here because it saved in methods
            getter = val.getter;
            setter = val.setter;
            return *this;
        }
        Property<T> &operator=(T val) {
            set(val);
            return *this;
        }
        [[maybe_unused]] bool Initialized() noexcept {
            return getter.init || setter.init;
        }
        template<typename NewRet>
        [[maybe_unused]] Property<NewRet> setRet() {
            return Property<NewRet>(Method<NewRet>(getter), setter).setInstance(getter.instance ? getter.instance : setter.instance, false);
        }
    };
    template<typename ...Args>
    void *LoadClass::CreateNewObjectCtor(int args_count, const std::vector<std::string> &arg_names, Args... args) {
        if (!klass) return nullptr;
        TryInit();
        auto method = arg_names.empty() ? GetMethodByName(OBFUSCATES_BNM(".ctor"), args_count) : GetMethodByName(OBFUSCATES_BNM(".ctor"), arg_names);
        auto instance = CreateNewInstance();
        method[instance](args...);
        return instance;
    }
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
                    if (((*linkSlots)[i].HashCode  &-2147483648) != 0)
                        ret.insert(std::make_pair((*keys)[i], (*values)[i]));
                return std::move(ret);
            }
            std::vector<TKey> getKeys() {
                std::vector<TKey> ret;
                for (int i = 0; i < touchedSlots; i++)
                    if (((*linkSlots)[i].HashCode  &-2147483648) != 0)
                        ret.push_back((*keys)[i]);
                return std::move(ret);
            }
            std::vector<TValue> getValues() {
                std::vector<TValue> ret;
                for (int i = 0; i < touchedSlots; i++)
                    if (((*linkSlots)[i].HashCode  &-2147483648) != 0)
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
            bool TryGet(TKey key, TValue *value) { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("TryGetValue"), 2).template setRet<bool>().setInstance(this)(key, value); }
            [[maybe_unused]] void Add(TKey key, TValue value) { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("Add"), 2).setInstance(this)(key, value); }
            [[maybe_unused]] void Insert(TKey key, TValue value) { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("set_Item"), 2).setInstance(this)(key, value); }
            [[maybe_unused]] bool Remove(TKey key) { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("Remove"), 1).template setRet<bool>().setInstance(this)( key); }
            [[maybe_unused]] bool ContainsKey(TKey key) { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("ContainsKey"), 1).template setRet<bool>().setInstance(this)(key); }
            [[maybe_unused]] bool ContainsValue(TValue value) { return LoadClass((IL2CPP::Il2CppObject *)this).GetMethodByName(OBFUSCATE_BNM("ContainsValue"), 1).template setRet<bool>().setInstance(this)(value); }
            TValue Get(TKey key) {
                TValue ret;
                if (TryGet(key, &ret)) return ret;
                return {};
            }
            TValue operator[](TKey key)  { return Get(key); }
        };
    }
    struct TypeFinder {
        const char *_namespace{};
        const char *name{};
        bool isArray = false;
        LoadClass ToLC() const;
        IL2CPP::Il2CppType *ToIl2CppType() const;
        IL2CPP::Il2CppClass *ToIl2CppClass() const;
        operator IL2CPP::Il2CppType*() const;
        operator IL2CPP::Il2CppClass*() const;
        operator LoadClass() const;
    };
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
    namespace NEW_CLASSES {
        struct NewMethod {
            NewMethod() noexcept;
            IL2CPP::MethodInfo *thisMethod{};
            uint8_t argsCount = 0;
            void *address{}, *invoker_address{};
            const char *Name{};
            [[maybe_unused]] const IL2CPP::MethodInfo *virtualMethod{};
            TypeFinder ret_type{};
            std::vector<TypeFinder> args_types{};
            bool isStatic = false;
        };
        struct NewField {
            NewField() noexcept;
            const char *Name{};
            int offset{}, size{};
            unsigned int attributes : 16{};
            TypeFinder type{};
            BNM_PTR cppOffset{};
            bool isStatic = false;
        };
        struct NewClass {
            NewClass() noexcept;
            size_t size{};
            IL2CPP::Il2CppClass *thisClass{};
            MonoType *type{};
            const char *myNamespace{}, *Name{}, *BaseNamespace{}, *BaseName{}, *DllName{};
            uint8_t classType = 0x12;
            std::vector<NewMethod *> Methods4Add{};
            std::vector<NewField *> Fields4Add{};
            std::vector<TypeFinder> Interfaces{};
            int staticFieldOffset = 0x0;
            BNM_PTR staticFieldsAddress{};
            void AddNewField(NewField *field, bool isStatic = false);
            void AddNewMethod(NewMethod *method);
        };
        void AddNewClass(NEW_CLASSES::NewClass *klass);
        template<typename Q>
        static inline Q UnpackArg(void *arg) {
            if constexpr (std::is_pointer_v<Q>) return (Q)arg;
            else return *(Q *)arg;
        }
        template<typename> struct GetNewMethodCalls {};
        template<typename> struct GetNewStaticMethodCalls {};
        template<typename RetT, typename T, typename ...ArgsT>
        struct GetNewMethodCalls<RetT(T:: *)(ArgsT...)> {
            template<RetT(T:: *member)(ArgsT...)>
            static RetT get(T *self, ArgsT ...args) { return (self->*member)(args...); }
            template<std::size_t ...As>
            static void *InvokeMethod(RetT(*func)(T*, ArgsT...), T *instance, void **args, std::index_sequence<As...>) {
                if constexpr (std::is_same_v<RetT, void>) {
                    func(instance, UnpackArg<ArgsT>(args[As])...);
                    return nullptr;
                } else
                    return func(instance, UnpackArg<ArgsT>(args[As])...);
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
        template<typename RetT, typename ...ArgsT>
        struct GetNewStaticMethodCalls<RetT(*)(ArgsT...)> {
            template<std::size_t ...As>
            static void *InvokeMethod(RetT(*func)(ArgsT...), void **args, std::index_sequence<As...>) {
                if constexpr (std::is_same_v<RetT, void>) {
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
    [[maybe_unused]] constexpr TypeFinder GetType(const char *_namespace, const char *name, bool isArray = false) noexcept { return TypeFinder{._namespace = _namespace, .name = name, .isArray = isArray}; }
    char *str2char(const std::string &str);
    MONO_STRUCTS::monoString *CreateMonoString(const char *str);
    [[maybe_unused]] MONO_STRUCTS::monoString *CreateMonoString(const std::string &str);
    void *getExternMethod(const std::string &str);
    bool Il2cppLoaded();
    void SetIl2CppLoadEvent(void (*event)());
    [[maybe_unused]] bool AttachIl2Cpp(); // Return true if need Detach
    [[maybe_unused]] IL2CPP::Il2CppThread *CurrentIl2CppThread();
    [[maybe_unused]] void DetachIl2Cpp();
    [[maybe_unused]] std::string GetLibIl2CppPath();
    [[maybe_unused]] BNM_PTR GetLibIl2CppOffset();
    // Don't close it! BNM will just crash without it.
    [[maybe_unused]] void *GetLibIl2CppDlInst();
    // Get Il2Cpp mono type name at compile time
    template<typename T>
    constexpr TypeFinder GetType(bool isArray = false) noexcept {
        if (std::is_same<T, void>::value)
            return {._namespace = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Void"), .isArray = isArray};
        else if (std::is_same<T, bool>::value)
            return {._namespace = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Boolean"), .isArray = isArray};
        else if (std::is_same<T, uint8_t>::value || std::is_same<T, unsigned char>::value)
            return {._namespace = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Byte"), .isArray = isArray};
        else if (std::is_same<T, int8_t>::value)
            return {._namespace = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("SByte"), .isArray = isArray};
        else if (std::is_same<T, int16_t>::value)
            return {._namespace = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Int16"), .isArray = isArray};
        else if (std::is_same<T, uint16_t>::value)
            return {._namespace = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("UInt16"), .isArray = isArray};
        else if (std::is_same<T, int32_t>::value)
            return {._namespace = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Int32"), .isArray = isArray};
        else if (std::is_same<T, uint32_t>::value)
            return {._namespace = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("UInt32"), .isArray = isArray};
        else if (std::is_same<T, intptr_t>::value)
            return {._namespace = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("IntPtr"), .isArray = isArray};
        else if (std::is_same<T, int64_t>::value)
            return {._namespace = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Int64"), .isArray = isArray};
        else if (std::is_same<T, uint64_t>::value)
            return {._namespace = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("UInt64"), .isArray = isArray};
        else if (std::is_same<T, float>::value)
            return {._namespace = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Single"), .isArray = isArray};
        else if (std::is_same<T, double>::value)
            return {._namespace = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Double"), .isArray = isArray};
        else if (std::is_same<T, BNM::IL2CPP::Il2CppString *>::value || std::is_same<T, BNM::MONO_STRUCTS::monoString *>::value)
            return {._namespace = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("String"), .isArray = isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::Vector3>::value)
            return {._namespace = OBFUSCATE_BNM("UnityEngine"), .name = OBFUSCATE_BNM("Vector3"), .isArray = isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::Vector2>::value)
            return {._namespace = OBFUSCATE_BNM("UnityEngine"), .name = OBFUSCATE_BNM("Vector2"), .isArray = isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::Color>::value)
            return {._namespace = OBFUSCATE_BNM("UnityEngine"), .name = OBFUSCATE_BNM("Color"), .isArray = isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::Ray>::value)
            return {._namespace = OBFUSCATE_BNM("UnityEngine"), .name = OBFUSCATE_BNM("Ray"), .isArray = isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::RaycastHit>::value)
            return {._namespace = OBFUSCATE_BNM("UnityEngine"), .name = OBFUSCATE_BNM("RaycastHit"), .isArray = isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::Quaternion>::value)
            return {._namespace = OBFUSCATE_BNM("UnityEngine"), .name = OBFUSCATE_BNM("Quaternion"), .isArray = isArray};
        else return {._namespace = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Object"), .isArray = isArray};
    }
    template<typename T>
    [[maybe_unused]] static T UnboxObject(T obj) {
        return (T) (void *) (((char *) obj) + sizeof(BNM::IL2CPP::Il2CppObject));
    }
    void InvokeHook(BNM::Method<int> m, void *newMet, void **oldMet);
    void InvokeHook(IL2CPP::MethodInfo *m, void *newMet, void **oldMet);
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, IL2CPP::Il2CppClass *klass) { return IsA<BNM::IL2CPP::Il2CppObject *>((IL2CPP::Il2CppObject *)object, klass); }
    template<> bool IsA<IL2CPP::Il2CppObject *>(IL2CPP::Il2CppObject *object, IL2CPP::Il2CppClass *klass);
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, LoadClass klass) { return IsA(object, klass.klass); }
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, IL2CPP::Il2CppObject *klass) { if (!klass) return false; return IsA(object, klass->klass); }
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, MonoType *type) { return IsA(object, LoadClass(type)); }
    // Try bypass any protection by getting full lib path (INTERNAL ONLY)
    [[maybe_unused]] void HardBypass(JNIEnv *env);
    namespace External {
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
}
#define InitResolveFunc(x, y) BNM::InitFunc(x, BNM::getExternMethod(y))

#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
#define BNM_NewClassInit(_namespace, name, base_namespace, base_name, ...) BNM_NewClassWithDllInit("Assembly-CSharp", _namespace, name, base_namespace, base_name, __VA_ARGS__)

#define BNM_NewMethodInit(_type, _name, args, ...) \
    private: \
    struct _BNMMethod_##_name : BNM::NEW_CLASSES::NewMethod { \
        _BNMMethod_##_name() { \
            argsCount = args; \
            ret_type = _type; \
            address = (void *)&BNM::NEW_CLASSES::GetNewMethodCalls<decltype(&Me_Type::_name)>::get<&Me_Type::_name>; \
            invoker_address = (void *)&BNM::NEW_CLASSES::GetNewMethodCalls<decltype(&Me_Type::_name)>::invoke; \
            args_types = {__VA_ARGS__}; \
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
            argsCount = args; \
            ret_type = _type; \
            address = (void *)&Me_Type::_name; \
            invoker_address = (void *)&BNM::NEW_CLASSES::GetNewStaticMethodCalls<decltype(&Me_Type::_name)>::invoke; \
            args_types = {__VA_ARGS__}; \
            Name = OBFUSCATE_BNM(#_name); \
            isStatic = true; \
            BNMClass.AddNewMethod(this); \
        } \
    }; \
    public: \
    static inline _BNMStaticMethod_##_name BNMStaticMethod_##_name = _BNMStaticMethod_##_name()

#define BNM_NewFieldInit(_name, _type) \
    private: \
    struct _BNMField_##_name : BNM::NEW_CLASSES::NewField { \
        _BNMField_##_name() { \
            Name = OBFUSCATE_BNM(#_name); \
            offset = offsetof(Me_Type, _name); \
            attributes = 0x0006;      \
            type = _type; \
            BNMClass.AddNewField(this, false); \
        } \
    }; \
    public: \
    static inline _BNMField_##_name BNMField_##_name = _BNMField_##_name()

#define BNM_NewStaticFieldInit(_name, _type, cppType) \
    private: \
    struct _BNMStaticField_##_name : BNM::NEW_CLASSES::NewField { \
        _BNMStaticField_##_name() { \
            Name = OBFUSCATE_BNM(#_name); \
            size = sizeof(cppType); \
            cppOffset = (size_t)&(_name); \
            attributes = 0x0006 | 0x0010; \
            type = _type; \
            BNMClass.AddNewField(this, true); \
        } \
    }; \
    public: \
    static inline _BNMStaticField_##_name BNMStaticField_##_name = _BNMStaticField_##_name()

#define BNM_NewDotCtorInit(_name, args, ...) \
    private: \
    struct _BNMMethod_##_name : BNM::NEW_CLASSES::NewMethod { \
        _BNMMethod_##_name() { \
            argsCount = args; \
            ret_type = BNM::GetType<void>(); \
            address = (void *)&BNM::NEW_CLASSES::GetNewMethodCalls<decltype(&Me_Type::_name)>::get<&Me_Type::_name>; \
            invoker_address = (void *)&BNM::NEW_CLASSES::GetNewMethodCalls<decltype(&Me_Type::_name)>::invoke; \
            args_types = {__VA_ARGS__}; \
            Name = OBFUSCATE_BNM(".ctor"); \
            BNMClass.AddNewMethod(this); \
        } \
    }; \
    public: \
    static inline _BNMMethod_##_name BNMMethod_##_name = _BNMMethod_##_name()

// Add class to exist or to new dll. Write dll name without '.dll'!
#define BNM_NewClassWithDllInit(dll, _namespace, name, base_namespace, base_name, ...)\
    private: \
    struct _BNMClass : BNM::NEW_CLASSES::NewClass { \
        _BNMClass() { \
            Name = OBFUSCATE_BNM(#name); \
            myNamespace = OBFUSCATE_BNM(_namespace); \
            BaseName = OBFUSCATE_BNM(base_name); \
            BaseNamespace = OBFUSCATE_BNM(base_namespace); \
            DllName = OBFUSCATE_BNM(dll); \
            Interfaces = {__VA_ARGS__}; \
            this->size = sizeof(name); \
            BNM::NEW_CLASSES::AddNewClass(this); \
        } \
    }; \
    public: \
    static inline _BNMClass BNMClass = _BNMClass(); \
    using Me_Type = name
#endif