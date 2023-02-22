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
#include <jni.h>
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
    typedef std::vector<IL2CPP::Il2CppClass *> ClassVector;

    namespace UNITY_STRUCTS {
#include "BNM_data/BasicStructs.h"
    }
    // Проверить, действителен ли указатель
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
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    T CheckObj(T obj) {
        if (obj && isAllocated(obj)) return obj;
        return nullptr;
    }
    namespace UnityEngine {
        // Должен быть использован для новых классов, если родитель: UnityEngine.Object, MonoBehaviour, ScriptableObject
        // Для System.Object нужно использовать BNM::IL2CPP::Il2CppObject
        struct Object : public BNM::IL2CPP::Il2CppObject {
            BNM_INT_PTR m_CachedPtr = 0;
            bool Alive() { return BNM::CheckObj((void *)this) && (BNM_PTR)m_CachedPtr; }
        };
    }
    char *str2char(const std::string &str);
	// Только если объект - дочерний элемент класса UnityEngine.Object или объект - это UnityEngine.Object
    [[maybe_unused]] auto IsUnityObjectAlive = [](auto o) {
        return ((UnityEngine::Object *)o)->Alive();
    };
	// Только если объект - дочерний элемент класса UnityEngine.Object или объект - это UnityEngine.Object
    [[maybe_unused]] auto IsSameUnityObject = [](auto o1, auto o2) {
        auto obj1 = (UnityEngine::Object *)o1;
        auto obj2 = (UnityEngine::Object *)o2;
        return (!obj1->Alive() && !obj2->Alive()) || (obj1->Alive() && obj2->Alive() && obj1->m_CachedPtr == obj2->m_CachedPtr);
    };

    auto InitFunc = [](auto&& method, auto ptr) {
        if (ptr != 0) *(void **)(&method) = (void *)(ptr);
    };
	
	// Обычные классы C# (строка, массив, список) (string, [], List)
    namespace MONO_STRUCTS {
        struct monoString : BNM::IL2CPP::Il2CppObject {
            int length;
            IL2CPP::Il2CppChar chars[0];
            std::string get_string();
            const char *get_const_char();
            const char *c_str();
            std::string str();
            [[maybe_unused]] std::string strO();
            std::string get_string_old();
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
        struct monoList : BNM::IL2CPP::Il2CppObject {
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
                if (items->capacity > 0) // Не копировать, если массив пустой
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

    // Просто определение структур для loadClass
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
        [[maybe_unused]] LoadClass(const IL2CPP::Il2CppType *type); // Из типа
        [[maybe_unused]] LoadClass(const MonoType *type); // Из C#-типа
        [[maybe_unused]] LoadClass(RuntimeTypeGetter type); // Из BNM-типа 

        LoadClass(const std::string &namespaze, const std::string &name); // Из имени класса
        LoadClass(const std::string &namespaze, const std::string &name, const std::string &dllName); // Из имён класса и dll

        [[maybe_unused]] std::vector<LoadClass> GetInnerClasses(bool includeParent = true) const; // Получить все вложенные классы
        [[maybe_unused]] std::vector<IL2CPP::FieldInfo *> GetFieldsInfo(bool includeParent = true) const; // Получить все поля
        [[maybe_unused]] std::vector<IL2CPP::MethodInfo *> GetMethodsInfo(bool includeParent = true) const; // Получить все методы

        Method<void> GetMethodByName(const std::string &name, int parameters = -1) const; // Получить метод по имени и кол-ву аргументов
        Method<void> GetMethodByName(const std::string &name, const std::vector<std::string> &parametersName) const; // Получить метод по имени и именам аргументов
        Method<void> GetMethodByName(const std::string &name, const std::vector<RuntimeTypeGetter> &parametersType) const; // Получить метод по имени и типам аргументов
        [[maybe_unused]] Property<bool> GetPropertyByName(const std::string &name, bool warning = false); // Получить свойство по имени
        [[maybe_unused]] LoadClass GetInnerClass(const std::string &name) const; // Получить вложенный класс по имени
        Field<int> GetFieldByName(const std::string &name) const; // Получить поле по имени

        [[maybe_unused]] LoadClass GetArrayClass() const; // В класс массива (класс[])

        IL2CPP::Il2CppType *GetIl2CppType() const; // В il2cpp-тип

        [[maybe_unused]] MonoType *GetMonoType() const; // В C#-тип

        IL2CPP::Il2CppClass *GetIl2CppClass() const; // В il2cpp-класс

        // Быстрые операторы
        inline operator IL2CPP::Il2CppType *() const { return GetIl2CppType(); };
        inline operator MonoType *() const { return GetMonoType(); };

        void *CreateNewInstance() const; // То же самое, что и new Object() в C#, но без вызова конструктора (.ctor)

        [[maybe_unused]] std::string str() const; // Получить информацию о классе

        // То же самое, что и new Object[] в C#
        template<typename T>
        MONO_STRUCTS::monoArray<T> *NewArray(IL2CPP::il2cpp_array_size_t length = 0) {
            if (!klass) return nullptr;
            TryInit();
            return (BNM::MONO_STRUCTS::monoArray<T> *) ArrayNew(klass, length);
        }

        // То же самое, что и List<Object>() в C#
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

        // Упоковать объект
        template<typename T>
        [[maybe_unused]] IL2CPP::Il2CppObject *BoxObject(T obj) {
            if (!klass) return nullptr;
            TryInit();
            return ObjBox(klass, (void *) obj);
        }

        // То же самое, что и new Object() в C# с вызовом .ctor по кол-ву аргументов
        template<typename ...Args>
        [[maybe_unused]] void *CreateNewObject(Args ...args) { return CreateNewObjectCtor(sizeof...(Args), {}, args...); }

        // То же самое, что и new Object() в C# с вызовом .ctor по кол-ву аргументов или их именам
        template<typename ...Args>
        void *CreateNewObjectCtor(int args_count, const std::vector<std::string> &arg_names, Args ...args);

        // Проверка, жив ли LoadClass
        inline operator bool() const { return klass != nullptr; };
    private: // Приватно) Просто вызовы il2cpp-методов
        void TryInit() const;
        static IL2CPP::Il2CppObject *ObjBox(IL2CPP::Il2CppClass*, void *);
        static IL2CPP::Il2CppObject *ObjNew(IL2CPP::Il2CppClass *);
        static IL2CPP::Il2CppArray *ArrayNew(IL2CPP::Il2CppClass*, IL2CPP::il2cpp_array_size_t);
    };
    
    // Для статических полей потоков
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

        // Скопировать другое поле, только для автоматического приведения типов
        template<typename otherT>
        [[maybe_unused]] Field(Field<otherT> f) : Field(f.myInfo) {
            if (f.Initialized() && !f.isStatic && !f.thread_static && BNM::CheckObj(f.instance))
                setInstance(f.instance);
        }

        // Из информации
        Field(IL2CPP::FieldInfo *info) {
            init = BNM::CheckObj(info);
            if (init) {
                isStatic = CheckIsStatic(info);
                myInfo = info;
                thread_static = myInfo->offset == -1;
            }
        }

        // Получить адрес (смещение)
        BNM_PTR GetOffset() {
            if (!init) return 0;
            return myInfo->offset;
        }

        // Получить указатель на поле
        T *getPointer() {
            if (!init) return makeSafeRet();
            if (!isStatic && !CheckObj(instance)) {
                LOGEBNM(OBFUSCATE_BNM("Не могу получить нестатический указатель поля %s без объекта! Пожалуйста, установите объект перед попыткой получить указатель."), str().c_str());
                return makeSafeRet();
            } else if (isStatic && !CheckObj(myInfo->parent)) {
                LOGEBNM(OBFUSCATE_BNM("Что-то пошло не так, статическое поле %s не имеет класс."), str().c_str());
                return makeSafeRet();
            } else if (thread_static) {
                LOGEBNM(OBFUSCATE_BNM("Получение указателя на статические поля потоков не поддерживается, поле: %s"), str().c_str());
                return makeSafeRet();
            }
            if (isStatic) return (T *) ((BNM_PTR) myInfo->parent->static_fields + myInfo->offset);
            return (T *) ((BNM_PTR) instance + myInfo->offset);
        }

        // Получить значение из поля
        T get() {
            if (!init) return {};
            if (thread_static) {
                T val{};
                PRIVATE_FILED_UTILS::GetStaticValue(myInfo, (void *)&val);
                return val;
            }
            return *getPointer();
        }
        operator T() { return get(); }
        T operator()() { return get(); }

        // Изменить значение поля
        void set(T val) {
            if (!init) return;
            if (thread_static) {
                PRIVATE_FILED_UTILS::SetStaticValue(myInfo, (void *)&val);
                return;
            }
            *getPointer() = val;
        }
        Field<T> &operator=(T val) {
            set(val);
            return *this;
        }

        // Установить объект
        Field<T> &setInstance(IL2CPP::Il2CppObject *val, bool doWarn = true) {
            if (init && isStatic) {
                if (doWarn) LOGWBNM(OBFUSCATE_BNM("Попытка установить объект статическому полю %s. Пожалуйста, уберите вызов setInstance в коде."), str().c_str());
                return *this;
            }
            init = val && myInfo != nullptr;
            instance = val;
            return *this;
        }

        // Быстрая установка объекта
        Field<T> &operator[](void *val) { return setInstance((IL2CPP::Il2CppObject *)val); }
        Field<T> &operator[](IL2CPP::Il2CppObject *val) { return setInstance((IL2CPP::Il2CppObject *)val); }
        Field<T> &operator[](UnityEngine::Object *val) { return setInstance((IL2CPP::Il2CppObject *)val); }

        // Копировать поле
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

        // Изменить тип поля
        template<typename NewT>
        Field<NewT> cast() { return Field<NewT>(myInfo).setInstance(instance, false); }

        // Получить данные о поле
        std::string str() {
            if (init) return LoadClass(myInfo->parent).str() + OBFUSCATE_BNM(".(") + myInfo->name + OBFUSCATE_BNM(")");
            return OBFUSCATE_BNM("Мёртвое поле");
        }

        // Проверить, живо ли поле
        bool Initialized() noexcept { return init; }
    private:
        static bool CheckIsStatic(IL2CPP::FieldInfo *field) {
            if (!field || !field->type) return false;
            return (field->type->attrs & 0x0010) != 0 && field->offset != -1 && (field->type->attrs & 0x0040) == 0;
        }
        [[maybe_unused]] T *makeSafeRet() { T ret{}; return &ret; }
    };

    // Преобразует адрес (смещение) в памяти в адрес (смещение) в библиотеке (работает для любой библиотеки)
    void *offsetInLib(void *offsetInMemory);

    template<typename Ret = void>
    struct Method {
        IL2CPP::MethodInfo *myInfo{};
        IL2CPP::Il2CppObject *instance{};
        bool init = false, isStatic = false, isVirtual = false;

        Method() noexcept = default;

        // Скопировать другой метод, только для автоматического приведения типов
        template<typename T = void>
        Method(Method<T> m) : Method(m.myInfo) {
            if (m.Initialized() && !m.isStatic && BNM::CheckObj(m.instance))
                setInstance(m.instance);
        }

        // Из информации
        Method(const IL2CPP::MethodInfo *info) {
            init = BNM::CheckObj(info);
            if (init) {
                isStatic = info->flags & 0x0010;
                isVirtual = info->slot != IL2CPP::kInvalidIl2CppMethodSlot;
                myInfo = (decltype(myInfo)) info;
            }
        }

        // Установить объект
        Method<Ret> &setInstance(IL2CPP::Il2CppObject *val, bool doWarn = true) {
            if (!init) return *this;
            if (init && isStatic) {
                if (doWarn)
                    LOGWBNM(OBFUSCATE_BNM("Попытка установить объект статическому методу %s. Пожалуйста, уберите вызов setInstance в коде."), str().c_str());
                return *this;
            }
            instance = val;
            return *this;
        }

        // Быстрая установка объекта
        inline Method<Ret> &operator[](void *val) { return setInstance((IL2CPP::Il2CppObject *)val); }
        inline Method<Ret> &operator[](IL2CPP::Il2CppObject *val) { return setInstance((IL2CPP::Il2CppObject *)val); }
        inline Method<Ret> &operator[](UnityEngine::Object *val) { return setInstance((IL2CPP::Il2CppObject *)val); }

        // Вызвать метод
        template<typename ...Args>
        Ret call(Args...args) {
            if (!init) return SafeReturn<Ret>();
            bool canInfo = true;
            if (sizeof...(Args) != myInfo->parameters_count){
                canInfo = false;
                LOGWBNM(OBFUSCATE_BNM("Попытка вызвать %s с неправильным кол-вом аргументов... Я надеюсь, вы знаете, что делаете. BNM не может добавить MethodInfo к аргументам :(. Пожалуйста, исправьте это."), str().c_str());
            }
            if (!isStatic && !CheckObj(instance)) {
                LOGEBNM(OBFUSCATE_BNM("Нельзя вызвать нестатический метод %s без объекта! Пожалуйста, установите объект перед вызовом метода."), str().c_str());
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

        // Быстро вызвать метод
        template<typename ...Args>
        inline Ret operator ()(Args ...args) { return call(args...); }

        // Получить данные
        std::string str() {
#if UNITY_VER > 174
#define kls klass
#else
#define kls declaring_type
#endif
            if (init) {
                return LoadClass(myInfo->return_type).str() + OBFUSCATE_BNM(" ") +
                       LoadClass(myInfo->kls).str() + OBFUSCATE_BNM(".(") +
                       myInfo->name + OBFUSCATE_BNM("){кол-во аргументов: ") +
                       std::to_string(myInfo->parameters_count) + OBFUSCATE_BNM("}") +
                       (isStatic ? OBFUSCATE_BNM("(статический)") : OBFUSCATE_BNM(""));
            }
            return OBFUSCATE_BNM("Мёртвый метод");
#undef kls
        }

        // Получить информацию
        IL2CPP::MethodInfo *GetInfo() {
            if (init) return myInfo;
            return {};
        }

        // Получить адрес (смещение)
        BNM_PTR GetOffset() {
            if (init) return (BNM_PTR) myInfo->methodPointer;
            return {};
        }

        // Проверить, жив ли метод
        bool Initialized() noexcept { return init; }
        operator bool() noexcept { return Initialized(); }

        // Изменить тип метода
        template<typename NewRet>
        [[maybe_unused]] Method<NewRet> cast() {
            return Method<NewRet>(myInfo).setInstance(instance, false);
        }

        // Копировать метод
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
    private: // Костыль)
        template<typename T>static T SafeReturn(){return{};}
        template<>static void SafeReturn(){}
    };


    template<typename T = bool>
    struct Property {
        // Так проще, чем искать PropertyInfo
        Method<T> getter;
        Method<void> setter;

        Property() noexcept = default;

		// Для получателя и установщика, можно использовать методы не из свойства
        template<typename V>
        Property(Method<V> getter, Method<void> setter) {
            this->getter = getter;
            this->setter = setter;
        }

        // Установить объект
        Property<T> &setInstance(IL2CPP::Il2CppObject *val, bool doWarn = true) {
            getter.setInstance(val, doWarn);
            setter.setInstance(val, doWarn);
            return *this;
        }

        // Быстрая установка объекта
        inline Property<T> &operator[](void *val) { return setInstance((IL2CPP::Il2CppObject *)val); }
        inline Property<T> &operator[](IL2CPP::Il2CppObject *val) { return setInstance((IL2CPP::Il2CppObject *)val); }
        inline Property<T> &operator[](UnityEngine::Object *val) { return setInstance((IL2CPP::Il2CppObject *)val); }

        // Вызвать получатель
        T get() { return getter(); }
        operator T() { return get(); }
        T operator()() { return get(); }

        // Вызвать установщик
        void set(T v) { return setter(v); }
        Property<T> &operator=(T val) {
            set(val);
            return *this;
        }

        // Копировать свойство
        template<typename V>
        Property<T> &operator=(Property<V> val) {
            getter = val.getter;
            setter = val.setter;
            return *this;
        }

        // Проверить, живо ли свойство
        [[maybe_unused]] bool Initialized() noexcept { return getter.init || setter.init; }

        // Изменить тип свойста
        template<typename NewRet>
        [[maybe_unused]] Property<NewRet> cast() {
            return Property<NewRet>(Method<NewRet>(getter), setter).setInstance(getter.instance ? getter.instance : setter.instance, false);
        }
    };

    // Код CreateNewObjectCtor
    template<typename ...Args>
    void *LoadClass::CreateNewObjectCtor(int args_count, const std::vector<std::string> &arg_names, Args ...args) {
        if (!klass) return nullptr;
        TryInit();
        auto method = arg_names.empty() ? GetMethodByName(OBFUSCATES_BNM(".ctor"), args_count) : GetMethodByName(OBFUSCATES_BNM(".ctor"), arg_names);
        auto instance = CreateNewInstance();
        method[instance](args...);
        return instance;
    }

    // Обычный C#-словарь (Dictionary)
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

	// Структура для сохранения данных для их поиска во время выполнения кода
    struct RuntimeTypeGetter {
        const char *namespaze{}, *name{};
        bool isArray = false;
        LoadClass loadedClass;
        LoadClass ToLC();
        IL2CPP::Il2CppType *ToIl2CppType();
        IL2CPP::Il2CppClass *ToIl2CppClass();
        operator IL2CPP::Il2CppType*();
        operator IL2CPP::Il2CppClass*();
        operator LoadClass();
    };

    // TODO: Реализовать что-то вроде GetType для этой структуры
    struct RuntimeMethodGetter {
        RuntimeTypeGetter type{};
        const char* name{};
        int args = -1;
        std::vector<std::string> argNames{};
        std::vector<RuntimeTypeGetter> argTypes{};
        Method<void> loadedMethod;
        Method<void> ToMethod();
        operator Method<void>();
    };

#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
    // Структуры для создания новых классов
    namespace NEW_CLASSES {

        // Данные о новых методах
        struct NewMethod {
            NewMethod() noexcept;
            IL2CPP::MethodInfo *thisMethod{};
            uint8_t argsCount = 0;
            void *address{}, *invoker{};
            const char *name{};
            RuntimeMethodGetter virtualMethod{}; // TODO: Нужно реализовать
            RuntimeTypeGetter retType{};
            std::vector<RuntimeTypeGetter> argTypes{};
            bool isStatic = false;
        };

        // Данные о новых полях
        struct NewField {
            NewField() noexcept;
            const char *name{};
            int offset{}, size{};
            unsigned int attributes : 16{};
            RuntimeTypeGetter type{};
            BNM_PTR cppOffset{};
            bool isStatic = false;
        };

        // Данные о новых классах
        struct NewClass {
            NewClass() noexcept;
            size_t size{};
            IL2CPP::Il2CppClass *thisClass{};
            MonoType *type{};
            const char *namespaze{}, *name{}, *baseNamespace{}, *baseName{}, *dllName{};
            std::vector<NewMethod *> methods4Add{};
            std::vector<NewField *> fields4Add{};
            std::vector<RuntimeTypeGetter> interfaces{};
            int staticFieldOffset = 0x0;
            BNM_PTR staticFieldsAddress{};
            void AddNewField(NewField *field, bool isStatic = false);
            void AddNewMethod(NewMethod *method);
        };

        // Добавить новый класс в список
        void AddNewClass(NEW_CLASSES::NewClass *klass);

        // Распаковать аргумент
        template<typename Q>
        static inline Q UnpackArg(void *arg) {
            if constexpr (std::is_pointer_v<Q>) return (Q)arg;
            else return *(Q *)arg;
        }


        template<typename> struct GetNewMethodCalls {};
        template<typename> struct GetNewStaticMethodCalls {};

        // Класс для создания инициатора вызовов для методов
        template<typename RetT, typename T, typename ...ArgsT>
        struct GetNewMethodCalls<RetT(T:: *)(ArgsT...)> {
            template<std::size_t ...As>
            static void *InvokeMethod(RetT(*func)(T*, ArgsT...), T *instance, void **args, std::index_sequence<As...>) {
                if constexpr (std::is_same_v<RetT, void>) {
                    func(instance, UnpackArg<ArgsT>(args[As])...);
                    return nullptr;
                } else return func(instance, UnpackArg<ArgsT>(args[As])...);
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
        // Класс для создания инициатора вызовов для статических методов
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

    // Сохранить название типа во время компиляции
    [[maybe_unused]] constexpr RuntimeTypeGetter GetType(const char *namespaze, const char *name, bool isArray = false) noexcept { return RuntimeTypeGetter{.namespaze = namespaze, .name = name, .isArray = isArray, .loadedClass = {}}; }

    // Метод создания обычных C#-строк
    MONO_STRUCTS::monoString *CreateMonoString(const char *str);
    [[maybe_unused]] MONO_STRUCTS::monoString *CreateMonoString(const std::string &str);

    // Получить внешние методы (icall)
    void *getExternMethod(const std::string &str);

    // Истина (true), когда il2cpp и BNM загружены
    bool Il2cppLoaded();

    // Вызов после загрузки il2cpp и BNM
    void SetIl2CppLoadEvent(void (*event)());

    // Утилиты потоков il2cpp
    [[maybe_unused]] bool AttachIl2Cpp();
    [[maybe_unused]] IL2CPP::Il2CppThread *CurrentIl2CppThread();
    [[maybe_unused]] void DetachIl2Cpp();

    // Получить путь к libil2cpp.so
    [[maybe_unused]] std::string GetLibIl2CppPath();

    // Получить адрес (смещение) загруженного libil2cpp.so
    [[maybe_unused]] BNM_PTR GetLibIl2CppOffset();

    // Не закрывать его! BNM вызовет сбой из-за этого.
    [[maybe_unused]] void *GetLibIl2CppDlInst();

    // Сохранить имя базовых типов во время компиляции
    template<typename T>
    constexpr RuntimeTypeGetter GetType(bool isArray = false) noexcept {
        if (std::is_same<T, void>::value)
            return {.namespaze = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Void"), .isArray = isArray};
        else if (std::is_same<T, bool>::value)
            return {.namespaze = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Boolean"), .isArray = isArray};
        else if (std::is_same<T, uint8_t>::value || std::is_same<T, unsigned char>::value)
            return {.namespaze = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Byte"), .isArray = isArray};
        else if (std::is_same<T, int8_t>::value)
            return {.namespaze = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("SByte"), .isArray = isArray};
        else if (std::is_same<T, int16_t>::value)
            return {.namespaze = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Int16"), .isArray = isArray};
        else if (std::is_same<T, uint16_t>::value)
            return {.namespaze = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("UInt16"), .isArray = isArray};
        else if (std::is_same<T, int32_t>::value)
            return {.namespaze = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Int32"), .isArray = isArray};
        else if (std::is_same<T, uint32_t>::value)
            return {.namespaze = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("UInt32"), .isArray = isArray};
        else if (std::is_same<T, intptr_t>::value)
            return {.namespaze = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("IntPtr"), .isArray = isArray};
        else if (std::is_same<T, int64_t>::value)
            return {.namespaze = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Int64"), .isArray = isArray};
        else if (std::is_same<T, uint64_t>::value)
            return {.namespaze = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("UInt64"), .isArray = isArray};
        else if (std::is_same<T, float>::value)
            return {.namespaze = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Single"), .isArray = isArray};
        else if (std::is_same<T, double>::value)
            return {.namespaze = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Double"), .isArray = isArray};
        else if (std::is_same<T, BNM::IL2CPP::Il2CppString *>::value || std::is_same<T, BNM::MONO_STRUCTS::monoString *>::value)
            return {.namespaze = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("String"), .isArray = isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::Vector3>::value)
            return {.namespaze = OBFUSCATE_BNM("UnityEngine"), .name = OBFUSCATE_BNM("Vector3"), .isArray = isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::Vector2>::value)
            return {.namespaze = OBFUSCATE_BNM("UnityEngine"), .name = OBFUSCATE_BNM("Vector2"), .isArray = isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::Color>::value)
            return {.namespaze = OBFUSCATE_BNM("UnityEngine"), .name = OBFUSCATE_BNM("Color"), .isArray = isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::Ray>::value)
            return {.namespaze = OBFUSCATE_BNM("UnityEngine"), .name = OBFUSCATE_BNM("Ray"), .isArray = isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::RaycastHit>::value)
            return {.namespaze = OBFUSCATE_BNM("UnityEngine"), .name = OBFUSCATE_BNM("RaycastHit"), .isArray = isArray};
        else if (std::is_same<T, BNM::UNITY_STRUCTS::Quaternion>::value)
            return {.namespaze = OBFUSCATE_BNM("UnityEngine"), .name = OBFUSCATE_BNM("Quaternion"), .isArray = isArray};
        else return {.namespaze = OBFUSCATE_BNM("System"), .name = OBFUSCATE_BNM("Object"), .isArray = isArray};
    }

    // Распаковка объекта, просто копия метода из il2cpp
    template<typename T>
    [[maybe_unused]] static T UnboxObject(T obj) { return (T)(void *)(((char *)obj) + sizeof(BNM::IL2CPP::Il2CppObject)); }

    // Подмена метода путем изменения MethodInfo
    void InvokeHook(BNM::Method<int> m, void *newMet, void **oldMet);
    void InvokeHook(IL2CPP::MethodInfo *m, void *newMet, void **oldMet);

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


    // Попробовать обойти любую защиту, получив полный путь до библиотеки (ТОЛЬКО ВНУТРЕННЕЕ ИСПОЛЬЗОВАНИЕ)
    [[maybe_unused]] void HardBypass(JNIEnv *env);
    namespace External {
		// Попробовать загрузить BNM, если вы используете BNM извне
		// Нужно вызвать это из любого потока unity
		// dl - дескриптор загруженной libil2cpp.so
        [[maybe_unused]] void LoadBNM(void *dl);
		// Установить дескриптор libil2cpp.so, не проверяя его, и попробовать загрузить BNM
		// GetLibIl2CppOffset и GetLibIl2CppPath будут пустыми
		// Нужно вызвать это из любого потока unity
		// dl - дескриптор загруженной libil2cpp.so
        [[maybe_unused]] void ForceLoadBNM(void *dl);
    }
}
#define InitResolveFunc(x, y) BNM::InitFunc(x, BNM::getExternMethod(y))

#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
#define BNM_NewClassInit(_namespace, _name, _baseNamespace, _baseName, ...) BNM_NewClassWithDllInit("Assembly-CSharp", _namespace, _name, _baseNamespace, _baseName, __VA_ARGS__)

#define BNM_NewMethodInit(_retType, _name, _argsCount, ...) \
    private: \
    struct _BNMMethod_##_name : BNM::NEW_CLASSES::NewMethod { \
        _BNMMethod_##_name() { \
            argsCount = _argsCount; \
            retType = _retType; \
            auto p = &NewBNMType::_name; address = *(void **)&p; \
            invoker = (void *)&BNM::NEW_CLASSES::GetNewMethodCalls<decltype(&NewBNMType::_name)>::invoke; \
            argTypes = {__VA_ARGS__}; \
            name = OBFUSCATE_BNM(#_name); \
            BNMClass.AddNewMethod(this); \
        } \
    }; \
    public: \
    static inline _BNMMethod_##_name BNMMethod_##_name = _BNMMethod_##_name()

#define BNM_NewStaticMethodInit(_retType, _name, _argsCount, ...) \
    private: \
    struct _BNMStaticMethod_##_name : BNM::NEW_CLASSES::NewMethod { \
        _BNMStaticMethod_##_name() { \
            argsCount = _argsCount; \
            retType = _retType; \
            address = (void *)&NewBNMType::_name; \
            invoker = (void *)&BNM::NEW_CLASSES::GetNewStaticMethodCalls<decltype(&NewBNMType::_name)>::invoke; \
            argTypes = {__VA_ARGS__}; \
            name = OBFUSCATE_BNM(#_name); \
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
            name = OBFUSCATE_BNM(#_name); \
            offset = offsetof(NewBNMType, _name); \
            attributes = 0x0006; \
            type = _type; \
            BNMClass.AddNewField(this, false); \
        } \
    }; \
    public: \
    static inline _BNMField_##_name BNMField_##_name = _BNMField_##_name()

#define BNM_NewStaticFieldInit(_name, _type, _cppType) \
    private: \
    struct _BNMStaticField_##_name : BNM::NEW_CLASSES::NewField { \
        _BNMStaticField_##_name() { \
            name = OBFUSCATE_BNM(#_name); \
            size = sizeof(_cppType); \
            cppOffset = (size_t)&(_name); \
            attributes = 0x0006 | 0x0010; \
            type = _type; \
            BNMClass.AddNewField(this, true); \
        } \
    }; \
    public: \
    static inline _BNMStaticField_##_name BNMStaticField_##_name = _BNMStaticField_##_name()

#define BNM_NewDotCtorInit(_name, _argsCount, ...) \
    private: \
    struct _BNMMethod_##_name : BNM::NEW_CLASSES::NewMethod { \
        _BNMMethod_##_name() { \
            argsCount = _argsCount; \
            retType = BNM::GetType<void>(); \
            auto p = &NewBNMType::_name; address = *(void **)&p; \
            invoker = (void *)&BNM::NEW_CLASSES::GetNewMethodCalls<decltype(&NewBNMType::_name)>::invoke; \
            argTypes = {__VA_ARGS__}; \
            name = OBFUSCATE_BNM(".ctor"); \
            BNMClass.AddNewMethod(this); \
        } \
    }; \
    public: \
    static inline _BNMMethod_##_name BNMMethod_##_name = _BNMMethod_##_name()

// Добавьте класс в существующую или в новую библиотеку dll. Имя библиотеки dll без '.dll'!
#define BNM_NewClassWithDllInit(_dllName, _namespace, _name, _baseNamespace, _baseName, ...)\
    private: \
    struct _BNMClass : BNM::NEW_CLASSES::NewClass { \
        _BNMClass() { \
            name = OBFUSCATE_BNM(#_name); \
            namespaze = OBFUSCATE_BNM(_namespace); \
            baseName = OBFUSCATE_BNM(_baseName); \
            baseNamespace = OBFUSCATE_BNM(_baseNamespace); \
            dllName = OBFUSCATE_BNM(_dllName); \
            interfaces = {__VA_ARGS__}; \
            size = sizeof(_name); \
            BNM::NEW_CLASSES::AddNewClass(this); \
        } \
    }; \
    public: \
    static inline _BNMClass BNMClass = _BNMClass(); \
    using NewBNMType = _name
#endif