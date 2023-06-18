#include <list>
#include <locale>
#include <atomic>
#include <array>
#include <thread>
#ifdef BNM_DEPRECATED
#include <codecvt>
#endif
#include <shared_mutex>
#include <dlfcn.h>
#include "BNM.hpp"
#include "BNM_data/utf8.h"

#define DO_API(r, n, p) auto (n) = (r (*) p)BNM_dlsym(BNM_Internal::dlLib, OBFUSCATE_BNM(#n))
#if __cplusplus <= 201103L
namespace nonstd {
    struct shared_mutex {
        shared_mutex() : m_readerCount(0), m_writeInProgress(false) {}

        shared_mutex(const shared_mutex&) = delete;
        shared_mutex& operator=(const shared_mutex&) = delete;

        void lock() {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_writeCond.wait(lock, [this]() { return !m_writeInProgress; });
            m_writeInProgress = true;
            m_readCond.wait(lock, [this]() {
                return m_readerCount == 0;
            });
        }

        void lock_shared() {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_readCond.wait(lock, [this]() {
                return m_writeInProgress == false;
            });
            ++m_readerCount;
        }

        void unlock_shared() {
            std::lock_guard<std::mutex> lock(m_mutex);
            --m_readerCount;
            m_readCond.notify_all();
        }
    private:
        mutable std::mutex m_mutex;
        std::condition_variable m_writeCond;
        std::condition_variable m_readCond;
        size_t m_readerCount;
        bool m_writeInProgress;
    };
    struct shared_lock {
        explicit shared_lock(shared_mutex& lock) : m_lock(lock) { m_lock.lock_shared(); }
        ~shared_lock() { m_lock.unlock_shared(); }
    private:
        shared_mutex& m_lock;
    };
}
#endif
namespace BNM_Internal {
    using namespace BNM;

    // Внутренние переменные
    static bool LibLoaded = false;
    static void *dlLib{};
    static bool hardBypass = false;
    static const char *LibAbsolutePath{};
    static BNM_PTR LibAbsoluteAddress{};
    static bool HasImageGetCls = false;
#if __cplusplus >= 201402L
    using bnm_shared_mutex = std::shared_mutex;
    using bnm_shared_lock = std::shared_lock<bnm_shared_mutex>;
#else
    using bnm_shared_mutex = nonstd::shared_mutex;
    using bnm_shared_lock = nonstd::shared_lock;
#endif
    

    static bnm_shared_mutex findClasses{};
    static std::mutex addClass{}, modClass{};
    static std::list<void(*)()> onIl2CppLoaded{};

    std::list<void(*)()> &GetEvents() { return onIl2CppLoaded; }


    // Методы для новых классов и для обычного режима
    AssemblyVector *(*Assembly$$GetAllAssemblies)(){};
    void Empty() {}
    
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
    static std::shared_mutex classesFindAccess{};
    // Список со всеми новыми классами, которые BNM должен загрузить
    static std::vector<NEW_CLASSES::NewClass *> *classes4Add = nullptr;

    // Список со всеми классами, которые BNM должен изменить
    static std::vector<MODIFY_CLASSES::TargetClass *> *classes4Mod = nullptr;

    // Конструктор, который вызывает базовый конструктор (.ctor)
    void DefaultConstructor(IL2CPP::Il2CppObject* instance) {
        LoadClass(instance->klass->parent).GetMethodByName(OBFUSCATE_BNM(".ctor"), 0)[instance]();
    }
    void DefaultConstructorInvoke(IL2CPP::Il2CppMethodPointer, IL2CPP::MethodInfo*, IL2CPP::Il2CppObject *instance, void**) { DefaultConstructor(instance); }

    IL2CPP::Il2CppClass *(*old_Class$$FromIl2CppType)(IL2CPP::Il2CppType *type){};
    IL2CPP::Il2CppClass *Class$$FromIl2CppType(IL2CPP::Il2CppType *type);

    IL2CPP::Il2CppClass *(*old_Type$$GetClassOrElementClass)(IL2CPP::Il2CppType *type){};
    IL2CPP::Il2CppClass *Type$$GetClassOrElementClass(IL2CPP::Il2CppType *type);

    IL2CPP::Il2CppClass *(*old_Class$$FromName)(IL2CPP::Il2CppImage *image, const char *ns, const char *name){};
    IL2CPP::Il2CppClass *Class$$FromName(IL2CPP::Il2CppImage *image, const char *namespaze, const char *name);

    // Структура для Il2CppType, для классов, чтобы проверять, созданы ли они через BNM
    struct BNMTypeData {
        BNMTypeData() = default;
        BNM_PTR bnm = -0x424e4d;
        IL2CPP::Il2CppClass *cls{};
    };
    void ModifyClasses();
    void InitNewClasses();
#if UNITY_VER <= 174
    // Требуются, потому что в Unity 2017 и ниже в образах (image) и сборках (assembly) они хранятся по номерам

    IL2CPP::Il2CppImage *(*old_GetImageFromIndex)(IL2CPP::ImageIndex index);
    IL2CPP::Il2CppImage *new_GetImageFromIndex(IL2CPP::ImageIndex index);

    IL2CPP::Il2CppAssembly *Assembly$$Load(const char *name);
#endif

#endif
#ifdef BNM_IL2CPP_ZERO_PTR
    void *(*old_Object$$NewAllocSpecific)(IL2CPP::Il2CppClass *klass){};
    void *Object$$NewAllocSpecific(IL2CPP::Il2CppClass *klass) {
        auto obj = old_Object$$NewAllocSpecific(klass);
        if (obj) memset((char*)obj + sizeof(IL2CPP::Il2CppObject), 0, klass->instance_size - sizeof(IL2CPP::Il2CppObject));
        return obj;
    }
#endif
    void (*old_Image$$GetTypes)(IL2CPP::Il2CppImage *image, bool exportedOnly, ClassVector *target){};
    void Image$$GetTypes(IL2CPP::Il2CppImage *image, bool exportedOnly, ClassVector *target);

    void (*Class$$Init)(IL2CPP::Il2CppClass *klass){};

    void (*old_BNM_il2cpp_init)(const char *);
    void BNM_il2cpp_init(const char *domain_name);

    void InitIl2cppMethods();

#ifndef BNM_DISABLE_AUTO_LOAD
    [[maybe_unused]] __attribute__((constructor)) void PrepareBNM();
#endif

    bool InitDlLib(void *dl, const char *path = nullptr, bool external = false);

    // Методы для конвертации C#-строк (monoString)
    typedef std::basic_string<IL2CPP::Il2CppChar> string16;
    std::string Utf16ToUtf8(IL2CPP::Il2CppChar *utf16String, size_t length) {
        std::string utf8String;
        utf8String.reserve(length);
        utf8::unchecked::utf16to8(utf16String, utf16String + length, std::back_inserter(utf8String));
        return utf8String;
    }
    string16 Utf8ToUtf16(const char *utf8String, size_t length) {
        string16 utf16String;
        if (utf8::is_valid(utf8String, utf8String + length)) {
            utf16String.reserve(length);
            utf8::unchecked::utf8to16(utf8String, utf8String + length, std::back_inserter(utf16String));
        }
        return utf16String;
    }
}

// Безопасно конвертировать std::string в const char *
char *str2char(const std::string &str) {
    size_t size = str.end() - str.begin();
    if (str.c_str()[size]) {
        auto c = (char *)malloc(size + 1);
        memset(c, 0, size + 1);
        memcpy(c, str.data(), size);
        return c;
    }
    return (char *)str.c_str();
}

namespace BNM {
    // Получить/Установить внутренние данные
    bool Il2cppLoaded() { return BNM_Internal::LibLoaded; }

    namespace MONO_STRUCTS {
        void *CompareExchange4List(void *syncRoot) { // Единственный нормальный способ вызвать CompareExchange для syncRoot для monoList
            static std::once_flag once{};
            static BNM::Method<void *> CompareExchange{};
            static BNM::LoadClass objectType{};
            std::call_once(once, []() {
                objectType = BNM::GetType<void*>().ToLC();
                CompareExchange = BNM::LoadClass(OBFUSCATE_BNM("System.Threading"), OBFUSCATE_BNM("Interlocked")).GetMethodByName(OBFUSCATE_BNM("Exchange"), {objectType, objectType, objectType});
            });
            if (CompareExchange) CompareExchange((void **)&syncRoot, (void *)objectType.CreateNewInstance(), (void *)nullptr);
            return syncRoot;
        }
        std::string monoString::get_string() {
            monoString *me = this;
            if (!me) return OBFUSCATE_BNM("ОШИБКА: monoString мёртв");
            if (!IsAllocated(chars)) return OBFUSCATE_BNM("ОШИБКА: символы строки мертвы");
            if (!length) return OBFUSCATE_BNM("");
            return BNM_Internal::Utf16ToUtf8(chars, length);
        }
        std::string monoString::str() { return get_string(); }

        const char *monoString::get_const_char() { return str2char(get_string()); }
        const char *monoString::c_str() { return get_const_char(); }

#ifdef BNM_DEPRECATED
        std::string monoString::get_string_old() {
            if (!this) return OBFUSCATE_BNM("ОШИБКА: monoString мёртв");
            if (!IsAllocated(chars)) return OBFUSCATE_BNM("ОШИБКА: символы строки мертвы");
            if (!length) return OBFUSCATE_BNM("");
            return std::wstring_convert<std::codecvt_utf8<IL2CPP::Il2CppChar>, IL2CPP::Il2CppChar>().to_bytes((IL2CPP::Il2CppChar *)chars);
        }
        [[maybe_unused]] std::string monoString::strO() { return get_string_old(); }
#endif

        [[maybe_unused]] unsigned int monoString::getHash() {
            monoString *me = this;
            if (!me || !IsAllocated(chars)) return 0;
            IL2CPP::Il2CppChar *p = chars;
            unsigned int h = 0;
            for (int i = 0; i < length; ++i)
                h = (h << 5) - h + *p; p++;
            return h;
        }

        // Создать обычную C#-строку, как это делает il2cpp
        monoString *monoString::Create(const char *str) {
            const size_t length = strlen(str);
            const size_t utf16Size = sizeof(IL2CPP::Il2CppChar) * length;
            auto ret = (monoString *)malloc(sizeof(monoString) + utf16Size);
            ret->length = (int)length;
            auto u16 = BNM_Internal::Utf8ToUtf16(str, ret->length);
            memcpy(ret->chars, &u16[0], utf16Size);
            u16.clear();
            auto empty = Empty();
            if (empty) ret->klass = empty->klass;
            return (monoString *)ret;
        }
        [[maybe_unused]] monoString *monoString::Create(const std::string &str) { return Create(str2char(str)); }

        [[maybe_unused]] monoString *monoString::Empty() {
            static monoString** ret = LoadClass(OBFUSCATES_BNM("System"), OBFUSCATES_BNM("String")).GetFieldByName(OBFUSCATES_BNM("Empty")).cast<monoString *>().getPointer();
            return *ret;
        }
    }

    /*** LoadClass ***/
    LoadClass::LoadClass(const IL2CPP::Il2CppClass *clazz) { klass = (IL2CPP::Il2CppClass *)clazz; }

    LoadClass::LoadClass(const IL2CPP::Il2CppObject *obj) {
        if (!obj) return;
        klass = obj->klass;
    }

    [[maybe_unused]] LoadClass::LoadClass(const IL2CPP::Il2CppType *type) {
        if (!type) return;
        DO_API(IL2CPP::Il2CppClass *, il2cpp_class_from_il2cpp_type, (const IL2CPP::Il2CppType *));
        klass = il2cpp_class_from_il2cpp_type(type);
    }

    [[maybe_unused]] LoadClass::LoadClass(const MonoType *type) {
        if (!type) return;
        DO_API(IL2CPP::Il2CppClass *, il2cpp_class_from_il2cpp_type, (const IL2CPP::Il2CppType *));
        klass = il2cpp_class_from_il2cpp_type(type->type);
    }

    [[maybe_unused]] LoadClass::LoadClass(RuntimeTypeGetter type) { klass = type; }

    LoadClass::LoadClass(const std::string &namespaze, const std::string &name) {
        DO_API(IL2CPP::Il2CppImage *, il2cpp_assembly_get_image, (IL2CPP::Il2CppAssembly *));

        auto assemblies = BNM_Internal::Assembly$$GetAllAssemblies();

        for (auto assembly : *assemblies) {
            // Получить образ из сборки
            auto image = il2cpp_assembly_get_image(assembly);

            // Получить все классы образа
            ClassVector classes;
            BNM_Internal::Image$$GetTypes(image, false, &classes);

            for (auto cls : classes) {
                if (!cls) continue;

                // Попробовать инициализировать класс
                BNM_Internal::Class$$Init(cls);

                // Пропускать внутренние классы
                if (cls->declaringType) continue;

                // Проверить, нужный ли это класс
                if (cls->name == name && cls->namespaze == namespaze) {
                    klass = cls;
                    break;
                }
            }

            // Очистить список классов
            classes.clear(); classes.shrink_to_fit();

            if (klass) break;
        }
        if (!klass) LOGWBNM(OBFUSCATE_BNM("Класс: [%s]::[%s] - не найден."), namespaze.c_str(), name.c_str());
    }

    LoadClass::LoadClass(const std::string &namespaze, const std::string &name, const std::string &dllName) {
        DO_API(IL2CPP::Il2CppImage *, il2cpp_assembly_get_image, (const IL2CPP::Il2CppAssembly *));

        IL2CPP::Il2CppImage *image = nullptr;
        auto assemblies = BNM_Internal::Assembly$$GetAllAssemblies();

        // Попробовать найти образ
        for (auto assembly : *assemblies)
            if (dllName == il2cpp_assembly_get_image(assembly)->name) {
                image = il2cpp_assembly_get_image(assembly);
                break;
            }

        // Проверить, найден ли образ
        if (!image) {
            LOGWBNM(OBFUSCATE_BNM("Класс: [%s]::[%s]::[%s] - dll не найден."), dllName.c_str(), namespaze.c_str(), name.c_str());
            klass = nullptr;
            return;
        }

        // Получить все классы образа
        ClassVector classes;
        BNM_Internal::Image$$GetTypes(image, false, &classes);

        // Попробовать найти класс
        for (auto cls : classes) {
            if (!cls) continue;

            // Попробовать инициализировать класс
            BNM_Internal::Class$$Init(cls);

            // Пропускать внутренние классы
            if (cls->declaringType) continue;

            // Проверить, нужный ли это класс
            if (cls->name == name && cls->namespaze == namespaze) {
                klass = cls;
                break;
            }
        }

        // Очистить список классов
        classes.clear(); classes.shrink_to_fit();

        if (!klass) LOGWBNM(OBFUSCATE_BNM("Класс: [%s]::[%s]::[%s] - не найден."), dllName.c_str(), namespaze.c_str(), name.c_str());
    }

    [[maybe_unused]] std::vector<LoadClass> LoadClass::GetInnerClasses(bool includeParent) const {
        if (!klass) return {};
        TryInit(); // Попробовать инициализировать класс

        std::vector<LoadClass> ret;
        auto curClass = klass;

        do {
            // Добавить классы в список
            for (int i = 0; i < curClass->nested_type_count; ++i) ret.emplace_back(curClass->nestedTypes[i]);

            // Переключиться на родителя, если требуется
            if (includeParent) curClass = curClass->parent;
            else curClass = nullptr;

        } while (curClass);

        return ret;
    }

    [[maybe_unused]] std::vector<IL2CPP::FieldInfo *> LoadClass::GetFieldsInfo(bool includeParent) const {
        if (!klass) return {};
        TryInit(); // Попробовать инициализировать класс

        std::vector<IL2CPP::FieldInfo *> ret;
        auto curClass = klass;

        do {
            // Добавить поля в список
            for (int i = 0; i < curClass->field_count; ++i) ret.emplace_back(curClass->fields + i);

            // Переключиться на родителя, если требуется
            if (includeParent) curClass = curClass->parent;
            else curClass = nullptr;
        } while (curClass);

        return ret;
    }

    [[maybe_unused]] std::vector<IL2CPP::MethodInfo *> LoadClass::GetMethodsInfo(bool includeParent) const {
        if (!klass) return {};
        TryInit(); // Попробовать инициализировать класс

        std::vector<IL2CPP::MethodInfo *> ret;
        auto curClass = klass;

        do {
            // Добавить методы в список
            for (int i = 0; i < curClass->method_count; ++i) ret.emplace_back(Method<void>(curClass->methods[i]).GetInfo());

            // Переключиться на родителя, если требуется
            if (includeParent) curClass = curClass->parent;
            else curClass = nullptr;

        } while (curClass);

        return ret;
    }

    Method<void> LoadClass::GetMethodByName(const std::string &name, int parameters) const {
        if (!klass) return {};
        TryInit(); // Попробовать инициализировать класс

        Method<void> ret{};

        // Получить все методы
        auto methods = GetMethodsInfo(true);

        // Попробовать найти метод
        for (auto method : methods)
            if (name == method->name && (method->parameters_count == parameters || parameters == -1)) {
                ret = method;
                break;
            }

        // Очистить список методов
        methods.clear(); methods.shrink_to_fit();

        // Предупредить о ненахождении
        if (!ret) LOGWBNM(OBFUSCATE_BNM("Метод: [%s]::[%s].[%s], %d - не найден."), klass->namespaze, klass->name, name.c_str(), parameters);

        return ret;
    }

    Method<void> LoadClass::GetMethodByName(const std::string &name, const std::vector<std::string> &parametersName) const {
        if (!klass) return {};
        TryInit(); // Попробовать инициализировать класс

        DO_API(const char *, il2cpp_method_get_param_name, (IL2CPP::MethodInfo *method, uint32_t index));

        Method<void> ret{};

        // Получить все методы
        auto methods = GetMethodsInfo(true);

        size_t parameters = parametersName.size();

        // Попробовать найти метод
        for (auto method : methods)
            if (name == method->name && (method->parameters_count == parameters)) {
                bool ok = true;

                // Проверить аргументы
                for (int g = 0; g < parameters; ++g)
                    if (il2cpp_method_get_param_name(method, g) != parametersName[g]) {
                        // Имя не совпадает, останавливаем for
                        ok = false;
                        break;
                    }

                // Найдено, останавливаем for
                if (ok) {
                    ret = method;
                    break;
                }
            }

        // Очистить список методов
        methods.clear(); methods.shrink_to_fit();

        // Предупредить о ненахождении
        if (!ret) LOGWBNM(OBFUSCATE_BNM("Метод: [%s]::[%s].(%s), %d - не найден; использовались имена аргументов."), klass->namespaze, klass->name, name.c_str(), parameters);

        return ret;
    }

    Method<void> LoadClass::GetMethodByName(const std::string &name, const std::vector<RuntimeTypeGetter> &parametersType) const {
        if (!klass) return {};
        TryInit(); // Попробовать инициализировать класс

        DO_API(IL2CPP::Il2CppClass *, il2cpp_class_from_il2cpp_type, (const IL2CPP::Il2CppType *));

        Method<void> ret{};

        // Получить все методы
        auto methods = GetMethodsInfo(true);

        size_t parameters = parametersType.size();

        // Попробовать найти метод
        for (auto method : methods)
            if (name == method->name && (method->parameters_count == parameters)) {
                bool ok = true;

                // Проверить аргументы
                for (int g = 0; g < parameters; ++g) {
#if UNITY_VER < 212
                    auto param = method->parameters + g;
                    auto cls = il2cpp_class_from_il2cpp_type(param->parameter_type);
#else
                    auto param = method->parameters[g];
                    auto cls = il2cpp_class_from_il2cpp_type(param);
#endif
                    if (cls == ((RuntimeTypeGetter) parametersType[g]).ToIl2CppClass()) {
                        // Тип не совпадает, останавливаем for
                        ok = false;
                        break;
                    }
                }

                // Найдено, останавливаем for
                if (ok) {
                    ret = method;
                    break;
                }
            }

        // Очистить список методов
        methods.clear(); methods.shrink_to_fit();

        // Предупредить о ненахождении
        if (!ret) LOGWBNM(OBFUSCATE_BNM("Метод: [%s]::[%s].[%s], %d - не найден; использовались типы аргументов."), klass->namespaze, klass->name, name.c_str(), parameters);

        return ret;
    }

    [[maybe_unused]] Property<bool> LoadClass::GetPropertyByName(const std::string &name, bool warning) {
        if (!klass) return {};
        TryInit();
        auto get = GetMethodByName(OBFUSCATES_BNM("get_") + name);
        auto set = GetMethodByName(OBFUSCATES_BNM("set_") + name);
        if (!get && !set) {
            LOGWBNM(OBFUSCATE_BNM("Свойство %s.(%s) не найдено."), str().c_str(), name.c_str());
            return {};
        }
        if (!get && warning)
            LOGWBNM(OBFUSCATE_BNM("Свойство %s.(%s) без получателя."), str().c_str(), name.c_str());
        if (!set && warning)
            LOGWBNM(OBFUSCATE_BNM("Свойство %s.(%s) без установщика."), str().c_str(), name.c_str());
        return {get, set};
    }

    [[maybe_unused]] LoadClass LoadClass::GetInnerClass(const std::string &name) const {
        if (!klass) return {};
        TryInit(); // Попробовать инициализировать класс

        LoadClass ret{};

        // Получить все внутренние классы
        auto classes = GetInnerClasses(true);

        // Попробовать найти класс
        for (auto cls : classes)
            if (name == cls.klass->name) {
                ret = cls;
                break;
            }

        // Очистить список классов
        classes.clear(); classes.shrink_to_fit();

        // Предупредить о ненахождении
        if (!ret.klass) LOGWBNM(OBFUSCATE_BNM("Вложенный класс: [%s]::[%s]::[%s] - не найден."), klass->namespaze, klass->name, name.c_str());

        return ret;
    }

    Field<int> LoadClass::GetFieldByName(const std::string &name) const {
        if (!klass) return {};
        TryInit(); // Попробовать инициализировать класс

        Field<int> ret{};

        // Получить все поля
        auto fields = GetFieldsInfo(true);

        // Попробовать найти поле
        for (auto field : fields)
            if (name == field->name) {
                ret = field;
                break;
            }

        // Очистить список полей
        fields.clear(); fields.shrink_to_fit();

        // Предупредить о ненахождении
        if (!ret.Initialized()) LOGWBNM(OBFUSCATE_BNM("Поле: [%s]::[%s].(%s) - не найдено."), klass->namespaze, klass->name, name.c_str());

        return ret;
    }


    [[maybe_unused]] LoadClass LoadClass::GetParent() const {
        if (!klass) return {};
        TryInit(); // Попробовать инициализировать класс
        return klass->parent;
    }

    [[maybe_unused]] LoadClass LoadClass::GetArrayClass() const {
        if (!klass) return {};
        TryInit(); // Попробовать инициализировать класс

        DO_API(IL2CPP::Il2CppClass *, il2cpp_array_class_get, (IL2CPP::Il2CppClass *, uint32_t));

        return il2cpp_array_class_get(klass, 1);
    }

    IL2CPP::Il2CppType *LoadClass::GetIl2CppType() const {
        if (!klass) return nullptr;
        TryInit(); // Попробовать инициализировать класс

#if UNITY_VER > 174
        return (IL2CPP::Il2CppType *)&klass->byval_arg;
#else
        return (IL2CPP::Il2CppType *)klass->byval_arg;
#endif
    }

    [[maybe_unused]] MonoType *LoadClass::GetMonoType() const {
        if (!klass) return nullptr;
        TryInit(); // Попробовать инициализировать класс

        DO_API(IL2CPP::Il2CppObject *, il2cpp_type_get_object, (IL2CPP::Il2CppType *));

        return (MonoType *) il2cpp_type_get_object(GetIl2CppType());
    }

    IL2CPP::Il2CppClass *LoadClass::GetIl2CppClass() const {
        TryInit(); // Попробовать инициализировать класс
        return klass;
    }

    BNM::RuntimeTypeGetter LoadClass::GetRuntimeType() const {
        TryInit(); // Попробовать инициализировать класс
        return {nullptr, nullptr, false, {klass}};
    }

    LoadClass::operator BNM::RuntimeTypeGetter() const { return GetRuntimeType(); }

    void *LoadClass::CreateNewInstance() const {
        if (!klass) return nullptr;
        TryInit(); // Попробовать инициализировать класс

        if ((klass->flags & (0x00000080 | 0x00000020))) // TYPE_ATTRIBUTE_ABSTRACT | TYPE_ATTRIBUTE_INTERFACE
            LOGWBNM(OBFUSCATE_BNM("Вы пытаетесь создать объект абстрактного класса или интерфейса %s?\nВ C# это невозможно."), str().c_str());

        DO_API(IL2CPP::Il2CppObject *, il2cpp_object_new, (IL2CPP::Il2CppClass *));
        auto obj = il2cpp_object_new(klass);
        if (obj) memset((char*)obj + sizeof(IL2CPP::Il2CppObject), 0, klass->instance_size - sizeof(IL2CPP::Il2CppObject));
        return (void *) obj;
    }


    [[maybe_unused]] std::string LoadClass::str() const {
        if (klass) {
            TryInit(); // Попробовать инициализировать класс
            return OBFUSCATES_BNM("[") + klass->image->name + OBFUSCATES_BNM("]::[") + klass->namespaze + OBFUSCATES_BNM("]::[") + klass->name + OBFUSCATES_BNM("]");
        }
        return OBFUSCATES_BNM("Мёртвый класс");
    }

    // Попробовать инициализировать класс, если он жив
    void LoadClass::TryInit() const { if (klass) BNM_Internal::Class$$Init(klass); }

    IL2CPP::Il2CppObject *LoadClass::ObjBox(IL2CPP::Il2CppClass *klass, void *data) {
        DO_API(IL2CPP::Il2CppObject *, il2cpp_value_box, (IL2CPP::Il2CppClass *, void *));
        return il2cpp_value_box(klass, data);
    }

    IL2CPP::Il2CppArray *LoadClass::ArrayNew(IL2CPP::Il2CppClass *cls, IL2CPP::il2cpp_array_size_t length) {
        DO_API(IL2CPP::Il2CppArray *, il2cpp_array_new, (IL2CPP::Il2CppClass *, IL2CPP::il2cpp_array_size_t));
        return il2cpp_array_new(cls, length);
    }

    void MonoListFinalize(IL2CPP::Il2CppObject *obj) { // Метод очистки monoList
        for (auto i = 4; i < obj->klass->vtable_count; ++i) free((void *)obj->klass->vtable[i].method);
    }
    BNM::IL2CPP::MethodInfo* GetListFinalize() {
        static std::once_flag once{};
        static BNM::IL2CPP::MethodInfo* ret{};
        std::call_once(once, []() {
            auto i = BNM::GetType<void*>().ToLC().GetMethodByName(OBFUSCATE_BNM("Finalize")).myInfo;
            ret = (IL2CPP::MethodInfo *)malloc(sizeof(IL2CPP::MethodInfo));
            memcpy((void *)ret, (void *)i, sizeof(IL2CPP::MethodInfo));
            ret->methodPointer = (decltype(ret->methodPointer)) MonoListFinalize;
        });
        return ret;
    }
    void *LoadClass::NewListInstance() {
        static std::once_flag flag;
        static LoadClass LC;
        std::call_once(flag, [](){
            LC = LoadClass(OBFUSCATE_BNM("System.Collections.Generic"), OBFUSCATE_BNM("List`1"));
            auto cls = LC.klass;
            auto size = sizeof(IL2CPP::Il2CppClass) + cls->vtable_count * sizeof(IL2CPP::VirtualInvokeData);
            LC.klass = (IL2CPP::Il2CppClass *)malloc(size);
            memcpy(LC.klass, cls, size);
            LC.klass->has_finalize = 1;
            LC.klass->instance_size = sizeof(MONO_STRUCTS::monoList<void*>);
            LC.klass->vtable[1].method = GetListFinalize();
            LC.klass->vtable[1].methodPtr = LC.klass->vtable[1].method->methodPointer;
            // Обход создания статического поля _emptyArray, потому что его не может существовать
            LC.klass->has_cctor = 0;
            LC.klass->cctor_started = 0;
#if UNITY_VER >= 212
            LC.klass->cctor_finished_or_no_cctor = 1;
#else
            LC.klass->cctor_finished = 1;
#endif
            auto info = LC.GetMethodByName(OBFUSCATE_BNM(".ctor"), 0).myInfo;
            info->methodPointer = (decltype(info->methodPointer)) BNM_Internal::Empty;
        });
        auto r = LC.CreateNewInstance();
        LOGIBNM("NewListInstance 4, %p", r);
        return r;

    }


    /*** RuntimeTypeGetter ***/
    LoadClass RuntimeTypeGetter::ToLC() {
        if (!loadedClass.klass) {
            if (!name) {
                namespaze = OBFUSCATE_BNM("System");
                name = OBFUSCATE_BNM("Object");
            }
            loadedClass = LoadClass(namespaze, name);
            if (isArray) loadedClass = loadedClass.GetArrayClass();
        }
        return loadedClass;
    }
    RuntimeTypeGetter::operator LoadClass() { return ToLC(); }

    IL2CPP::Il2CppType *RuntimeTypeGetter::ToIl2CppType() { return ToLC().GetIl2CppType(); }
    RuntimeTypeGetter::operator IL2CPP::Il2CppType*() { return ToIl2CppType(); }

    IL2CPP::Il2CppClass *RuntimeTypeGetter::ToIl2CppClass() { return ToLC().GetIl2CppClass(); }
    RuntimeTypeGetter::operator IL2CPP::Il2CppClass*() { return ToIl2CppClass(); }

    // Для статических полей потоков
    namespace PRIVATE_FILED_UTILS {
        void GetStaticValue(IL2CPP::FieldInfo *info, void *value) {
            DO_API(void, il2cpp_field_static_get_value, (IL2CPP::FieldInfo *, void *));
            return il2cpp_field_static_get_value(info, value);
        }
        void SetStaticValue(IL2CPP::FieldInfo *info, void *value) {
            DO_API(void, il2cpp_field_static_set_value, (IL2CPP::FieldInfo *, void *));
            return il2cpp_field_static_set_value(info, value);
        }
    }

    /*** BNM-методы ***/
    using namespace MONO_STRUCTS;
    [[maybe_unused]] bool AttachIl2Cpp() {
        if (CurrentIl2CppThread()) return false;
        DO_API(IL2CPP::Il2CppDomain *, il2cpp_domain_get, ());
        DO_API(IL2CPP::Il2CppThread *, il2cpp_thread_attach, (IL2CPP::Il2CppDomain *));
        il2cpp_thread_attach(il2cpp_domain_get());
        return true;
    }

    [[maybe_unused]] IL2CPP::Il2CppThread *CurrentIl2CppThread() {
        DO_API(IL2CPP::Il2CppThread *, il2cpp_thread_current, ());
        return il2cpp_thread_current();
    }

    [[maybe_unused]] void DetachIl2Cpp() {
        auto thread = CurrentIl2CppThread();
        if (!thread) return;
        DO_API(void, il2cpp_thread_detach, (IL2CPP::Il2CppThread *));
        il2cpp_thread_detach(thread);
    }

    // Создать обычную C#-строку, используя il2cpp
    monoString *CreateMonoString(const char *str) {
        DO_API(monoString *, il2cpp_string_new, (const char *str));
        return il2cpp_string_new(str);
    }
    [[maybe_unused]] monoString *CreateMonoString(const std::string &str) { return CreateMonoString(str2char(str)); }

    void *GetExternMethod(const std::string &str) {
        DO_API(void *, il2cpp_resolve_icall, (const char *));
        auto c_str = str2char(str);
        auto ret = il2cpp_resolve_icall(c_str);
        if (!ret) LOGWBNM(OBFUSCATE_BNM("Внешний метод %s не найден. Пожалуйста, проверьте код."), c_str);
        return ret;
    }


    [[maybe_unused]] std::string GetLibIl2CppPath() {
        if (!BNM_Internal::dlLib) return OBFUSCATE_BNM("libil2cpp не найден!");
        return BNM_Internal::LibAbsolutePath;
    }

    [[maybe_unused]] BNM_PTR GetLibIl2CppOffset() {
        return BNM_Internal::LibAbsoluteAddress;
    }

    void *offsetInLib(void *offsetInMemory) {
        Dl_info info; BNM_dladdr(offsetInMemory, &info);
        return (void *) ((BNM_PTR) offsetInMemory - (BNM_PTR) info.dli_fbase);
    }

    [[maybe_unused]] void *GetLibIl2CppDlInst() {
        return BNM_Internal::dlLib;
    }

    bool InvokeHookImpl(IL2CPP::MethodInfo *m, void *newMet, void **oldMet) {
        if (!m) return false;
        if (oldMet) *oldMet = (void *)m->methodPointer;
        m->methodPointer = (void(*)())newMet;
        return true;
    }

    bool VirtualHookImpl(BNM::LoadClass targetClass, IL2CPP::MethodInfo *m, void *newMet, void **oldMet) {
        if (!m || !targetClass) return false;
        DO_API(bool, il2cpp_type_equals, (const IL2CPP::Il2CppType * type, const IL2CPP::Il2CppType * otherType));
        for (uint16_t i = 0; i < targetClass.klass->vtable_count; ++i) {
            auto &vTable = targetClass.klass->vtable[i];
            auto count = vTable.method->parameters_count;

            if (!strcmp(vTable.method->name, m->name) && count == m->parameters_count) {
                bool same = true;
                for (uint8_t p = 0; p < count; ++p) {
#if UNITY_VER < 212
                    auto type = (vTable.method->parameters + p)->parameter_type;
                    auto type2 = (m->parameters + p)->parameter_type;
#else
                    auto type = vTable.method->parameters[p];
                    auto type2 = m->parameters[p];
#endif
                    if (il2cpp_type_equals(type, type2)) continue;

                    same = false;
                    break;
                }
                if (!same) break;

                if (oldMet) *oldMet = (void *)vTable.methodPtr;
                vTable.methodPtr = (void(*)()) newMet;
                return true;

            }
        }
        return false;
    }

    template<> bool IsA<IL2CPP::Il2CppObject *>(IL2CPP::Il2CppObject *object, IL2CPP::Il2CppClass *klass) {
        if (!object || !klass) return false;
        for (auto cls = object->klass; cls; cls = cls->parent) if (cls == klass) return true;
        return false;
    }

    namespace UNITY_STRUCTS {
        [[maybe_unused]] void *RaycastHit::get_Collider() const {
            if (!m_Collider || (BNM_PTR)m_Collider < 0) return {};
#if UNITY_VER > 174
            static void *(*FromId)(int);
            if (!FromId) InitResolveFunc(FromId, OBFUSCATE_BNM("UnityEngine.Object::FindObjectFromInstanceID"));
            return FromId(m_Collider);
#else
            return m_Collider;
#endif
        }
        [[maybe_unused]] const float Vector4::infinity = std::numeric_limits<float>::infinity();
        [[maybe_unused]] const Vector4 Vector4::infinityVec = {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
        [[maybe_unused]] const Vector4 Vector4::zero = {0, 0, 0, 0};
        [[maybe_unused]] const Vector4 Vector4::one = {1, 1, 1, 1};
        [[maybe_unused]] const float Vector3::epsilon = 0.00001f;
        [[maybe_unused]] const Vector3 Vector3::infinityVec = {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
        [[maybe_unused]] const Vector3 Vector3::zero = {0.f, 0.f, 0.f};
        [[maybe_unused]] const Vector3 Vector3::one = {1.f, 1.f, 1.f};
        [[maybe_unused]] const Vector3 Vector3::up = {0.f, 1.f, 0.f};
        [[maybe_unused]] const Vector3 Vector3::down = {0.f, -1.f, 0.f};
        [[maybe_unused]] const Vector3 Vector3::left = {-1.f, 0.f, 0.f};
        [[maybe_unused]] const Vector3 Vector3::right = {1.f, 0.f, 0.f};
        [[maybe_unused]] const Vector3 Vector3::forward = {0.f, 0.f, 1.f};
        [[maybe_unused]] const Vector3 Vector3::back = {0.f, 0.f, -1.f};

        inline Vector3 Matrix3x3::MultiplyVector3(const Vector3& v) const
        {
            Vector3 res;
            res.x = m_Data[0] * v.x + m_Data[3] * v.y + m_Data[6] * v.z;
            res.y = m_Data[1] * v.x + m_Data[4] * v.y + m_Data[7] * v.z;
            res.z = m_Data[2] * v.x + m_Data[5] * v.y + m_Data[8] * v.z;
            return res;
        }

        inline void Matrix3x3::MultiplyVector3(const Vector3& v, Vector3& output) const
        {
            output.x = m_Data[0] * v.x + m_Data[3] * v.y + m_Data[6] * v.z;
            output.y = m_Data[1] * v.x + m_Data[4] * v.y + m_Data[7] * v.z;
            output.z = m_Data[2] * v.x + m_Data[5] * v.y + m_Data[8] * v.z;
        }

        inline void MultiplyMatrices3x3(const Matrix3x3* __restrict lhs, const Matrix3x3* __restrict rhs, Matrix3x3* __restrict res)
        {
            for (int i = 0; i < 3; ++i)
            {
                res->m_Data[i]    = lhs->m_Data[i] * rhs->m_Data[0]  + lhs->m_Data[i + 3] * rhs->m_Data[1]  + lhs->m_Data[i + 6] * rhs->m_Data[2];
                res->m_Data[i + 3]  = lhs->m_Data[i] * rhs->m_Data[3]  + lhs->m_Data[i + 3] * rhs->m_Data[4]  + lhs->m_Data[i + 6] * rhs->m_Data[5];
                res->m_Data[i + 6]  = lhs->m_Data[i] * rhs->m_Data[6]  + lhs->m_Data[i + 3] * rhs->m_Data[7]  + lhs->m_Data[i + 6] * rhs->m_Data[8];
            }
        }

        inline Matrix3x3 operator*(const Matrix3x3& lhs, const Matrix3x3& rhs)
        {
            Matrix3x3 temp;
            MultiplyMatrices3x3(&lhs, &rhs, &temp);
            return temp;
        }

        inline Vector3 Matrix3x3::MultiplyVector3Transpose(const Vector3& v) const
        {
            Vector3 res;
            res.x = Get(0, 0) * v.x + Get(1, 0) * v.y + Get(2, 0) * v.z;
            res.y = Get(0, 1) * v.x + Get(1, 1) * v.y + Get(2, 1) * v.z;
            res.z = Get(0, 2) * v.x + Get(1, 2) * v.y + Get(2, 2) * v.z;
            return res;
        }

        Matrix3x3& Matrix3x3::SetIdentity()
        {
            Get(0, 0) = 1.0F;  Get(0, 1) = 0.0F;  Get(0, 2) = 0.0F;
            Get(1, 0) = 0.0F;  Get(1, 1) = 1.0F;  Get(1, 2) = 0.0F;
            Get(2, 0) = 0.0F;  Get(2, 1) = 0.0F;  Get(2, 2) = 1.0F;
            return *this;
        }

        Matrix3x3& Matrix3x3::SetZero()
        {
            Get(0, 0) = 0.0F;  Get(0, 1) = 0.0F;  Get(0, 2) = 0.0F;
            Get(1, 0) = 0.0F;  Get(1, 1) = 0.0F;  Get(1, 2) = 0.0F;
            Get(2, 0) = 0.0F;  Get(2, 1) = 0.0F;  Get(2, 2) = 0.0F;
            return *this;
        }

        Matrix3x3& Matrix3x3::SetBasis(const Vector3& inX, const Vector3& inY, const Vector3& inZ)
        {
            Get(0, 0) = inX[0];    Get(0, 1) = inY[0];    Get(0, 2) = inZ[0];
            Get(1, 0) = inX[1];    Get(1, 1) = inY[1];    Get(1, 2) = inZ[1];
            Get(2, 0) = inX[2];    Get(2, 1) = inY[2];    Get(2, 2) = inZ[2];
            return *this;
        }

        Matrix3x3& Matrix3x3::SetBasisTransposed(const Vector3& inX, const Vector3& inY, const Vector3& inZ)
        {
            Get(0, 0) = inX[0];    Get(1, 0) = inY[0];    Get(2, 0) = inZ[0];
            Get(0, 1) = inX[1];    Get(1, 1) = inY[1];    Get(2, 1) = inZ[1];
            Get(0, 2) = inX[2];    Get(1, 2) = inY[2];    Get(2, 2) = inZ[2];
            return *this;
        }

        Matrix3x3& Matrix3x3::SetScale(const Vector3& inScale)
        {
            Get(0, 0) = inScale[0];    Get(0, 1) = 0.0F;          Get(0, 2) = 0.0F;
            Get(1, 0) = 0.0F;          Get(1, 1) = inScale[1];    Get(1, 2) = 0.0F;
            Get(2, 0) = 0.0F;          Get(2, 1) = 0.0F;          Get(2, 2) = inScale[2];
            return *this;
        }
        inline bool CompareApproximately(float f0, float f1, float epsilon = 0.000001F) {
            float dist = (f0 - f1);
            dist = abs(dist);
            return dist <= epsilon;
        }
        bool Matrix3x3::IsIdentity(float threshold)
        {
            if (CompareApproximately(Get(0, 0), 1.0f, threshold) && CompareApproximately(Get(0, 1), 0.0f, threshold) && CompareApproximately(Get(0, 2), 0.0f, threshold) &&
                CompareApproximately(Get(1, 0), 0.0f, threshold) && CompareApproximately(Get(1, 1), 1.0f, threshold) && CompareApproximately(Get(1, 2), 0.0f, threshold) &&
                CompareApproximately(Get(2, 0), 0.0f, threshold) && CompareApproximately(Get(2, 1), 0.0f, threshold) && CompareApproximately(Get(2, 2), 1.0f, threshold))
                return true;
            return false;
        }

        Matrix3x3& Matrix3x3::Scale(const Vector3& inScale)
        {
            Get(0, 0) *= inScale[0];
            Get(1, 0) *= inScale[0];
            Get(2, 0) *= inScale[0];

            Get(0, 1) *= inScale[1];
            Get(1, 1) *= inScale[1];
            Get(2, 1) *= inScale[1];

            Get(0, 2) *= inScale[2];
            Get(1, 2) *= inScale[2];
            Get(2, 2) *= inScale[2];
            return *this;
        }

        float Matrix3x3::GetDeterminant() const
        {
            float fCofactor0 = Get(0, 0) * Get(1, 1) * Get(2, 2);
            float fCofactor1 = Get(0, 1) * Get(1, 2) * Get(2, 0);
            float fCofactor2 = Get(0, 2) * Get(1, 0) * Get(2, 1);

            float fCofactor3 = Get(0, 2) * Get(1, 1) * Get(2, 0);
            float fCofactor4 = Get(0, 1) * Get(1, 0) * Get(2, 2);
            float fCofactor5 = Get(0, 0) * Get(1, 2) * Get(2, 1);

            return fCofactor0 + fCofactor1 + fCofactor2 - fCofactor3 - fCofactor4 - fCofactor5;
        }

        Matrix3x3& Matrix3x3::Transpose()
        {
            std::swap(Get(0, 1), Get(1, 0));
            std::swap(Get(0, 2), Get(2, 0));
            std::swap(Get(2, 1), Get(1, 2));
            return *this;
        }

        void Matrix3x3::InvertTranspose()
        {
            Invert();
            Transpose();
        }

        Matrix3x3& Matrix3x3::operator*=(float f)
        {
            for (float & i : m_Data)
                i *= f;
            return *this;
        }

        Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& inM)
        {
            int i;
            for (i = 0; i < 3; i++)
            {
                float v[3] = {Get(i, 0), Get(i, 1), Get(i, 2)};
                Get(i, 0) = v[0] * inM.Get(0, 0) + v[1] * inM.Get(1, 0) + v[2] * inM.Get(2, 0);
                Get(i, 1) = v[0] * inM.Get(0, 1) + v[1] * inM.Get(1, 1) + v[2] * inM.Get(2, 1);
                Get(i, 2) = v[0] * inM.Get(0, 2) + v[1] * inM.Get(1, 2) + v[2] * inM.Get(2, 2);
            }
            return *this;
        }


        Matrix3x3& Matrix3x3::SetAxisAngle(const Vector3& rotationAxis, float radians)
        {
            GetRotMatrixNormVec(m_Data, rotationAxis.GetPtr(), radians);
            return *this;
        }

        void Matrix3x3::EulerToMatrix(const Vector3& v, Matrix3x3& matrix)
        {
            float cx = cos(v.x);
            float sx = sin(v.x);
            float cy = cos(v.y);
            float sy = sin(v.y);
            float cz = cos(v.z);
            float sz = sin(v.z);

            matrix.Get(0, 0) = cy * cz + sx * sy * sz;
            matrix.Get(0, 1) = cz * sx * sy - cy * sz;
            matrix.Get(0, 2) = cx * sy;

            matrix.Get(1, 0) = cx * sz;
            matrix.Get(1, 1) = cx * cz;
            matrix.Get(1, 2) = -sx;

            matrix.Get(2, 0) = -cz * sy + cy * sx * sz;
            matrix.Get(2, 1) = cy * cz * sx + sy * sz;
            matrix.Get(2, 2) = cx * cy;
        }

        Matrix3x3& Matrix3x3::SetFromToRotation(const Vector3& from, const Vector3& to)
        {
            Vector3 v = Vector3::Cross(from, to);
            float e = Vector3::Dot(from, to);
            const float kEpsilon = 0.000001f;
            if (e > 1.0 - kEpsilon)
            {
                Get(0, 0) = 1.0; Get(0, 1) = 0.0; Get(0, 2) = 0.0;
                Get(1, 0) = 0.0; Get(1, 1) = 1.0; Get(1, 2) = 0.0;
                Get(2, 0) = 0.0; Get(2, 1) = 0.0; Get(2, 2) = 1.0;
            }
            else if (e < -1.0 + kEpsilon)
            {
                float invlen;
                float fxx, fyy, fzz, fxy, fxz, fyz;
                float uxx, uyy, uzz, uxy, uxz, uyz;
                float lxx, lyy, lzz, lxy, lxz, lyz;
                Vector3 left(0.0f, from[2], -from[1]);
                if (Vector3::Dot(left, left) < kEpsilon) left[0] = -from[2]; left[1] = 0.0; left[2] = from[0];
                
                invlen = 1.0f / sqrt(Vector3::Dot(left, left));
                left[0] *= invlen;
                left[1] *= invlen;
                left[2] *= invlen;
                Vector3 up = Vector3::Cross(left, from);
                fxx = -from[0] * from[0]; fyy = -from[1] * from[1]; fzz = -from[2] * from[2];
                fxy = -from[0] * from[1]; fxz = -from[0] * from[2]; fyz = -from[1] * from[2];

                uxx = up[0] * up[0]; uyy = up[1] * up[1]; uzz = up[2] * up[2];
                uxy = up[0] * up[1]; uxz = up[0] * up[2]; uyz = up[1] * up[2];

                lxx = -left[0] * left[0]; lyy = -left[1] * left[1]; lzz = -left[2] * left[2];
                lxy = -left[0] * left[1]; lxz = -left[0] * left[2]; lyz = -left[1] * left[2];

                Get(0, 0) = fxx + uxx + lxx; Get(0, 1) = fxy + uxy + lxy; Get(0, 2) = fxz + uxz + lxz;
                Get(1, 0) = Get(0, 1);   Get(1, 1) = fyy + uyy + lyy; Get(1, 2) = fyz + uyz + lyz;
                Get(2, 0) = Get(0, 2);   Get(2, 1) = Get(1, 2);   Get(2, 2) = fzz + uzz + lzz;
            } else {
                float hvx, hvz, hvxy, hvxz, hvyz;
                float h = (1.0f - e) / Vector3::Dot(v, v);
                hvx = h * v[0];
                hvz = h * v[2];
                hvxy = hvx * v[1];
                hvxz = hvx * v[2];
                hvyz = hvz * v[1];
                Get(0, 0) = e + hvx * v[0]; Get(0, 1) = hvxy - v[2];     Get(0, 2) = hvxz + v[1];
                Get(1, 0) = hvxy + v[2];  Get(1, 1) = e + h * v[1] * v[1]; Get(1, 2) = hvyz - v[0];
                Get(2, 0) = hvxz - v[1];  Get(2, 1) = hvyz + v[0];     Get(2, 2) = e + hvz * v[2];
            }
            return *this;
        }

        bool Matrix3x3::LookRotationToMatrix(const Vector3& viewVec, const Vector3& upVec, Matrix3x3* m)
        {
            Vector3 z = viewVec;
            float mag = Vector3::Magnitude(z);
            if (mag < Vector3::epsilon)
            {
                m->SetIdentity();
                return false;
            }
            z /= mag;

            Vector3 x = Vector3::Cross(upVec, z);
            mag = Vector3::Magnitude(x);
            if (mag < Vector3::epsilon)
            {
                m->SetIdentity();
                return false;
            }
            x /= mag;

            Vector3 y(Vector3::Cross(z, x));
            if (!CompareApproximately(Vector3::SqrMagnitude(y), 1.0F))
                return false;

            m->SetBasis(x, y, z);
            return true;
        }

        void Matrix3x3::GetRotMatrixNormVec(float* out, const float* inVec, float radians)
        {
            float s, c;
            float vx, vy, vz, xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;

            s = sin(radians);
            c = cos(radians);

            vx = inVec[0];
            vy = inVec[1];
            vz = inVec[2];

#define M(row, col)  out[(row)*3 + col]
            xx = vx * vx;
            yy = vy * vy;
            zz = vz * vz;
            xy = vx * vy;
            yz = vy * vz;
            zx = vz * vx;
            xs = vx * s;
            ys = vy * s;
            zs = vz * s;
            one_c = 1.0F - c;

            M(0, 0) = (one_c * xx) + c;
            M(1, 0) = (one_c * xy) - zs;
            M(2, 0) = (one_c * zx) + ys;

            M(0, 1) = (one_c * xy) + zs;
            M(1, 1) = (one_c * yy) + c;
            M(2, 1) = (one_c * yz) - xs;

            M(0, 2) = (one_c * zx) - ys;
            M(1, 2) = (one_c * yz) + xs;
            M(2, 2) = (one_c * zz) + c;

#undef M
        }

        void Matrix3x3::OrthoNormalize(Matrix3x3& matrix)
        {
            Vector3* c0 = (Vector3*)matrix.GetPtr() + 0;
            Vector3* c1 = (Vector3*)matrix.GetPtr() + 3;
            Vector3* c2 = (Vector3*)matrix.GetPtr() + 6;
            Vector3::OrthoNormalize(c0, c1, c2);
        }
        Matrix3x3& Matrix3x3::operator=(const Matrix4x4& other)
        {
            m_Data[0] = other.m_Data[0];
            m_Data[1] = other.m_Data[1];
            m_Data[2] = other.m_Data[2];

            m_Data[3] = other.m_Data[4];
            m_Data[4] = other.m_Data[5];
            m_Data[5] = other.m_Data[6];

            m_Data[6] = other.m_Data[8];
            m_Data[7] = other.m_Data[9];
            m_Data[8] = other.m_Data[10];
            return *this;
        }
        Matrix3x3::Matrix3x3(const Matrix4x4& other)
        {
            m_Data[0] = other.m_Data[0];
            m_Data[1] = other.m_Data[1];
            m_Data[2] = other.m_Data[2];

            m_Data[3] = other.m_Data[4];
            m_Data[4] = other.m_Data[5];
            m_Data[5] = other.m_Data[6];

            m_Data[6] = other.m_Data[8];
            m_Data[7] = other.m_Data[9];
            m_Data[8] = other.m_Data[10];
        }
        Matrix3x3& Matrix3x3::operator*=(const Matrix4x4& inM)
        {
            int i;
            for (i = 0; i < 3; i++)
            {
                float v[3] = {Get(i, 0), Get(i, 1), Get(i, 2)};
                Get(i, 0) = v[0] * inM.Get(0, 0) + v[1] * inM.Get(1, 0) + v[2] * inM.Get(2, 0);
                Get(i, 1) = v[0] * inM.Get(0, 1) + v[1] * inM.Get(1, 1) + v[2] * inM.Get(2, 1);
                Get(i, 2) = v[0] * inM.Get(0, 2) + v[1] * inM.Get(1, 2) + v[2] * inM.Get(2, 2);
            }
            return *this;
        }
        bool Matrix3x3::Invert()
        {
            Matrix4x4 m = *this;
            bool success = InvertMatrix4x4_Full(m.GetPtr(), m.GetPtr());
            *this = m;
            return success;
        }
        void QuaternionToMatrix(const Quaternion& q, Matrix4x4& m)
        {
            float x = q.x * 2.0F;
            float y = q.y * 2.0F;
            float z = q.z * 2.0F;
            float xx = q.x * x;
            float yy = q.y * y;
            float zz = q.z * z;
            float xy = q.x * y;
            float xz = q.x * z;
            float yz = q.y * z;
            float wx = q.w * x;
            float wy = q.w * y;
            float wz = q.w * z;

            m.m_Data[0] = 1.0f - (yy + zz);
            m.m_Data[1] = xy + wz;
            m.m_Data[2] = xz - wy;
            m.m_Data[3] = 0.0F;

            m.m_Data[4] = xy - wz;
            m.m_Data[5] = 1.0f - (xx + zz);
            m.m_Data[6] = yz + wx;
            m.m_Data[7] = 0.0F;

            m.m_Data[8]  = xz + wy;
            m.m_Data[9]  = yz - wx;
            m.m_Data[10] = 1.0f - (xx + yy);
            m.m_Data[11] = 0.0F;

            m.m_Data[12] = 0.0F;
            m.m_Data[13] = 0.0F;
            m.m_Data[14] = 0.0F;
            m.m_Data[15] = 1.0F;
        }
        const Matrix4x4 Matrix4x4::identity(kIdentity);

        Matrix4x4::Matrix4x4(const Matrix3x3 &other)
        {
            m_Data[0] = other.m_Data[0];
            m_Data[1] = other.m_Data[1];
            m_Data[2] = other.m_Data[2];
            m_Data[3] = 0.0F;

            m_Data[4] = other.m_Data[3];
            m_Data[5] = other.m_Data[4];
            m_Data[6] = other.m_Data[5];
            m_Data[7] = 0.0F;

            m_Data[8] = other.m_Data[6];
            m_Data[9] = other.m_Data[7];
            m_Data[10] = other.m_Data[8];
            m_Data[11] = 0.0F;

            m_Data[12] = 0.0F;
            m_Data[13] = 0.0F;
            m_Data[14] = 0.0F;
            m_Data[15] = 1.0F;
        }

        Matrix4x4& Matrix4x4::operator=(const Matrix3x3& other)
        {
            m_Data[0] = other.m_Data[0];
            m_Data[1] = other.m_Data[1];
            m_Data[2] = other.m_Data[2];
            m_Data[3] = 0.0F;

            m_Data[4] = other.m_Data[3];
            m_Data[5] = other.m_Data[4];
            m_Data[6] = other.m_Data[5];
            m_Data[7] = 0.0F;

            m_Data[8] = other.m_Data[6];
            m_Data[9] = other.m_Data[7];
            m_Data[10] = other.m_Data[8];
            m_Data[11] = 0.0F;

            m_Data[12] = 0.0F;
            m_Data[13] = 0.0F;
            m_Data[14] = 0.0F;
            m_Data[15] = 1.0F;
            return *this;
        }

        bool Matrix4x4::IsIdentity(float threshold) const
        {
            if (CompareApproximately(Get(0, 0), 1.0f, threshold) && CompareApproximately(Get(0, 1), 0.0f, threshold) && CompareApproximately(Get(0, 2), 0.0f, threshold) && CompareApproximately(Get(0, 3), 0.0f, threshold) &&
                CompareApproximately(Get(1, 0), 0.0f, threshold) && CompareApproximately(Get(1, 1), 1.0f, threshold) && CompareApproximately(Get(1, 2), 0.0f, threshold) && CompareApproximately(Get(1, 3), 0.0f, threshold) &&
                CompareApproximately(Get(2, 0), 0.0f, threshold) && CompareApproximately(Get(2, 1), 0.0f, threshold) && CompareApproximately(Get(2, 2), 1.0f, threshold) && CompareApproximately(Get(2, 3), 0.0f, threshold) &&
                CompareApproximately(Get(3, 0), 0.0f, threshold) && CompareApproximately(Get(3, 1), 0.0f, threshold) && CompareApproximately(Get(3, 2), 0.0f, threshold) && CompareApproximately(Get(3, 3), 1.0f, threshold))
                return true;
            return false;
        }

        void Matrix4x4::RemoveScale()
        {
            SetAxisX(Vector3::Normalize(GetAxisX()));
            SetAxisY(Vector3::Normalize(GetAxisY()));
            SetAxisZ(Vector3::Normalize(GetAxisZ()));
        }

        float Matrix4x4::GetDeterminant2x2() const
        {
            return Get(0,0)*Get(1,1) - Get(0,1)*Get(1,0);
        }

        float Matrix4x4::GetDeterminant() const
        {
            double m00 = Get(0, 0);  double m01 = Get(0, 1);  double m02 = Get(0, 2);  double m03 = Get(0, 3);
            double m10 = Get(1, 0);  double m11 = Get(1, 1);  double m12 = Get(1, 2);  double m13 = Get(1, 3);
            double m20 = Get(2, 0);  double m21 = Get(2, 1);  double m22 = Get(2, 2);  double m23 = Get(2, 3);
            double m30 = Get(3, 0);  double m31 = Get(3, 1);  double m32 = Get(3, 2);  double m33 = Get(3, 3);

            double result =
                    m03 * m12 * m21 * m30 - m02 * m13 * m21 * m30 - m03 * m11 * m22 * m30 + m01 * m13 * m22 * m30 +
                    m02 * m11 * m23 * m30 - m01 * m12 * m23 * m30 - m03 * m12 * m20 * m31 + m02 * m13 * m20 * m31 +
                    m03 * m10 * m22 * m31 - m00 * m13 * m22 * m31 - m02 * m10 * m23 * m31 + m00 * m12 * m23 * m31 +
                    m03 * m11 * m20 * m32 - m01 * m13 * m20 * m32 - m03 * m10 * m21 * m32 + m00 * m13 * m21 * m32 +
                    m01 * m10 * m23 * m32 - m00 * m11 * m23 * m32 - m02 * m11 * m20 * m33 + m01 * m12 * m20 * m33 +
                    m02 * m10 * m21 * m33 - m00 * m12 * m21 * m33 - m01 * m10 * m22 * m33 + m00 * m11 * m22 * m33;
            return (float)result;
        }
        inline void MultiplyMatrices4x4(const Matrix4x4* __restrict lhs, const Matrix4x4* __restrict rhs, Matrix4x4* __restrict res) {
            for (int i = 0; i < 4; i++)
            {
                res->m_Data[i] = lhs->m_Data[i] * rhs->m_Data[0] + lhs->m_Data[i + 4] * rhs->m_Data[1] + lhs->m_Data[i + 8] * rhs->m_Data[2] + lhs->m_Data[i + 12] * rhs->m_Data[3];
                res->m_Data[i + 4] = lhs->m_Data[i] * rhs->m_Data[4] + lhs->m_Data[i + 4] * rhs->m_Data[5] + lhs->m_Data[i + 8] * rhs->m_Data[6] + lhs->m_Data[i + 12] * rhs->m_Data[7];
                res->m_Data[i + 8] = lhs->m_Data[i] * rhs->m_Data[8] + lhs->m_Data[i + 4] * rhs->m_Data[9] + lhs->m_Data[i + 8] * rhs->m_Data[10] + lhs->m_Data[i + 12] * rhs->m_Data[11];
                res->m_Data[i + 12] = lhs->m_Data[i] * rhs->m_Data[12] + lhs->m_Data[i + 4] * rhs->m_Data[13] + lhs->m_Data[i + 8] * rhs->m_Data[14] + lhs->m_Data[i + 12] * rhs->m_Data[15];
            }
        }
        Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& inM1)
        {
            Matrix4x4 tmp;
            MultiplyMatrices4x4(this, &inM1, &tmp);
            *this = tmp;
            return *this;
        }

        void MultiplyMatrices3x4(const Matrix4x4& lhs, const Matrix4x4& rhs, Matrix4x4& res)
        {
            for (int i = 0; i < 3; i++)
            {
                res.m_Data[i]    = lhs.m_Data[i] * rhs.m_Data[0]  + lhs.m_Data[i + 4] * rhs.m_Data[1]  + lhs.m_Data[i + 8] * rhs.m_Data[2];//  + lhs.m_Data[i+12] * rhs.m_Data[3];
                res.m_Data[i + 4]  = lhs.m_Data[i] * rhs.m_Data[4]  + lhs.m_Data[i + 4] * rhs.m_Data[5]  + lhs.m_Data[i + 8] * rhs.m_Data[6];//  + lhs.m_Data[i+12] * rhs.m_Data[7];
                res.m_Data[i + 8]  = lhs.m_Data[i] * rhs.m_Data[8]  + lhs.m_Data[i + 4] * rhs.m_Data[9]  + lhs.m_Data[i + 8] * rhs.m_Data[10];// + lhs.m_Data[i+12] * rhs.m_Data[11];
                res.m_Data[i + 12] = lhs.m_Data[i] * rhs.m_Data[12] + lhs.m_Data[i + 4] * rhs.m_Data[13] + lhs.m_Data[i + 8] * rhs.m_Data[14] + lhs.m_Data[i + 12];// * rhs.m_Data[15];
            }

            res.m_Data[3]  = 0.0f;
            res.m_Data[7]  = 0.0f;
            res.m_Data[11] = 0.0f;
            res.m_Data[15] = 1.0f;
        }

        void MultiplyMatrices2D(const Matrix4x4& lhs, const Matrix4x4& rhs, Matrix4x4& res)
        {
            res.m_Data[0] = lhs.m_Data[0] * rhs.m_Data[0]  + lhs.m_Data[0 + 4] * rhs.m_Data[1]  + lhs.m_Data[0 + 8] * rhs.m_Data[2];
            res.m_Data[4] = lhs.m_Data[0] * rhs.m_Data[4]  + lhs.m_Data[0 + 4] * rhs.m_Data[5]  + lhs.m_Data[0 + 8] * rhs.m_Data[6];
            res.m_Data[12] = lhs.m_Data[0] * rhs.m_Data[12] + lhs.m_Data[0 + 4] * rhs.m_Data[13] + lhs.m_Data[0 + 8] * rhs.m_Data[14] + lhs.m_Data[0 + 12];
            res.m_Data[1]    = lhs.m_Data[1] * rhs.m_Data[0]  + lhs.m_Data[1 + 4] * rhs.m_Data[1]  + lhs.m_Data[1 + 8] * rhs.m_Data[2];
            res.m_Data[5]  = lhs.m_Data[1] * rhs.m_Data[4]  + lhs.m_Data[1 + 4] * rhs.m_Data[5]  + lhs.m_Data[1 + 8] * rhs.m_Data[6];
            res.m_Data[13] = lhs.m_Data[1] * rhs.m_Data[12] + lhs.m_Data[1 + 4] * rhs.m_Data[13] + lhs.m_Data[1 + 8] * rhs.m_Data[14] + lhs.m_Data[1 + 12];
            res.m_Data[2]    = lhs.m_Data[2] * rhs.m_Data[0]  + lhs.m_Data[2 + 4] * rhs.m_Data[1]  + lhs.m_Data[2 + 8] * rhs.m_Data[2];
            res.m_Data[14] = lhs.m_Data[2] * rhs.m_Data[12] + lhs.m_Data[2 + 4] * rhs.m_Data[13] + lhs.m_Data[2 + 8] * rhs.m_Data[14] + lhs.m_Data[2 + 12];
            res.m_Data[3]  = 0;
            res.m_Data[6]  = 0;
            res.m_Data[7]  = 0;
            res.m_Data[8]  = 0;
            res.m_Data[9]  = 0;
            res.m_Data[10]  = 1.0f;
            res.m_Data[11] = 0;
            res.m_Data[15] = 1.0f;
        }

        Matrix4x4& Matrix4x4::SetIdentity()
        {
            Get(0, 0) = 1.0;   Get(0, 1) = 0.0;   Get(0, 2) = 0.0;   Get(0, 3) = 0.0;
            Get(1, 0) = 0.0;   Get(1, 1) = 1.0;   Get(1, 2) = 0.0;   Get(1, 3) = 0.0;
            Get(2, 0) = 0.0;   Get(2, 1) = 0.0;   Get(2, 2) = 1.0;   Get(2, 3) = 0.0;
            Get(3, 0) = 0.0;   Get(3, 1) = 0.0;   Get(3, 2) = 0.0;   Get(3, 3) = 1.0;
            return *this;
        }

        Matrix4x4& Matrix4x4::SetZero()
        {
            Get(0, 0) = 0.0;   Get(0, 1) = 0.0;   Get(0, 2) = 0.0;   Get(0, 3) = 0.0;
            Get(1, 0) = 0.0;   Get(1, 1) = 0.0;   Get(1, 2) = 0.0;   Get(1, 3) = 0.0;
            Get(2, 0) = 0.0;   Get(2, 1) = 0.0;   Get(2, 2) = 0.0;   Get(2, 3) = 0.0;
            Get(3, 0) = 0.0;   Get(3, 1) = 0.0;   Get(3, 2) = 0.0;   Get(3, 3) = 0.0;
            return *this;
        }

        Matrix4x4& Matrix4x4::SetBasis(const Vector3& inX, const Vector3& inY, const Vector3& inZ)
        {
            Get(0, 0) = inX[0];    Get(0, 1) = inY[0];    Get(0, 2) = inZ[0];    Get(0, 3) = 0.0;
            Get(1, 0) = inX[1];    Get(1, 1) = inY[1];    Get(1, 2) = inZ[1];    Get(1, 3) = 0.0;
            Get(2, 0) = inX[2];    Get(2, 1) = inY[2];    Get(2, 2) = inZ[2];    Get(2, 3) = 0.0;
            Get(3, 0) = 0.0;       Get(3, 1) = 0.0;       Get(3, 2) = 0.0;       Get(3, 3) = 1.0;
            return *this;
        }

        Matrix4x4& Matrix4x4::SetBasisTransposed(const Vector3& inX, const Vector3& inY, const Vector3& inZ)
        {
            Get(0, 0) = inX[0];    Get(1, 0) = inY[0];    Get(2, 0) = inZ[0];    Get(3, 0) = 0.0;
            Get(0, 1) = inX[1];    Get(1, 1) = inY[1];    Get(2, 1) = inZ[1];    Get(3, 1) = 0.0;
            Get(0, 2) = inX[2];    Get(1, 2) = inY[2];    Get(2, 2) = inZ[2];    Get(3, 2) = 0.0;
            Get(0, 3) = 0.0;       Get(1, 3) = 0.0;       Get(2, 3) = 0.0;       Get(3, 3) = 1.0;
            return *this;
        }

        Matrix4x4& Matrix4x4::SetPositionAndOrthoNormalBasis(const Vector3& inPosition, const Vector3& inX, const Vector3& inY, const Vector3& inZ)
        {
            Get(0, 0) = inX[0];    Get(0, 1) = inY[0];    Get(0, 2) = inZ[0];    Get(0, 3) = inPosition[0];
            Get(1, 0) = inX[1];    Get(1, 1) = inY[1];    Get(1, 2) = inZ[1];    Get(1, 3) = inPosition[1];
            Get(2, 0) = inX[2];    Get(2, 1) = inY[2];    Get(2, 2) = inZ[2];    Get(2, 3) = inPosition[2];
            Get(3, 0) = 0.0;       Get(3, 1) = 0.0;       Get(3, 2) = 0.0;       Get(3, 3) = 1.0;
            return *this;
        }

        Matrix4x4& Matrix4x4::SetScaleAndPosition(const Vector3& inScale, const Vector3& inPosition)
        {
            Get(0, 0) = inScale[0];    Get(0, 1) = 0.0;           Get(0, 2) = 0.0;           Get(0, 3) = inPosition[0];
            Get(1, 0) = 0.0;           Get(1, 1) = inScale[1];    Get(1, 2) = 0.0;           Get(1, 3) = inPosition[1];
            Get(2, 0) = 0.0;           Get(2, 1) = 0.0;           Get(2, 2) = inScale[2];    Get(2, 3) = inPosition[2];
            Get(3, 0) = 0.0;           Get(3, 1) = 0.0;           Get(3, 2) = 0.0;           Get(3, 3) = 1.0;
            return *this;
        }

        Matrix4x4& Matrix4x4::Scale(const Vector3& inScale)
        {
            Get(0, 0) *= inScale[0];
            Get(1, 0) *= inScale[0];
            Get(2, 0) *= inScale[0];
            Get(3, 0) *= inScale[0];

            Get(0, 1) *= inScale[1];
            Get(1, 1) *= inScale[1];
            Get(2, 1) *= inScale[1];
            Get(3, 1) *= inScale[1];

            Get(0, 2) *= inScale[2];
            Get(1, 2) *= inScale[2];
            Get(2, 2) *= inScale[2];
            Get(3, 2) *= inScale[2];
            return *this;
        }

        Matrix4x4& Matrix4x4::Translate(const Vector3& inTrans)
        {
            Get(0, 3) = Get(0, 0) * inTrans[0] + Get(0, 1) * inTrans[1] + Get(0, 2) * inTrans[2] + Get(0, 3);
            Get(1, 3) = Get(1, 0) * inTrans[0] + Get(1, 1) * inTrans[1] + Get(1, 2) * inTrans[2] + Get(1, 3);
            Get(2, 3) = Get(2, 0) * inTrans[0] + Get(2, 1) * inTrans[1] + Get(2, 2) * inTrans[2] + Get(2, 3);
            Get(3, 3) = Get(3, 0) * inTrans[0] + Get(3, 1) * inTrans[1] + Get(3, 2) * inTrans[2] + Get(3, 3);
            return *this;
        }

        Matrix4x4& Matrix4x4::SetPerspective(
                float fovy,
                float aspect,
                float zNear,
                float zFar)
        {
            float cotangent, deltaZ;
            float radians = Deg2Rad * (fovy / 2.0f);
            cotangent = cos(radians) / sin(radians);
            deltaZ = zNear - zFar;

            Get(0, 0) = cotangent / aspect; Get(0, 1) = 0.0F;      Get(0, 2) = 0.0F;                    Get(0, 3) = 0.0F;
            Get(1, 0) = 0.0F;               Get(1, 1) = cotangent; Get(1, 2) = 0.0F;                    Get(1, 3) = 0.0F;
            Get(2, 0) = 0.0F;               Get(2, 1) = 0.0F;      Get(2, 2) = (zFar + zNear) / deltaZ; Get(2, 3) = 2.0F * zNear * zFar / deltaZ;
            Get(3, 0) = 0.0F;               Get(3, 1) = 0.0F;      Get(3, 2) = -1.0F;                   Get(3, 3) = 0.0F;

            return *this;
        }

        Matrix4x4& Matrix4x4::SetPerspectiveCotan(
                float cotangent,
                float zNear,
                float zFar)
        {
            float deltaZ = zNear - zFar;

            Get(0, 0) = cotangent;          Get(0, 1) = 0.0F;      Get(0, 2) = 0.0F;                    Get(0, 3) = 0.0F;
            Get(1, 0) = 0.0F;               Get(1, 1) = cotangent; Get(1, 2) = 0.0F;                    Get(1, 3) = 0.0F;
            Get(2, 0) = 0.0F;               Get(2, 1) = 0.0F;      Get(2, 2) = (zFar + zNear) / deltaZ; Get(2, 3) = 2.0F * zNear * zFar / deltaZ;
            Get(3, 0) = 0.0F;               Get(3, 1) = 0.0F;      Get(3, 2) = -1.0F;                   Get(3, 3) = 0.0F;

            return *this;
        }

        Matrix4x4& Matrix4x4::SetOrtho(
                float left,
                float right,
                float bottom,
                float top,
                float zNear,
                float zFar)
        {
            SetIdentity();

            float deltax = right - left;
            float deltay = top - bottom;
            float deltaz = zFar - zNear;

            Get(0, 0) = 2.0F / deltax;
            Get(0, 3) = -(right + left) / deltax;
            Get(1, 1) = 2.0F / deltay;
            Get(1, 3) = -(top + bottom) / deltay;
            Get(2, 2) = -2.0F / deltaz;
            Get(2, 3) = -(zFar + zNear) / deltaz;
            return *this;
        }

        Matrix4x4& Matrix4x4::SetFrustum(
                float left,
                float right,
                float bottom,
                float top,
                float nearval,
                float farval)
        {
            float x, y, a, b, c, d, e;

            x =  (2.0F * nearval)       / (right - left);
            y =  (2.0F * nearval)       / (top - bottom);
            a =  (right + left)         / (right - left);
            b =  (top + bottom)         / (top - bottom);
            c = -(farval + nearval)        / (farval - nearval);
            d = -(2.0f * farval * nearval) / (farval - nearval);
            e = -1.0f;

            Get(0, 0) = x;    Get(0, 1) = 0.0;  Get(0, 2) = a;   Get(0, 3) = 0.0;
            Get(1, 0) = 0.0;  Get(1, 1) = y;    Get(1, 2) = b;   Get(1, 3) = 0.0;
            Get(2, 0) = 0.0;  Get(2, 1) = 0.0;  Get(2, 2) = c;   Get(2, 3) = d;
            Get(3, 0) = 0.0;  Get(3, 1) = 0.0;  Get(3, 2) = e;  Get(3, 3) = 0.0;
            return *this;
        }

        Matrix4x4& Matrix4x4::AdjustDepthRange(float origNear, float newNear, float newFar)
        {
            if (IsPerspective())
            {
                float x = Get(0, 0);
                float y = Get(1, 1);
                float w = Get(0, 2);
                float z = Get(1, 2);

                float r = ((2.0f * origNear) / x) * ((w + 1) * 0.5f);
                float t = ((2.0f * origNear) / y) * ((z + 1) * 0.5f);
                float l = ((2.0f * origNear) / x) * (((w + 1) * 0.5f) - 1.0f);
                float b = ((2.0f * origNear) / y) * (((z + 1) * 0.5f) - 1.0f);

                float ratio = (newNear / origNear);

                r *= ratio;
                t *= ratio;
                l *= ratio;
                b *= ratio;

                return SetFrustum(l, r, b, t, newNear, newFar);
            }
            else
            {
                float deltaz = newFar - newNear;
                Get(2, 2) = -2.0F / deltaz;
                Get(2, 3) = -(newFar + newNear) / deltaz;
                return *this;
            }
        }

        float ComputeUniformScale(const Matrix4x4& matrix)
        {
            return Vector3::Magnitude(matrix.GetAxisX());
        }


#define SWAP_ROWS(a, b) PP_WRAP_CODE(float *_tmp = a; (a)=(b); (b)=_tmp;)
        bool InvertMatrix4x4_Full(const float* m, float* out) {
            float wtmp[4][8];
            float m0, m1, m2, m3, s;
            float *r0, *r1, *r2, *r3;

            r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

            r0[0] = MAT(m, 0, 0); r0[1] = MAT(m, 0, 1);
            r0[2] = MAT(m, 0, 2); r0[3] = MAT(m, 0, 3);
            r0[4] = 1.0; r0[5] = r0[6] = r0[7] = 0.0;

            r1[0] = MAT(m, 1, 0); r1[1] = MAT(m, 1, 1);
            r1[2] = MAT(m, 1, 2); r1[3] = MAT(m, 1, 3);
            r1[5] = 1.0; r1[4] = r1[6] = r1[7] = 0.0;

            r2[0] = MAT(m, 2, 0); r2[1] = MAT(m, 2, 1);
            r2[2] = MAT(m, 2, 2); r2[3] = MAT(m, 2, 3);
            r2[6] = 1.0; r2[4] = r2[5] = r2[7] = 0.0;

            r3[0] = MAT(m, 3, 0); r3[1] = MAT(m, 3, 1);
            r3[2] = MAT(m, 3, 2); r3[3] = MAT(m, 3, 3);
            r3[7] = 1.0; r3[4] = r3[5] = r3[6] = 0.0;

            if (abs(r3[0]) > abs(r2[0]))
                SWAP_ROWS(r3, r2);
            if (abs(r2[0]) > abs(r1[0]))
                SWAP_ROWS(r2, r1);
            if (abs(r1[0]) > abs(r0[0]))
                SWAP_ROWS(r1, r0);
            if (0.0F == r0[0])
                RETURN_ZERO;

            m1 = r1[0] / r0[0]; m2 = r2[0] / r0[0]; m3 = r3[0] / r0[0];
            s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
            s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
            s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
            s = r0[4];
            if (s != 0.0F)
            {
                r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s;
            }
            s = r0[5];
            if (s != 0.0F)
            {
                r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s;
            }
            s = r0[6];
            if (s != 0.0F)
            {
                r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s;
            }
            s = r0[7];
            if (s != 0.0F)
            {
                r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s;
            }

            if (abs(r3[1]) > abs(r2[1]))
                SWAP_ROWS(r3, r2);
            if (abs(r2[1]) > abs(r1[1]))
                SWAP_ROWS(r2, r1);
            if (0.0F == r1[1])
                RETURN_ZERO;

            m2 = r2[1] / r1[1]; m3 = r3[1] / r1[1];
            r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
            r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
            s = r1[4]; if (0.0F != s)
            {
                r2[4] -= m2 * s; r3[4] -= m3 * s;
            }
            s = r1[5]; if (0.0F != s)
            {
                r2[5] -= m2 * s; r3[5] -= m3 * s;
            }
            s = r1[6]; if (0.0F != s)
            {
                r2[6] -= m2 * s; r3[6] -= m3 * s;
            }
            s = r1[7]; if (0.0F != s)
            {
                r2[7] -= m2 * s; r3[7] -= m3 * s;
            }

            if (abs(r3[2]) > abs(r2[2]))
                SWAP_ROWS(r3, r2);
            if (0.0F == r2[2])
                RETURN_ZERO;

            m3 = r3[2] / r2[2];
            r3[3] -= m3 * r2[3]; r3[4] -= m3 * r2[4];
            r3[5] -= m3 * r2[5]; r3[6] -= m3 * r2[6];
            r3[7] -= m3 * r2[7];

            if (0.0F == r3[3])
                RETURN_ZERO;

            s = 1.0F / r3[3];
            r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

            m2 = r2[3];
            s  = 1.0F / r2[2];
            r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
            r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
            m1 = r1[3];
            r1[4] -= r3[4] * m1; r1[5] -= r3[5] * m1,
                    r1[6] -= r3[6] * m1; r1[7] -= r3[7] * m1;
            m0 = r0[3];
            r0[4] -= r3[4] * m0; r0[5] -= r3[5] * m0,
                    r0[6] -= r3[6] * m0; r0[7] -= r3[7] * m0;

            m1 = r1[2];
            s  = 1.0F / r1[1];
            r1[4] = s * (r1[4] - r2[4] * m1); r1[5] = s * (r1[5] - r2[5] * m1),
                    r1[6] = s * (r1[6] - r2[6] * m1); r1[7] = s * (r1[7] - r2[7] * m1);
            m0 = r0[2];
            r0[4] -= r2[4] * m0; r0[5] -= r2[5] * m0,
                    r0[6] -= r2[6] * m0; r0[7] -= r2[7] * m0;

            m0 = r0[1];
            s  = 1.0F / r0[0];
            r0[4] = s * (r0[4] - r1[4] * m0); r0[5] = s * (r0[5] - r1[5] * m0),
                    r0[6] = s * (r0[6] - r1[6] * m0); r0[7] = s * (r0[7] - r1[7] * m0);

            MAT(out, 0, 0) = r0[4]; MAT(out, 0, 1) = r0[5], MAT(out, 0, 2) = r0[6]; MAT(out, 0, 3) = r0[7];
            MAT(out, 1, 0) = r1[4]; MAT(out, 1, 1) = r1[5], MAT(out, 1, 2) = r1[6]; MAT(out, 1, 3) = r1[7];
            MAT(out, 2, 0) = r2[4]; MAT(out, 2, 1) = r2[5], MAT(out, 2, 2) = r2[6]; MAT(out, 2, 3) = r2[7];
            MAT(out, 3, 0) = r3[4]; MAT(out, 3, 1) = r3[5], MAT(out, 3, 2) = r3[6]; MAT(out, 3, 3) = r3[7];

            return true;
        }

#undef SWAP_ROWS
#undef MAT
#undef RETURN_ZERO

        Matrix4x4& Matrix4x4::Transpose()
        {
            std::swap(Get(0, 1), Get(1, 0));
            std::swap(Get(0, 2), Get(2, 0));
            std::swap(Get(0, 3), Get(3, 0));
            std::swap(Get(1, 2), Get(2, 1));
            std::swap(Get(1, 3), Get(3, 1));
            std::swap(Get(2, 3), Get(3, 2));
            return *this;
        }

        Matrix4x4& Matrix4x4::SetFromToRotation(const Vector3& from, const Vector3& to)
        {
            Matrix3x3 mat;
            mat.SetFromToRotation(from, to);
            *this = mat;
            return *this;
        }

        bool CompareApproximately(const Matrix4x4& lhs, const Matrix4x4& rhs, float dist)
        {
            for (int i = 0; i < 16; i++)
            {
                if (!CompareApproximately(lhs[i], rhs[i], dist))
                    return false;
            }
            return true;
        }

        void Matrix4x4::SetTR(const Vector3& pos, const Quaternion& q)
        {
            QuaternionToMatrix(q, *this);
            m_Data[12] = pos[0];
            m_Data[13] = pos[1];
            m_Data[14] = pos[2];
        }

        void Matrix4x4::SetTRS(const Vector3& pos, const Quaternion& q, const Vector3& s)
        {
            QuaternionToMatrix(q, *this);

            m_Data[0] *= s[0];
            m_Data[1] *= s[0];
            m_Data[2] *= s[0];

            m_Data[4] *= s[1];
            m_Data[5] *= s[1];
            m_Data[6] *= s[1];

            m_Data[8] *= s[2];
            m_Data[9] *= s[2];
            m_Data[10] *= s[2];

            m_Data[12] = pos[0];
            m_Data[13] = pos[1];
            m_Data[14] = pos[2];
        }

        void Matrix4x4::SetTRInverse(const Vector3& pos, const Quaternion& q)
        {
            QuaternionToMatrix(Quaternion::Inverse(q), *this);
            Translate(Vector3(-pos[0], -pos[1], -pos[2]));
        }

        void TransformPoints3x3(const Matrix4x4& matrix, const Vector3* in, Vector3* out, int count)
        {
            auto m = Matrix3x3(matrix);
            for (int i = 0; i < count; i++)
                out[i] = m.MultiplyPoint3(in[i]);
        }

        void TransformPoints3x4(const Matrix4x4& matrix, const Vector3* in, Vector3* out, int count)
        {
            for (int i = 0; i < count; i++)
                out[i] = matrix.MultiplyPoint3(in[i]);
        }

        void TransformPoints3x3(const Matrix4x4& matrix, const Vector3* in, size_t inStride, Vector3* out, size_t outStride, int count)
        {
            auto m = Matrix3x3(matrix);
            for (int i = 0; i < count; ++i, in = Stride(in, inStride), out = Stride(out, outStride))
            {
                *out = m.MultiplyPoint3(*in);
            }
        }

        void TransformPoints3x4(const Matrix4x4& matrix, const Vector3* in, size_t inStride, Vector3* out, size_t outStride, int count)
        {
            for (int i = 0; i < count; ++i, in = Stride(in, inStride), out = Stride(out, outStride))
            {
                *out = matrix.MultiplyPoint3(*in);
            }
        }

        FrustumPlanes Matrix4x4::DecomposeProjection() const {
            FrustumPlanes planes{};

            if (IsPerspective())
            {
                planes.zNear = Get(2, 3) / (Get(2, 2) - 1.0f);
                planes.zFar = Get(2, 3) / (Get(2, 2) + 1.0f);
                planes.right = planes.zNear * (1.0f + Get(0, 2)) / Get(0, 0);
                planes.left = planes.zNear * (-1.0f + Get(0, 2)) / Get(0, 0);
                planes.top = planes.zNear  * (1.0f + Get(1, 2)) / Get(1, 1);
                planes.bottom = planes.zNear  * (-1.0f + Get(1, 2)) / Get(1, 1);
            }
            else
            {
                planes.zNear = (Get(2, 3) + 1.0f) / Get(2, 2);
                planes.zFar =  (Get(2, 3) - 1.0f) / Get(2, 2);
                planes.right = (1.0f - Get(0, 3)) / Get(0, 0);
                planes.left = (-1.0f - Get(0, 3)) / Get(0, 0);
                planes.top = (1.0f - Get(1, 3)) / Get(1, 1);
                planes.bottom = (-1.0f - Get(1, 3)) / Get(1, 1);
            }

            return planes;
        }

        Vector3 Matrix4x4::GetLossyScale() const {

            Vector3 result;
            result.x = Vector3::Magnitude(GetAxisX());
            result.y = Vector3::Magnitude(GetAxisY());
            result.z = Vector3::Magnitude(GetAxisZ());

            float determinant = Matrix3x3(*this).GetDeterminant();
            if (determinant < 0)
                result.x *= -1;

            return result;
        }

        bool Matrix4x4::ValidTRS() const
        {
            return Get(3, 0) == 0 && Get(3, 1) == 0 && Get(3, 2) == 0 && fabs(Get(3, 3)) == 1;
        }

        inline Vector3 Matrix4x4::GetAxisX() const
        {
            return {Get(0, 0), Get(1, 0), Get(2, 0)};
        }

        inline Vector3 Matrix4x4::GetAxisY() const
        {
            return {Get(0, 1), Get(1, 1), Get(2, 1)};
        }

        inline Vector3 Matrix4x4::GetAxisZ() const
        {
            return {Get(0, 2), Get(1, 2), Get(2, 2)};
        }

        inline Vector3 Matrix4x4::GetAxis(int axis) const
        {
            return {Get(0, axis), Get(1, axis), Get(2, axis)};
        }

        inline Vector3 Matrix4x4::GetPosition() const
        {
            return {Get(0, 3), Get(1, 3), Get(2, 3)};
        }

        inline Vector4 Matrix4x4::GetRow(int row) const
        {
            return {Get(row, 0), Get(row, 1), Get(row, 2), Get(row, 3)};
        }

        inline Vector4 Matrix4x4::GetColumn(int col) const
        {
            return {Get(0, col), Get(1, col), Get(2, col), Get(3, col)};
        }

        inline void Matrix4x4::SetAxisX(const Vector3& v)
        {
            Get(0, 0) = v.x; Get(1, 0) = v.y; Get(2, 0) = v.z;
        }

        inline void Matrix4x4::SetAxisY(const Vector3& v)
        {
            Get(0, 1) = v.x; Get(1, 1) = v.y; Get(2, 1) = v.z;
        }

        inline void Matrix4x4::SetAxisZ(const Vector3& v)
        {
            Get(0, 2) = v.x; Get(1, 2) = v.y; Get(2, 2) = v.z;
        }

        inline void Matrix4x4::SetAxis(int axis, const Vector3& v)
        {
            Get(0, axis) = v.x; Get(1, axis) = v.y; Get(2, axis) = v.z;
        }

        inline void Matrix4x4::SetPosition(const Vector3& v)
        {
            Get(0, 3) = v.x; Get(1, 3) = v.y; Get(2, 3) = v.z;
        }

        inline void Matrix4x4::SetRow(int row, const Vector4& v)
        {
            Get(row, 0) = v.x; Get(row, 1) = v.y; Get(row, 2) = v.z; Get(row, 3) = v.w;
        }

        inline void Matrix4x4::SetColumn(int col, const Vector4& v)
        {
            Get(0, col) = v.x; Get(1, col) = v.y; Get(2, col) = v.z; Get(3, col) = v.w;
        }

        inline Vector3 Matrix4x4::MultiplyPoint3(const Vector3& v) const
        {
            Vector3 res;
            res.x = m_Data[0] * v.x + m_Data[4] * v.y + m_Data[8] * v.z + m_Data[12];
            res.y = m_Data[1] * v.x + m_Data[5] * v.y + m_Data[9] * v.z + m_Data[13];
            res.z = m_Data[2] * v.x + m_Data[6] * v.y + m_Data[10] * v.z + m_Data[14];
            return res;
        }

        inline void Matrix4x4::MultiplyPoint3(const Vector3& v, Vector3& output) const
        {
            output.x = m_Data[0] * v.x + m_Data[4] * v.y + m_Data[8] * v.z + m_Data[12];
            output.y = m_Data[1] * v.x + m_Data[5] * v.y + m_Data[9] * v.z + m_Data[13];
            output.z = m_Data[2] * v.x + m_Data[6] * v.y + m_Data[10] * v.z + m_Data[14];
        }

        inline Vector2 Matrix4x4::MultiplyPoint2(const Vector2& v) const
        {
            Vector2 res;
            res.x = m_Data[0] * v.x + m_Data[4] * v.y + m_Data[12];
            res.y = m_Data[1] * v.x + m_Data[5] * v.y + m_Data[13];
            return res;
        }

        inline void Matrix4x4::MultiplyPoint2(const Vector2& v, Vector2& output) const
        {
            output.x = m_Data[0] * v.x + m_Data[4] * v.y + m_Data[12];
            output.y = m_Data[1] * v.x + m_Data[5] * v.y + m_Data[13];
        }

        inline Vector3 Matrix4x4::MultiplyVector3(const Vector3& v) const
        {
            Vector3 res;
            res.x = m_Data[0] * v.x + m_Data[4] * v.y + m_Data[8] * v.z;
            res.y = m_Data[1] * v.x + m_Data[5] * v.y + m_Data[9] * v.z;
            res.z = m_Data[2] * v.x + m_Data[6] * v.y + m_Data[10] * v.z;
            return res;
        }

        inline void Matrix4x4::MultiplyVector3(const Vector3& v, Vector3& output) const
        {
            output.x = m_Data[0] * v.x + m_Data[4] * v.y + m_Data[8] * v.z;
            output.y = m_Data[1] * v.x + m_Data[5] * v.y + m_Data[9] * v.z;
            output.z = m_Data[2] * v.x + m_Data[6] * v.y + m_Data[10] * v.z;
        }

        inline bool Matrix4x4::PerspectiveMultiplyPoint3(const Vector3& v, Vector3& output) const
        {
            Vector3 res;
            float w;
            res.x = Get(0, 0) * v.x + Get(0, 1) * v.y + Get(0, 2) * v.z + Get(0, 3);
            res.y = Get(1, 0) * v.x + Get(1, 1) * v.y + Get(1, 2) * v.z + Get(1, 3);
            res.z = Get(2, 0) * v.x + Get(2, 1) * v.y + Get(2, 2) * v.z + Get(2, 3);
            w     = Get(3, 0) * v.x + Get(3, 1) * v.y + Get(3, 2) * v.z + Get(3, 3);
            if (abs(w) > 1.0e-7f)
            {
                float invW = 1.0f / w;
                output.x = res.x * invW;
                output.y = res.y * invW;
                output.z = res.z * invW;
                return true;
            }
            else
            {
                output.x = 0.0f;
                output.y = 0.0f;
                output.z = 0.0f;
                return false;
            }
        }

        inline Vector4 Matrix4x4::MultiplyVector4(const Vector4& v) const
        {
            Vector4 res;
            MultiplyVector4(v, res);
            return res;
        }

        inline void Matrix4x4::MultiplyVector4(const Vector4& v, Vector4& output) const
        {
            output.x = m_Data[0] * v.x + m_Data[4] * v.y + m_Data[8] * v.z + m_Data[12] * v.w;
            output.y = m_Data[1] * v.x + m_Data[5] * v.y + m_Data[9] * v.z + m_Data[13] * v.w;
            output.z = m_Data[2] * v.x + m_Data[6] * v.y + m_Data[10] * v.z + m_Data[14] * v.w;
            output.w = m_Data[3] * v.x + m_Data[7] * v.y + m_Data[11] * v.z + m_Data[15] * v.w;
        }

        inline bool Matrix4x4::PerspectiveMultiplyVector3(const Vector3& v, Vector3& output) const
        {
            Vector3 res;
            float w;
            res.x = Get(0, 0) * v.x + Get(0, 1) * v.y + Get(0, 2) * v.z;
            res.y = Get(1, 0) * v.x + Get(1, 1) * v.y + Get(1, 2) * v.z;
            res.z = Get(2, 0) * v.x + Get(2, 1) * v.y + Get(2, 2) * v.z;
            w     = Get(3, 0) * v.x + Get(3, 1) * v.y + Get(3, 2) * v.z;
            if (abs(w) > 1.0e-7f)
            {
                float invW = 1.0f / w;
                output.x = res.x * invW;
                output.y = res.y * invW;
                output.z = res.z * invW;
                return true;
            }
            else
            {
                output.x = 0.0f;
                output.y = 0.0f;
                output.z = 0.0f;
                return false;
            }
        }

        inline Vector3 Matrix4x4::InverseMultiplyPoint3Affine(const Vector3& inV) const
        {
            Vector3 v(inV.x - Get(0, 3), inV.y - Get(1, 3), inV.z - Get(2, 3));
            Vector3 res;
            res.x = Get(0, 0) * v.x + Get(1, 0) * v.y + Get(2, 0) * v.z;
            res.y = Get(0, 1) * v.x + Get(1, 1) * v.y + Get(2, 1) * v.z;
            res.z = Get(0, 2) * v.x + Get(1, 2) * v.y + Get(2, 2) * v.z;
            return res;
        }

        inline Vector3 Matrix4x4::InverseMultiplyVector3Affine(const Vector3& v) const
        {
            Vector3 res;
            res.x = Get(0, 0) * v.x + Get(1, 0) * v.y + Get(2, 0) * v.z;
            res.y = Get(0, 1) * v.x + Get(1, 1) * v.y + Get(2, 1) * v.z;
            res.z = Get(0, 2) * v.x + Get(1, 2) * v.y + Get(2, 2) * v.z;
            return res;
        }
        inline Matrix4x4 Matrix4x4::Rotate(Quaternion q) { // Из C#
            float x = q.x * 2.0f;
            float y = q.y * 2.0f;
            float z = q.z * 2.0f;
            float xx = q.x * x;
            float yy = q.y * y;
            float zz = q.z * z;
            float xy = q.x * y;
            float xz = q.x * z;
            float yz = q.y * z;
            float wx = q.w * x;
            float wy = q.w * y;
            float wz = q.w * z;

            Matrix4x4 m;
            m.m_Data[0] = 1.0f - (yy + zz); m.m_Data[1] = xy + wz; m.m_Data[2] = xz - wy; m.m_Data[3] = 0.0f;
            m.m_Data[4] = xy - wz; m.m_Data[5] = 1.0f - (xx + zz); m.m_Data[6] = yz + wx; m.m_Data[7] = 0.0f;
            m.m_Data[8] = xz + wy; m.m_Data[9] = yz - wx; m.m_Data[10] = 1.0f - (xx + yy); m.m_Data[11] = 0.0f;
            m.m_Data[12] = 0.0f; m.m_Data[13] = 0.0f; m.m_Data[14] = 0.0F; m.m_Data[15] = 1.0f;
            return m;
        }
    }

    namespace HexUtils {

        // Перевернуть шестнадцатеричную строку (из 001122 в 221100)
        std::string ReverseHexString(const std::string &hex) {
            std::string out;
            for (unsigned int i = 0; i < hex.length(); i += 2) out.insert(0, hex.substr(i, 2));
            return out;
        }

        // Удалить пробелы и 0x
        std::string FixHexString(std::string str) {
            std::string::size_type tmp;
            if (str.find(OBFUSCATE_BNM("0x")) != -1) {
                tmp = str.find(OBFUSCATE_BNM("0x"));
                str.replace(tmp, 2, OBFUSCATE_BNM(""));
            }
            for (int i = (int)str.length() - 1; i >= 0; --i)
                if (str[i] == ' ') str.erase(i, 1);
            return str;
        }

        // Конвертировать шестнадцатеричную строку в значение
        BNM_PTR HexStr2Value(const std::string &hex) { return strtoull(hex.c_str(), nullptr, 16); }

#if defined(__ARM_ARCH_7A__)

        // Проверить, является ли ассемблер `bl ...` или `b ...`
        bool IsBranchHex(const std::string &hex) {
            BNM_PTR hexW = HexStr2Value(ReverseHexString(FixHexString(hex)));
            return (hexW & 0x0A000000) == 0x0A000000;
        }

#elif defined(__aarch64__)

        // Проверить, является ли ассемблер `bl ...` или `b ...`
        bool IsBranchHex(const std::string &hex) {
            BNM_PTR hexW = HexStr2Value(ReverseHexString(FixHexString(hex)));
            return (hexW & 0xFC000000) == 0x14000000 || (hexW & 0xFC000000) == 0x94000000;
        }

#elif defined(__i386__) || defined(__x86_64__)

        // Проверить, является ли ассемблер `call ...`
        bool IsCallHex(const std::string &hex) { return hex.substr(0, 2) == OBFUSCATE_BNM("E8"); }

#else
#error "BNM поддерживает только arm64, arm, x86 и x86_64"
#endif

        // Прочитать память, как шестнадцатеричную строку
        std::string ReadMemory(BNM_PTR address, size_t len) {
            char temp[len];
            memset(temp, 0, len);
            const size_t bufferLen = len * 2 + 1;
            char buffer[bufferLen];
            memset(buffer, 0, bufferLen);
            std::string ret;
            if (memcpy(temp, (void *)address, len) == nullptr) return ret;
            for (int i = 0; i < len; ++i)
                sprintf(&buffer[i * 2], OBFUSCATE_BNM("%02X"), (unsigned char) temp[i]);
            ret += buffer;
            return ret;
        }

        // Декодировать b или bl и получить адрес, по которому он переходит
        bool DecodeBranchOrCall(const std::string &hex, BNM_PTR offset, BNM_PTR &outOffset) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
            if (!IsBranchHex(hex)) return false;
#if defined(__aarch64__)
            int add = 0;
#else
            int add = 8;
#endif
            // Эта строка основана на коде capstone
            outOffset = ((int32_t)(((((HexStr2Value(ReverseHexString(FixHexString(hex)))) & (((uint32_t)1 << 24) - 1) << 0) >> 0) << 2) << (32 - 26)) >> (32 - 26)) + offset + add;
#elif defined(__i386__) || defined(__x86_64__)
            if (!IsCallHex(hex)) return false;
            // Адрес + адрес из `call` + размер инструкции
            outOffset = offset + HexStr2Value(ReverseHexString(FixHexString(hex)).substr(0, 8)) + 5;
#else
#error "BNM поддерживает только arm64, arm, x86 и x86_64"
            return false;
#endif
            return true;
        }

        // Идёт по памяти и пытается найти b-, bl- или call-инструкции
        // Потом получает адрес, по которому они переходят
        // index: 0 - первый, 1 - второй и т. д.
        BNM_PTR FindNextJump(BNM_PTR start, int index) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
            int offset = 0;
            std::string curHex = ReadMemory(start, 4);
            BNM_PTR outOffset = 0;
            bool out;
            while (!(out = DecodeBranchOrCall(curHex, start + offset, outOffset)) || index != 1) {
                offset += 4;
                curHex = ReadMemory(start + offset, 4);
                if (out) index--;
            }
            return outOffset;
#elif defined(__i386__) || defined(__x86_64__)
            int offset = 0;
            std::string curHex = ReadMemory(start, 1);
            BNM_PTR outOffset = 0;
            bool out;
            while (!(out = IsCallHex(curHex)) || index != 1) {
                offset += 1;
                curHex = ReadMemory(start + offset, 1);
                if (out) index--;
            }
            DecodeBranchOrCall(ReadMemory(start + offset, 5), start + offset, outOffset);
            return outOffset;
#endif
        }
    }
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
    namespace MODIFY_CLASSES {

        TargetClass::TargetClass() noexcept = default;
        AddableMethod::AddableMethod() noexcept = default;
        AddableField::AddableField() noexcept = default;

        void AddTargetClass(TargetClass *klass) {
            if (!BNM_Internal::classes4Mod)
                BNM_Internal::classes4Mod = new std::vector<TargetClass *>();
            // Добавить класс ко всем целевым классам
            BNM_Internal::classes4Mod->push_back(klass);
        }
    }

    namespace NEW_CLASSES {

        NewMethod::NewMethod() noexcept = default;
        NewField::NewField() noexcept = default;
        NewClass::NewClass() noexcept = default;

        void AddNewClass(NewClass *klass) {
            if (!BNM_Internal::classes4Add)
                BNM_Internal::classes4Add = new std::vector<NewClass *>();
            // Добавить класс ко всем созданным классам
            BNM_Internal::classes4Add->push_back(klass);
        }
    }
#endif

    [[maybe_unused]] void HardBypass(JNIEnv *env, jobject context) {
        if (!env || BNM_Internal::dlLib || BNM_Internal::hardBypass) return;
        BNM_Internal::hardBypass = true;

        // Получить путь до папки libs, используя JNI
        if (!context) {
            jclass activityThread = env->FindClass(OBFUSCATE_BNM("android/app/ActivityThread"));
            context = env->CallObjectMethod(env->CallStaticObjectMethod(activityThread, env->GetStaticMethodID(activityThread, OBFUSCATE_BNM("currentActivityThread"), OBFUSCATE_BNM("()Landroid/app/ActivityThread;"))), env->GetMethodID(activityThread, OBFUSCATE_BNM("getApplication"), OBFUSCATE_BNM("()Landroid/app/Application;")));
        }
        auto appInfo = env->CallObjectMethod(context, env->GetMethodID(env->GetObjectClass(context), OBFUSCATE_BNM("getApplicationInfo"), OBFUSCATE_BNM("()Landroid/content/pm/ApplicationInfo;")));
        std::string path = env->GetStringUTFChars((jstring)env->GetObjectField(appInfo, env->GetFieldID(env->GetObjectClass(appInfo), OBFUSCATE_BNM("nativeLibraryDir"), OBFUSCATE_BNM("Ljava/lang/String;"))), nullptr);

        // Попробовать загрузить il2cpp по этому пути
        auto libPath = str2char(path + OBFUSCATE_BNM("/libil2cpp.so"));
        auto dl = BNM_dlopen(libPath, RTLD_LAZY);
        if (!BNM_Internal::InitDlLib(dl, libPath)) LOGEBNM(OBFUSCATE_BNM("Тяжёлый обход не удался!"));
    }

    namespace External {
        [[maybe_unused]] bool TryInitDl(void *dl, const char *path, bool external) {
            return BNM_Internal::InitDlLib(dl, path, external);
        }
        [[maybe_unused]] void LoadBNM(void *dl) {
            if (BNM_Internal::InitDlLib(dl, nullptr, true)) {
                BNM_Internal::InitIl2cppMethods(); // Найти некоторые методы и подменить их
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
                BNM_Internal::InitNewClasses(); // Создать новые классы и добавить их в il2cpp
                BNM_Internal::ModifyClasses(); // Изменить существующие классы (ОПАСНО при загрузке извне)
#endif
                BNM_Internal::LibLoaded = true; // Установить LibLoaded в значение "истина"
            } else LOGWBNM(OBFUSCATE_BNM("[External::LoadBNM] dl мёртв или неверен, BNM не загружен!"));
        }
        [[maybe_unused]] void ForceLoadBNM(void *dl) {
            BNM_Internal::dlLib = dl; // Насильно установить dl
            BNM_Internal::InitIl2cppMethods(); // Найти некоторые методы и подменить их
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
            BNM_Internal::InitNewClasses(); // Создать новые классы и добавить их в il2cpp
            BNM_Internal::ModifyClasses(); // Изменить существующие классы (ОПАСНО при загрузке извне)
#endif
            BNM_Internal::LibLoaded = true; // Установить LibLoaded в значение "истина"
        }
    }
    
    void AddOnLoadedEvent(void (*event)()) {
        if (event) BNM_Internal::onIl2CppLoaded.push_back(event);
    }
    
    void ClearOnLoadedEvents() {
        BNM_Internal::onIl2CppLoaded.clear();
    }
}
namespace BNM_Internal {
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
    struct { // Структура для быстрого поиска классов по их образам

        // Добавить класс к образу
        void addClass(IL2CPP::Il2CppImage *image, IL2CPP::Il2CppClass *cls) {
            return addClass((BNM_PTR)image, cls);
        }
        void addClass(BNM_PTR image, IL2CPP::Il2CppClass *cls) {
            std::shared_lock lock(classesFindAccess);
            map[image].emplace_back(cls);
        }

        // Перебрать все классы, добавленные к образу
        void forEachByImage(IL2CPP::Il2CppImage *image, const std::function<bool(IL2CPP::Il2CppClass *)> &func) {
            std::shared_lock lock(classesFindAccess);
            return forEachByImage((BNM_PTR)image, func);
        }
        void forEachByImage(BNM_PTR image, const std::function<bool(IL2CPP::Il2CppClass *)> &func) {
            std::shared_lock lock(classesFindAccess);
            if (map[image].empty()) return;
            for (auto &item: map[image]) {
                if (func(item))
                    break;
            }
        }

        // Перебрать все образы с их классами
        void forEach(const std::function<bool(IL2CPP::Il2CppImage *, std::vector<IL2CPP::Il2CppClass *>)> &func) {
            std::shared_lock lock(classesFindAccess);
            for (auto [img, classes] : map) {
                if (func((IL2CPP::Il2CppImage *)img, classes))
                    break;
            }
        }
    private:
        std::map<BNM_PTR, std::vector<IL2CPP::Il2CppClass *>> map;
    } BNMClassesMap;

#define BNM_I2C_NEW(type, ...) new IL2CPP::type(__VA_ARGS__)

    // Создать новый образ
    IL2CPP::Il2CppImage *makeImage(NEW_CLASSES::NewClass *cls) {

        // Создать новый образ
        auto newImg = BNM_I2C_NEW(Il2CppImage);

        // Создать имя без .dll
        auto dllName = cls->GetDllName();
        auto nameLen = strlen(dllName) + 1;
        newImg->nameNoExt = (char *) malloc(sizeof(char) * nameLen);
        memset((void *)newImg->nameNoExt, 0, nameLen);
        strcpy((char *)newImg->nameNoExt, dllName);


        // Создать имя с .dll
        auto extLen = nameLen + 4;
        newImg->name = (char *) malloc(sizeof(char) * extLen);
        memset((void *)newImg->name, 0, extLen);
        strcpy((char *)newImg->name, dllName);
        strcat((char *)newImg->name, OBFUSCATE_BNM(".dll"));

#if UNITY_VER > 182
        newImg->assembly = nullptr;
        newImg->customAttributeCount = 0;
#if UNITY_VER < 201
        newImg->customAttributeStart = -1;
#endif
#endif

#if UNITY_VER > 201

        // Создать пустой Il2CppImageDefinition
        auto handle = (IL2CPP::Il2CppImageDefinition *)malloc(sizeof(IL2CPP::Il2CppImageDefinition));
        handle->typeStart = -1;
        handle->exportedTypeStart = -1;
        handle->typeCount = 0;
        handle->exportedTypeCount = 0;
        handle->nameIndex = -1;
        handle->assemblyIndex = -1;
        handle->customAttributeCount = 0;
        handle->customAttributeStart = -1;
        newImg->metadataHandle = (decltype(newImg->metadataHandle))handle;
#else
        // -1 для отключения анализа от il2cpp
        newImg->typeStart = -1;
        newImg->exportedTypeStart = -1;
#endif
        // Инициализировать переменные
        newImg->typeCount = 0;
        newImg->exportedTypeCount = 0;
        newImg->token = 1;

        // Создать новую сборку для образа
        auto newAsm = BNM_I2C_NEW(Il2CppAssembly);
#if UNITY_VER > 174
        // Установить образ и сборку
        newAsm->image = newImg;
        newAsm->image->assembly = newAsm;
        newAsm->aname.name = newImg->name;
#else
        // Отрицательное значение для отключения анализа от il2cpp
        static int newAsmCount = 1;
        newImg->assemblyIndex = newAsm->imageIndex = -newAsmCount;
        newAsmCount++;
#endif
        // Инициализировать эти переменные на всякий случай
        newAsm->aname.major = 0;
        newAsm->aname.minor = 0;
        newAsm->aname.build = 0;
        newAsm->aname.revision = 0;
        newAsm->referencedAssemblyStart = -1;
        newAsm->referencedAssemblyCount = 0;

        // Используя это, BNM может проверить, создана ли им эта сборка
        newImg->nameToClassHashTable = (decltype(newImg->nameToClassHashTable)) -0x424e4d;

        // Добавить сборку в список
        Assembly$$GetAllAssemblies()->push_back(newAsm);

        LOGIBNM(OBFUSCATE_BNM("Добавлена новая сборка: [%s]."), dllName);

        return newImg;
    }

    // Проверка, является ли тип класса созданным BNM
    bool isBNMType(IL2CPP::Il2CppType *type) {
        if (CheckObj(type->data.dummy)) return ((BNMTypeData *)type->data.dummy)->bnm == -0x424e4d;
        return false;
    }

    // Подмена `FromIl2CppType`, чтобы предотвратить вылет il2cpp при попытке получить класс из типа, созданного BNM
    IL2CPP::Il2CppClass *Class$$FromIl2CppType(IL2CPP::Il2CppType *type) {
        // Проверить, верен ли тип
        if (!type) return nullptr;

        // Проверить, создан ли тип BNM
        if (isBNMType(type)) return ((BNMTypeData *)type->data.dummy)->cls;

        return old_Class$$FromIl2CppType(type);
    }

    // Подмена `GetClassOrElementClass`, чтобы предотвратить вылет il2cpp при попытке unity загрузить пакет с полем класс которого, создан BNM
    IL2CPP::Il2CppClass *Type$$GetClassOrElementClass(IL2CPP::Il2CppType *type) {
        // Проверить, верен ли тип
        if (!type) return nullptr;

        // Проверить, создан ли тип BNM
        if (isBNMType(type)) return ((BNMTypeData *) type->data.dummy)->cls;

        return old_Type$$GetClassOrElementClass(type);
    }

    // Подмена `FromName`, чтобы предотвратить вылет il2cpp при попытке найти класс, созданный BNM
    IL2CPP::Il2CppClass *Class$$FromName(IL2CPP::Il2CppImage *image, const char *namespaze, const char *name) {
        // Проверить, верен ли образ
        if (!image) return nullptr;

        IL2CPP::Il2CppClass *ret = nullptr;

        // Проверить, создан ли образ BNM
        if (image->nameToClassHashTable != (decltype(image->nameToClassHashTable))-0x424e4d)
            ret = old_Class$$FromName(image, namespaze, name);

        // Если через BNM, ищем класс
        if (!ret) BNMClassesMap.forEachByImage(image, [namespaze, name, &ret](IL2CPP::Il2CppClass *BNM_class) -> bool {
                if (!strcmp(namespaze, BNM_class->namespaze) && !strcmp(name, BNM_class->name)) {
                    ret = BNM_class;
                    // Найдено, останавливаем for
                    return true;
                }
                return false;
            });

        // Вернуть ничего или найденный класс
        return ret;
    }

    // Требуются, потому что в Unity 2017 и ниже в образах и сборках они хранятся по номерам
#if UNITY_VER <= 174
    IL2CPP::Il2CppImage *new_GetImageFromIndex(IL2CPP::ImageIndex index) {

        // Номер меньше 0, значит, это сборка BNM
        if (index < 0) {
            IL2CPP::Il2CppImage *ret = nullptr;

            // Перебрать все образы и проверить, совпадает ли номер
            BNMClassesMap.forEach([index, &ret](IL2CPP::Il2CppImage *img, const std::vector<IL2CPP::Il2CppClass *> &classes) -> bool {
                if (img->assemblyIndex == index) {
                    ret = img;
                    return true;
                }
                return false;
            });

            // Вернуть ничего или найденный образ
            return ret;
        }

        return old_GetImageFromIndex(index);
    }
    // Все запросы перенаправляются в BNM и обрабатываются им
    // В Unity 2017 и ниже имена хранятся, как номер в метаданных, поэтому мы не можем их использовать
    // Но мы можем проверить название по образам
    IL2CPP::Il2CppAssembly *Assembly$$Load(const char *name) {
        auto LoadedAssemblies = Assembly$$GetAllAssemblies();

        for (auto assembly : *LoadedAssemblies) {

            // Получить образ для сборки
            auto image = new_GetImageFromIndex(assembly->imageIndex);

            // Проверить, совпадают ли имена
            if (!strcmp(image->name, name) || !strcmp(image->nameNoExt, name)) return assembly;
        }

        // Ничего не найдено
        return nullptr;
    }
#endif
    void ModifyClasses() {
        std::lock_guard<std::mutex> lock(modClass);
        if (classes4Mod == nullptr) return;
        for (auto& klass : *classes4Mod) {
            auto lc = klass->GetTargetType().ToLC();
            auto fields4Add = klass->GetFields();
            auto methods4Add = klass->GetMethods();

            if (!lc) {
                LOGWBNM("[ModifyClasses] Не удалось добавить %d методов и %d полей к классу.", methods4Add.size(), fields4Add.size());
                continue;
            }

            auto cls = lc.GetIl2CppClass();
            auto newMethodsCount = methods4Add.size();
            auto newFieldsCount = fields4Add.size();
            auto targetModifier = klass->GetTargetModifier();

            // Заменить родителя, если нужно
            if (targetModifier.newParentGetter)
                cls->parent = targetModifier.newParentGetter().ToIl2CppClass();

            if (newMethodsCount) {
                auto oldCount = cls->method_count;
                auto oldMethods = cls->methods;

                // Создать новый список методов
                auto newMethods = (IL2CPP::MethodInfo **) calloc(oldCount + newMethodsCount, sizeof(IL2CPP::MethodInfo *));

                // Копировать старые методы, если есть
                if (oldCount) memcpy(newMethods, oldMethods, oldCount * sizeof(IL2CPP::MethodInfo *));

                // Создать и добавить все методы
                for (size_t i = 0; i < newMethodsCount; ++i, ++oldCount) {
                    auto &method = methods4Add[i];

                    // Создать новый MethodInfo
                    method->myInfo = BNM_I2C_NEW(MethodInfo);

                    // Установить адрес метода
                    method->myInfo->methodPointer = (IL2CPP::Il2CppMethodPointer)method->GetAddress();

                    // Установить адрес инициатора вызова метода
                    method->myInfo->invoker_method = (IL2CPP::InvokerMethod)method->GetInvoker();

                    // Установить кол-во аргументов
                    method->myInfo->parameters_count = method->GetArgsCount();

                    // Создать имя метода
                    auto name = method->GetName();
                    auto len = strlen(name) + 1;
                    method->myInfo->name = (char *) malloc(sizeof(char) * len);
                    memset((void *)method->myInfo->name, 0, len);
                    strcpy((char *)method->myInfo->name, name);

                    // Установить флаги метода
                    method->myInfo->flags = 0x0006 | 0x0080; // PUBLIC | HIDE_BY_SIG
                    if (method->IsStatic()) method->myInfo->flags |= 0x0010; // |= STATIC

                    // BNM не поддерживает generic-методы
                    method->myInfo->is_generic = false;

                    // Установить возвращаемый тип
                    method->myInfo->return_type = method->GetRetType().ToIl2CppType();

                    // Создать аргументы
                    auto argsCount = method->GetArgsCount();
                    if (argsCount) {
                        auto types = method->GetArgTypes();
#if UNITY_VER < 212
                        // Создать новый массив аргументов
                        method->myInfo->parameters = (IL2CPP::ParameterInfo *)calloc(argsCount, sizeof(IL2CPP::ParameterInfo));

                        // Получить первый аргумент
                        auto newParam = (IL2CPP::ParameterInfo *)method->myInfo->parameters;

                        // Создать аргументы
                        for (int p = 0; p < argsCount; ++p) {

                            // Получить длину нового имени
                            len = (OBFUSCATES_BNM("аргумент") + std::to_string(p)).size();

                            // Создать имя
                            newParam->name = (char *) malloc(sizeof(char) * len + 1);
                            memset((void *)newParam->name, 0, len + 1);
                            strcat((char *)newParam->name, OBFUSCATE_BNM("аргумент")); // Перевод чтобы был
                            strcpy((char *)newParam->name, std::to_string(p).c_str());

                            // Установить номер аргумента
                            newParam->position = p;

                            // Попытаться установить тип аргумента
                            if (!types.empty() && p < types.size()) {
                                newParam->parameter_type = types[p].ToIl2CppType();
                                newParam->token = newParam->parameter_type->attrs | p;
                            } else {
                                newParam->parameter_type = nullptr;
                                newParam->token = p;
                            }
                            // Следующий!
                            ++newParam;
                        }
#else
                        // Создать новый маcсив типов аргументов
                        auto **params = new IL2CPP::Il2CppType*[argsCount];

                        // Установить маcсив
                        method->myInfo->parameters = (const IL2CPP::Il2CppType **)params;

                        // Создать аргументы
                        for (int p = 0; p < argsCount; ++p) {

                            // Создать тип
                            auto newParam = BNM_I2C_NEW(Il2CppType);

                            // Копировать, если можно
                            if (p < types.size())
                                memcpy(newParam, types[p].ToIl2CppType(), sizeof(IL2CPP::Il2CppType));

                            // Установить
                            params[p] = newParam;
                        }
#endif
                    }
#if UNITY_VER >= 212
                    else {
                        // Мы не можем без лишнего кода установить имена аргументов, потому что они перемещены в метаданные
                        method->myInfo->methodMetadataHandle = nullptr;
                    }
#endif
                    newMethods[oldCount] = method->myInfo;
                    LOGDBNM(OBFUSCATE_BNM("[ModifyClasses] Добавлен %sметод %s %d к %s."), method->IsStatic() ? OBFUSCATE_BNM("статический ") : OBFUSCATE_BNM(""), name, argsCount, lc.str().c_str());
                }

                // Очистить methods4Add
                methods4Add.clear(); methods4Add.shrink_to_fit();

                // Заменить список методов
                cls->methods = (const IL2CPP::MethodInfo **)newMethods;

                // Увеличить кол-во методов
                cls->method_count += newMethodsCount;
            }

            if (newFieldsCount) {
                // Данные для статических полей
                int newStaticFieldsSize = 0;

                auto oldCount = cls->field_count;

                // Создать новый список полей
                auto newFields = (IL2CPP::FieldInfo *)calloc(oldCount + newFieldsCount, sizeof(IL2CPP::FieldInfo));

                // Копировать старые поля, если есть
                if (oldCount) memcpy(newFields, cls->fields, oldCount * sizeof(IL2CPP::FieldInfo));

                // Адрес новых полей
                auto currentAddress = (int) cls->instance_size;

                // Получить первое поле
                IL2CPP::FieldInfo *newField = newFields + oldCount;
                for (auto &field : fields4Add) {
                    // Создать имя
                    auto name = field->GetName();
                    auto len = strlen(name) + 1;
                    newField->name = (char *) malloc(sizeof(char) * len);
                    memset((void *)newField->name, 0, len);
                    strcpy((char *)newField->name, name);

                    // Копировать тип
                    newField->type = BNM_I2C_NEW(Il2CppType, *field->GetType().ToIl2CppType());

                    // Добавить флаги видимости
                    ((IL2CPP::Il2CppType*)newField->type)->attrs |= 0x0006; // PUBLIC

                    // Создать знак (token)
                    newField->token = newField->type->attrs | 0x0006;

                    // Установить новый класс
                    newField->parent = cls;

                    // Установить адрес
                    field->offset = newField->offset = currentAddress;

                    // Получить адрес следующего поля
                    currentAddress += field->GetSize();

                    // Установить информацию
                    field->myInfo = newField;

                    // Следующий!
                    newField++;
                    LOGDBNM(OBFUSCATE_BNM("[ModifyClasses] Добавлено поле %s к %s."), name, lc.str().c_str());
                }

                // Очистить fields4Add
                fields4Add.clear(); fields4Add.shrink_to_fit();

                // Заменить список полей
                cls->fields = newFields;

                // Увеличить кол-во полей
                cls->field_count += newFieldsCount;

                // Увеличить размер класса
                cls->instance_size = (uint) currentAddress;

                if (newStaticFieldsSize) {
                    auto oldSize = cls->static_fields_size;

                    // Создать новые данные статических полей
                    void *newStaticFields = malloc(oldSize + newStaticFieldsSize);

                    // Копировать старые данные статических полей, если есть
                    if (oldSize) memcpy(cls->static_fields, newStaticFields, oldSize);

                    // Увеличить размер данных статических полей
                    cls->static_fields_size += newStaticFieldsSize;

                    // Заменить данные статических полей
                    cls->static_fields = newStaticFields;
                }
            }
            LOGIBNM(OBFUSCATE_BNM("[ModifyClasses] Класс %s успешно изменён."), lc.str().c_str());
        }

        // Очистить classes4Mod
        classes4Mod->clear(); classes4Mod->shrink_to_fit();
        delete classes4Mod;
        classes4Mod = nullptr;
    }
    bool hasInterface(IL2CPP::Il2CppClass *parent, IL2CPP::Il2CppClass *interface) {
        for (auto i = 0; i < parent->interfaces_count; ++i) if (parent->implementedInterfaces[i] == interface) return true;
        if (parent->parent) return hasInterface(parent->parent, interface);
        return false;
    }
    void InitNewClasses() {
        std::lock_guard<std::mutex> lock(addClass);
        if (!classes4Add) return;
        DO_API(bool, il2cpp_type_equals, (const IL2CPP::Il2CppType * type, const IL2CPP::Il2CppType * otherType));
        DO_API(IL2CPP::Il2CppImage *, il2cpp_assembly_get_image, (const IL2CPP::Il2CppAssembly *));
        for (auto& klass : *classes4Add) {
            IL2CPP::Il2CppImage *curImg = nullptr;

            auto className = klass->GetName();
            auto classNamespace = klass->GetNamespace();

            // Проверка, существует ли уже класс в il2cpp
            {
                LoadClass existLS;


                auto assemblies = BNM_Internal::Assembly$$GetAllAssemblies();

                // Попробовать найти образ
                auto dllName = klass->GetDllName();
                for (auto assembly : *assemblies) {
                    auto image = il2cpp_assembly_get_image(assembly);

                    if (strcmp(dllName, image->nameNoExt) != 0) continue;

                    curImg = image;
                    break;

                }

                // Проверить, найден ли образ
                if (curImg) {

                    // Получить все классы образа
                    ClassVector classes;
                    BNM_Internal::Image$$GetTypes(curImg, false, &classes);

                    // Попробовать найти класс
                    for (auto cls : classes) {
                        if (!cls) continue;

                        // Попробовать инициализировать класс
                        BNM_Internal::Class$$Init(cls);

                        // Проверить, существует ли новый класс уже
                        if (strcmp(cls->name, className) != 0 || strcmp(cls->namespaze, classNamespace) != 0) continue;

                        existLS = cls;
                        break;
                    }

                    // Очистить список классов
                    classes.clear(); classes.shrink_to_fit();

                } else curImg = makeImage(klass); // Создать новый образ для нового класса

                // Если существует, предупредить и установить класс и тип
                if (existLS) {
                    LOGWBNM(OBFUSCATE_BNM("Класс %s уже существует, он не может быть добавлен в il2cpp! Пожалуйста, проверьте код."), existLS.str().c_str());
                    // На всякий
                    klass->myClass = existLS.klass;
                    klass->type = existLS;
                    continue;
                }
            }

            // Создать типы для новых классов
            auto typeByVal = BNM_I2C_NEW(Il2CppType);
            memset(typeByVal, 0, sizeof(IL2CPP::Il2CppType));
            typeByVal->type = IL2CPP::Il2CppTypeEnum::IL2CPP_TYPE_CLASS;
            typeByVal->attrs = 0x1; // Public
            typeByVal->pinned = 0;
            typeByVal->byref = 0;
            typeByVal->num_mods = 0;

            auto typeThis = BNM_I2C_NEW(Il2CppType);
            memset(typeThis, 0, sizeof(IL2CPP::Il2CppType));
            typeThis->type = IL2CPP::Il2CppTypeEnum::IL2CPP_TYPE_CLASS;
            typeThis->attrs = 0x1; // Public
            typeThis->pinned = 0;
            typeThis->byref = 1;
            typeThis->num_mods = 0;

            // Создать BNMTypeData для получения класса из типа
            auto bnmTypeData = new BNMTypeData();
            typeByVal->data.dummy = (void *)bnmTypeData; // Для il2cpp::vm::Class::FromIl2CppType
            typeThis->data.dummy = (void *)bnmTypeData; // Для il2cpp::vm::Class::FromIl2CppType

            // Получить родительский класс
            IL2CPP::Il2CppClass *parent = klass->GetBaseType().ToIl2CppClass();
            if (!parent) parent = GetType<IL2CPP::Il2CppObject *>().ToIl2CppClass();

            // Требуется для интерфейсов, но они еще не полностью реализованы
            std::vector<IL2CPP::Il2CppRuntimeInterfaceOffsetPair> newInterOffsets;
            if (parent->interfaceOffsets)
                for (uint16_t i = 0; i < parent->interface_offsets_count; ++i)
                    newInterOffsets.push_back(parent->interfaceOffsets[i]);

            auto newVtableSize = parent->vtable_count;

            // Изучить интерфейсы
            auto allInterfaces = klass->GetInterfaces();
            std::vector<IL2CPP::Il2CppClass *> interfaces;
            if (!allInterfaces.empty()) for (auto &interface : allInterfaces)
                if (auto cls = interface.ToIl2CppClass(); !hasInterface(parent, cls)) {
                    interfaces.push_back(cls);
                }
            // Требуется для переопределения виртуальных методов
            std::vector<IL2CPP::VirtualInvokeData> newVTable(newVtableSize);
            for (uint16_t i = 0; i < parent->vtable_count; ++i) newVTable[i] = parent->vtable[i];
            for (auto &interface : interfaces) {
                newInterOffsets.push_back({interface, newVtableSize});
                for (uint16_t i = 0; i < interface->method_count; ++i) {
                    auto v = interface->methods[i];
                    ++newVtableSize;
                    newVTable.push_back({nullptr, v});
                }
            }
            // Создать все новые методы
            const IL2CPP::MethodInfo **methods = nullptr;
            auto methods4Add = klass->GetMethods();
            if (!methods4Add.empty()) {
                // Проверить, нужно ли добавлять обычный конструктор в класс
                // Обычный конструктор вызывается в il2cpp::vm::Runtime::ObjectInitException
                bool needDefaultConstructor = std::none_of(methods4Add.begin(), methods4Add.end(), [](NEW_CLASSES::NewMethod *met) {
                    return !strcmp(met->GetName(), OBFUSCATE_BNM(".ctor")) && met->GetArgsCount() == 0;
                });

                // Создать маcсив методов
                methods = (const IL2CPP::MethodInfo **) calloc(methods4Add.size() + needDefaultConstructor, sizeof(IL2CPP::MethodInfo *));

                // Создать все методы
                for (int i = 0; i < methods4Add.size(); ++i) {
                    auto &method = methods4Add[i];
                    // Создать новый MethodInfo
                    method->myInfo = BNM_I2C_NEW(MethodInfo);

                    // Установить адрес метода
                    method->myInfo->methodPointer = (IL2CPP::Il2CppMethodPointer)method->GetAddress();

                    // Установить адрес инициатора вызова метода
                    method->myInfo->invoker_method = (IL2CPP::InvokerMethod)method->GetInvoker();

                    // Установить кол-во аргументов
                    method->myInfo->parameters_count = method->GetArgsCount();

                    // Создать имя метода
                    auto name = method->GetName();
                    auto len = strlen(name) + 1;
                    method->myInfo->name = (char *) malloc(sizeof(char) * len);
                    memset((void *)method->myInfo->name, 0, len);
                    strcpy((char *)method->myInfo->name, name);

                    // Замена методов в таблице
                    auto types = method->GetArgTypes();
                    for (uint16_t v = 0; v < newVtableSize; ++v) {
                        auto &vTable = newVTable[v];
                        auto count = vTable.method->parameters_count;

                        if (!strcmp(vTable.method->name, method->myInfo->name) && count == method->myInfo->parameters_count && types.size() == count) {
                            bool same = true;
                            for (uint8_t p = 0; p < count; ++p) {
#if UNITY_VER < 212
                                auto type = (vTable.method->parameters + p)->parameter_type;
#else
                                auto type = vTable.method->parameters[p];
#endif
                                if (il2cpp_type_equals(type, types[p])) continue;

                                same = false;
                                break;

                            }
                            if (!same) break;

                            vTable.method = method->myInfo;
                            vTable.methodPtr = method->myInfo->methodPointer;
                            break;

                        }
                    }

                    // Установить флаги метода
                    method->myInfo->flags = 0x0006 | 0x0080; // PUBLIC | HIDE_BY_SIG
                    if (method->IsStatic()) method->myInfo->flags |= 0x0010; // |= STATIC

                    // BNM не поддерживает generic-методы
                    method->myInfo->is_generic = false;

                    // Установить возвращаемый тип
                    method->myInfo->return_type = method->GetRetType().ToIl2CppType();

                    // Создать аргументы
                    auto argsCount = method->GetArgsCount();
                    if (argsCount) {
#if UNITY_VER < 212
                        // Создать новый массив аргументов
                        method->myInfo->parameters = (IL2CPP::ParameterInfo *)calloc(argsCount, sizeof(IL2CPP::ParameterInfo));

                        // Получить первый аргумент
                        auto newParam = (IL2CPP::ParameterInfo *)method->myInfo->parameters;

                        // Создать аргументы
                        for (int p = 0; p < argsCount; ++p) {

                            // Получить длину нового имени
                            len = (OBFUSCATES_BNM("аргумент") + std::to_string(p)).size();

                            // Создать имя
                            newParam->name = (char *) malloc(sizeof(char) * len + 1);
                            memset((void *)newParam->name, 0, len + 1);
                            strcat((char *)newParam->name, OBFUSCATE_BNM("аргумент")); // Перевод чтобы был
                            strcpy((char *)newParam->name, std::to_string(p).c_str());

                            // Установить номер аргумента
                            newParam->position = p;

                            // Попытаться установить тип аргумента
                            if (!types.empty() && p < types.size()) {
                                newParam->parameter_type = types[p].ToIl2CppType();
                                newParam->token = newParam->parameter_type->attrs | p;
                            } else {
                                newParam->parameter_type = nullptr;
                                newParam->token = p;
                            }
                            // Следующий!
                            ++newParam;
                        }
#else
                        // Создать новый маcсив типов аргументов
                        auto **params = new IL2CPP::Il2CppType*[argsCount];

                        // Установить маcсив
                        method->myInfo->parameters = (const IL2CPP::Il2CppType **)params;

                        // Создать аргументы
                        for (int p = 0; p < argsCount; ++p) {

                            // Создать тип
                            auto newParam = BNM_I2C_NEW(Il2CppType);

                            // Копировать, если можно
                            if (p < types.size())
                                memcpy(newParam, types[p].ToIl2CppType(), sizeof(IL2CPP::Il2CppType));

                            // Установить
                            params[p] = newParam;
                        }
#endif
                    }
#if UNITY_VER >= 212
                    else {
                        // Мы не можем без лишнего кода установить имена аргументов, потому что они перемещены в метаданные
                        method->myInfo->methodMetadataHandle = nullptr;
                    }
#endif
                    types.clear(); types.shrink_to_fit();
                    methods[i] = method->myInfo;
                }
                // Создать обычный конструктор
                if (needDefaultConstructor) {
                    auto method = BNM_I2C_NEW(MethodInfo);
                    method->name = (char *) malloc(sizeof(char) * 6);
                    memset((void *)method->name, 0, 6);
                    strcpy((char *)method->name, OBFUSCATE_BNM(".ctor"));
                    method->parameters_count = 0;
                    method->parameters = nullptr;
                    method->return_type = GetType<void>().ToIl2CppType();
                    method->is_generic = false;
                    method->flags = 0x0006 | 0x0080; // PUBLIC | HIDE_BY_SIG
                    method->methodPointer = (IL2CPP::Il2CppMethodPointer) DefaultConstructor;
                    method->invoker_method = (IL2CPP::InvokerMethod) DefaultConstructorInvoke;
                    methods[methods4Add.size()] = method;
                }
            }
#if UNITY_VER > 174
#define kls klass
#define typeSymbol *
#else
#define kls declaring_type
#define typeSymbol
#endif

            // Создать новый класс
            klass->myClass = (IL2CPP::Il2CppClass *)malloc(sizeof(IL2CPP::Il2CppClass) + newVTable.size() * sizeof(IL2CPP::VirtualInvokeData));
            memset(klass->myClass, 0, sizeof(IL2CPP::Il2CppClass) + newVTable.size() * sizeof(IL2CPP::VirtualInvokeData));
            uint8_t hasFinalize = 0;
            // Установить методы, если они существуют
            if (!methods4Add.empty()) {
                // Установить новый созданный класс
                for (int i = 0; i < methods4Add.size(); ++i) {
                    if (!hasFinalize) hasFinalize = !strcmp(methods[i]->name, OBFUSCATE_BNM("Finalize"));
                    ((IL2CPP::MethodInfo *)methods[i])->kls = klass->myClass;
                }

                // Установить кол-во методов
                klass->myClass->method_count = methods4Add.size();

                // Установить массив методов
                klass->myClass->methods = methods;

                // Очистить methods4Add
                methods4Add.clear(); methods4Add.shrink_to_fit();
            } else {
                klass->myClass->method_count = 0;
                klass->myClass->methods = nullptr;
            }
            // Установить родителя
            klass->myClass->parent = parent;

            auto fields4Add = klass->GetFields();
            klass->myClass->field_count = fields4Add.size();
            if (klass->myClass->field_count > 0) {
                // Создать список полей
                auto fields = (IL2CPP::FieldInfo *)calloc(klass->myClass->field_count, sizeof(IL2CPP::FieldInfo));

                // Получить первое поле
                IL2CPP::FieldInfo *newField = fields;
                if (!fields4Add.empty()) {
                    for (auto &field : fields4Add) {
                        // Создать имя
                        auto name = field->GetName();
                        auto len = strlen(name) + 1;
                        newField->name = (char *) malloc(sizeof(char) * len);
                        memset((void *)newField->name, 0, len);
                        strcpy((char *)newField->name, name);

                        // Копировать тип
                        newField->type = BNM_I2C_NEW(Il2CppType, *field->GetType().ToIl2CppType());

                        // Добавить флаги видимости
                        ((IL2CPP::Il2CppType*)newField->type)->attrs |= field->GetAttributes(); // PUBLIC

                        // Создать знак (token)
                        newField->token = newField->type->attrs;

                        // Установить новый класс
                        newField->parent = klass->myClass;

                        // Установить адрес
                        newField->offset = field->GetOffset();

                        // Установить информацию
                        field->myInfo = newField;

                        // Следующий!
                        newField++;
                    }
                    // Очистить fields4Add
                    fields4Add.clear(); fields4Add.shrink_to_fit();
                }
                klass->myClass->fields = fields;
            } else {
                klass->myClass->static_fields_size = 0;
                klass->myClass->static_fields = nullptr;
                klass->myClass->fields = nullptr;
            }

            // Создать иерархию типов
            klass->myClass->typeHierarchyDepth = parent->typeHierarchyDepth + 1;
            klass->myClass->typeHierarchy = (IL2CPP::Il2CppClass **)calloc(klass->myClass->typeHierarchyDepth, sizeof(IL2CPP::Il2CppClass *));
            klass->myClass->typeHierarchy[klass->myClass->typeHierarchyDepth - 1] = klass->myClass;
            memcpy(klass->myClass->typeHierarchy, parent->typeHierarchy, parent->typeHierarchyDepth * sizeof(IL2CPP::Il2CppClass *));

            // Установить образ
            klass->myClass->image = curImg;

            // Создать имя
            auto len = strlen(className) + 1;
            klass->myClass->name = (char *) malloc(sizeof(char) * len);
            memset((void *)klass->myClass->name, 0, len);
            strcpy((char *)klass->myClass->name, className);

            // Создать имя пространства имён
            len = strlen(classNamespace) + 1;
            klass->myClass->namespaze = (char *) malloc(sizeof(char) * len);
            memset((void *)klass->myClass->namespaze, 0, len);
            strcpy((char *)klass->myClass->namespaze, classNamespace);

            // Установить типы
            klass->myClass->byval_arg = typeSymbol typeByVal;
            klass->myClass->this_arg = typeSymbol typeThis;
#undef kls
#undef typeSymbol
            // Это для внутренних классов, но BNM не может создавать внутренние классы
            klass->myClass->declaringType = nullptr;

            // Копировать флаги родителя и убрать ABSTRACT флаг, если существует
            klass->myClass->flags = klass->myClass->parent->flags & ~0x00000080; // TYPE_ATTRIBUTE_ABSTRACT

            // Установить классы
            klass->myClass->element_class = klass->myClass;
            klass->myClass->castClass = klass->myClass;
#if UNITY_VER > 174
            klass->myClass->klass = klass->myClass;
#endif
            // Инициализировать размеры
            klass->myClass->native_size = -1;
            klass->myClass->element_size = 0;
            klass->myClass->instance_size = klass->myClass->actualSize = klass->GetSize();

            // Установить виртуальную таблицу
            klass->myClass->vtable_count = newVTable.size();
            for (int i = 0; i < newVTable.size(); ++i) klass->myClass->vtable[i] = newVTable[i];
            newVTable.clear(); newVTable.shrink_to_fit();

            // Установить адреса интерфейсов
            klass->myClass->interface_offsets_count = newInterOffsets.size();
            klass->myClass->interfaceOffsets = (IL2CPP::Il2CppRuntimeInterfaceOffsetPair *)(calloc(newInterOffsets.size(), sizeof(IL2CPP::Il2CppRuntimeInterfaceOffsetPair)));
            for (int i = 0; i < newInterOffsets.size(); ++i) klass->myClass->interfaceOffsets[i] = newInterOffsets[i];
            newInterOffsets.clear(); newInterOffsets.shrink_to_fit();

            // Добавить интерфейсы
            if (!interfaces.empty()) {
                klass->myClass->interfaces_count = interfaces.size();
                klass->myClass->implementedInterfaces = (IL2CPP::Il2CppClass **)calloc(interfaces.size(), sizeof(IL2CPP::Il2CppClass *));
                for (int i = 0; i < interfaces.size(); ++i) klass->myClass->implementedInterfaces[i] = interfaces[i];
                interfaces.clear(); interfaces.shrink_to_fit();
            } else {
                klass->myClass->interfaces_count = 0;
                klass->myClass->implementedInterfaces = nullptr;
            }

            // Запретить il2cpp вызывать LivenessState::TraverseGCDescriptor для класса
            klass->myClass->gc_desc = nullptr;

            // BNM не поддерживает создание generic классов
            klass->myClass->generic_class = nullptr;
            klass->myClass->genericRecursionDepth = 1;

            // Остальная инициализация
            klass->myClass->initialized = 1;
#if UNITY_VER > 182
            klass->myClass->initialized_and_no_error = 1;
#if UNITY_VER >= 231
            klass->myClass->initializationExceptionGCHandle = nullptr;
#else
            klass->myClass->initializationExceptionGCHandle = 0;
#endif
#if UNITY_VER < 212
            klass->myClass->has_initialization_error = 0;
#endif
#if UNITY_VER < 222
            klass->myClass->naturalAligment = 0;
#endif
#endif
            klass->myClass->init_pending = 0;
#if UNITY_VER < 202
            // Отсутствует поддержка generic
            klass->myClass->genericContainerIndex = -1;
#else
            // Отсутствует поддержка generic
            klass->myClass->genericContainerHandle = nullptr;
            klass->myClass->typeMetadataHandle = nullptr;
#endif
#if UNITY_VER < 211
            klass->myClass->valuetype = 1;
#endif
            // Остальные переменные, которые необходимо установить
            klass->myClass->interopData = nullptr;
            klass->myClass->nestedTypes = nullptr;
            klass->myClass->properties = nullptr;
            klass->myClass->rgctx_data = nullptr;
            klass->myClass->has_references = 0;
            klass->myClass->has_finalize = 0;
            klass->myClass->size_inited = klass->myClass->is_vtable_initialized = 1;
            klass->myClass->has_cctor = 0;
            klass->myClass->enumtype = 0;
            klass->myClass->minimumAlignment = 8;
            klass->myClass->is_generic = 0;
            klass->myClass->rank = 0;
            klass->myClass->nested_type_count = 0;
            klass->myClass->thread_static_fields_offset = 0;
            klass->myClass->thread_static_fields_size = -1;
            klass->myClass->cctor_started = 0;
#if UNITY_VER >= 203 && (UNITY_VER != 211 || UNITY_PATCH_VER >= 24)
            klass->myClass->size_init_pending = 0;
#endif
#if UNITY_VER < 212
            klass->myClass->cctor_finished = 0;
#endif
            klass->myClass->cctor_thread = 0;

            // Установить класс в bnmTypeData для получения его из типа
            bnmTypeData->cls = klass->myClass;

            // Добавить класс к списку созданных классов
            BNMClassesMap.addClass(curImg, klass->myClass);

            // Получить С#-тип
            klass->type = LoadClass(klass->myClass);

            LOGIBNM(OBFUSCATE_BNM("[InitNewClasses] Добавлен новый класс (%p) [%s]::[%s], родитель которого - [%s]::[%s], в [%s]."), klass->myClass, klass->myClass->namespaze, klass->myClass->name, parent->namespaze, parent->name, klass->myClass->image->name);
        }
        // Очистить classes4Add
        classes4Add->clear(); classes4Add->shrink_to_fit();
        delete classes4Add;
        classes4Add = nullptr;
    }
#endif
    void Image$$GetTypes(IL2CPP::Il2CppImage *image, bool, ClassVector *target) {
        bnm_shared_lock lock(findClasses);

        // Проверить образ и цель (target)
        if (!image || !target) return;

        // Получить не BNM-классы
        if (image->nameToClassHashTable != (decltype(image->nameToClassHashTable))-0x424e4d) {
            if (HasImageGetCls) {
                // BNM предпочитает использовать il2cpp_image_get_class
                // Проще получить его, чем найти Image$$GetTypes
                DO_API(IL2CPP::Il2CppClass *, il2cpp_image_get_class, (IL2CPP::Il2CppImage *, decltype(image->typeCount)));

                auto typeCount = image->typeCount;

                for (decltype(image->typeCount) i = 0; i < typeCount; ++i) {
                    // Получить класс
                    auto type = il2cpp_image_get_class(image, i);

                    // Пропустить <Module>
                    if (OBFUSCATES_BNM("<Module>") == type->name) continue;

                    // Добавить класс
                    target->push_back(type);
                }
            } else old_Image$$GetTypes(image, false, target);
        }

#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
        // Получить BNM-классы
        BNMClassesMap.forEachByImage(image, [&target](IL2CPP::Il2CppClass *BNM_class) -> bool {
            target->push_back(BNM_class);
            return false;
        });
#endif
    }

    // Найти некоторые методы и подменить их
    void InitIl2cppMethods() {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
        int count = 1;
#elif defined(__i386__) || defined(__x86_64__)
        // x86 имеет один вызов до кода метода
        int count = 2;
#endif


        //! il2cpp::vm::Class::Init ПОЛУЧИТЬ
        if (!Class$$Init) {
            // Путь:
            // il2cpp_array_new_specific ->
            // il2cpp::vm::Array::NewSpecific ->
            // il2cpp::vm::Class::Init
            Class$$Init = (decltype(Class$$Init))  HexUtils::FindNextJump(HexUtils::FindNextJump((BNM_PTR) BNM_dlsym(dlLib, OBFUSCATE_BNM("il2cpp_array_new_specific")), count), count);
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::Init в библиотеке: %p."), offsetInLib((void *)Class$$Init));
        }


        //! il2cpp_image_get_class ПРОВЕРИТЬ (Unity 2018+)
        {
            DO_API(void,il2cpp_image_get_class,());
            HasImageGetCls = il2cpp_image_get_class != nullptr;
        }


        //! il2cpp::vm::Image::GetTypes ПОДМЕНИТЬ И ПОЛУЧИТЬ (ИЛИ ТОЛЬКО ПОЛУЧИТЬ)
        if (!old_Image$$GetTypes && !HasImageGetCls) {
            DO_API(const IL2CPP::Il2CppImage *, il2cpp_get_corlib, ());
            DO_API(IL2CPP::Il2CppClass *, il2cpp_class_from_name, (const IL2CPP::Il2CppImage *, const char *, const char *));
            auto assemblyClass = il2cpp_class_from_name(il2cpp_get_corlib(), OBFUSCATE_BNM("System.Reflection"), OBFUSCATE_BNM("Assembly"));
            BNM_PTR GetTypesAdr = LoadClass(assemblyClass).GetMethodByName(OBFUSCATE_BNM("GetTypes"), 1).GetOffset();
            const int sCount
#if UNITY_VER >= 211
                    = count;
#elif UNITY_VER > 174
            = count + 1;
#else
            = count + 2;
#endif
            // Путь:
            // System.Reflection.Assembly.GetTypes(bool) ->
            // il2cpp::icalls::mscorlib::System::Reflection::Assembly::GetTypes ->
            // il2cpp::icalls::mscorlib::System::Module::InternalGetTypes ->
            // il2cpp::vm::Image::GetTypes
            auto Image$$GetTypes_t = HexUtils::FindNextJump(HexUtils::FindNextJump(HexUtils::FindNextJump(GetTypesAdr, count), sCount), count);
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
            HOOK(Image$$GetTypes_t, Image$$GetTypes, old_Image$$GetTypes);
#else
            old_Image$$GetTypes = (decltype(old_Image$$GetTypes)) Image$$GetTypes_t;
#endif
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Image::GetTypes в библиотеке: %p."), offsetInLib((void *)Image$$GetTypes_t));
        } else if (HasImageGetCls) {
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] в коде есть il2cpp_image_get_class. BNM будет использовать его."));
        }
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES

        //! il2cpp::vm::Class::FromIl2CppType ПОДМЕНИТЬ
        if (!old_Class$$FromIl2CppType) {
            // Путь:
            // il2cpp_class_from_type ->
            // il2cpp::vm::Class::FromIl2CppType
            auto from_type_adr = HexUtils::FindNextJump((BNM_PTR) BNM_dlsym(dlLib, OBFUSCATE_BNM("il2cpp_class_from_type")), count);
            HOOK(from_type_adr, Class$$FromIl2CppType, old_Class$$FromIl2CppType);
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::FromIl2CppType в библиотеке: %p."), offsetInLib((void *)from_type_adr));
        }

        //! il2cpp::vm::Type::GetClassOrElementClass ПОДМЕНИТЬ
        if (!old_Type$$GetClassOrElementClass) {
            // Путь:
            // il2cpp_type_get_class_or_element_class ->
            // il2cpp::vm::Type::GetClassOrElementClass
            auto from_type_adr = HexUtils::FindNextJump((BNM_PTR) BNM_dlsym(dlLib, OBFUSCATE_BNM("il2cpp_type_get_class_or_element_class")), count);
            HOOK(from_type_adr, Type$$GetClassOrElementClass, old_Type$$GetClassOrElementClass);
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Type::GetClassOrElementClass в библиотеке: %p."), offsetInLib((void *)from_type_adr));
        }

        //! il2cpp::vm::Image::ClassFromName ПОДМЕНИТЬ
        if (!old_Class$$FromName) {
            // Путь:
            // il2cpp_class_from_name ->
            // il2cpp::vm::Class::FromName ->
            // il2cpp::vm::Image::ClassFromName
            auto from_name_adr = HexUtils::FindNextJump(HexUtils::FindNextJump((BNM_PTR) BNM_dlsym(dlLib, OBFUSCATE_BNM("il2cpp_class_from_name")), count), count);
            HOOK(from_name_adr, Class$$FromName, old_Class$$FromName);
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Image::FromName в библиотеке: %p."), offsetInLib((void *)from_name_adr));
        }
#ifdef BNM_IL2CPP_ZERO_PTR
        //! il2cpp::vm::Object::NewAllocSpecific ПОДМЕНИТЬ
        if (!old_Object$$NewAllocSpecific) {
            // Путь:
            // il2cpp_object_new ->
            // il2cpp::vm::Object::New ->
            // il2cpp::vm::Object::NewAllocSpecific
            auto alloc_specific_adr = HexUtils::FindNextJump(HexUtils::FindNextJump((BNM_PTR) BNM_dlsym(dlLib, OBFUSCATE_BNM("il2cpp_object_new")), count), count);
            HOOK(alloc_specific_adr, Object$$NewAllocSpecific, old_Object$$NewAllocSpecific);
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Object::NewAllocSpecific в библиотеке: %p."), offsetInLib((void *)alloc_specific_adr));
        }
#endif
#if UNITY_VER <= 174

        //! il2cpp::vm::MetadataCache::GetImageFromIndex ПОДМЕНИТЬ
        if (!old_GetImageFromIndex) {
            // Путь:
            // il2cpp_assembly_get_image ->
            // il2cpp::vm::Assembly::GetImage ->
            // il2cpp::vm::MetadataCache::GetImageFromIndex
            auto GetImageFromIndexOffset = HexUtils::FindNextJump(HexUtils::FindNextJump((BNM_PTR)BNM_dlsym(dlLib, OBFUSCATE_BNM("il2cpp_assembly_get_image")), count), count);
            HOOK(GetImageFromIndexOffset, new_GetImageFromIndex, old_GetImageFromIndex);
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::MetadataCache::GetImageFromIndex в библиотеке: %p."), offsetInLib((void *)GetImageFromIndexOffset));
        }

        //! il2cpp::vm::Assembly::Load ЗАМЕНИТЬ
        static void *old_AssemblyLoad = nullptr;
        if (!old_AssemblyLoad) {
            // Путь:
            // il2cpp_domain_assembly_open ->
            // il2cpp::vm::Assembly::Load
            BNM_PTR AssemblyLoadOffset = HexUtils::FindNextJump((BNM_PTR)BNM_dlsym(dlLib, OBFUSCATE_BNM("il2cpp_domain_assembly_open")), count);
            HOOK(AssemblyLoadOffset, Assembly$$Load, old_AssemblyLoad);
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::Load в библиотеке: %p."), offsetInLib((void *)AssemblyLoadOffset));
        }

#endif
#endif

        //! il2cpp::vm::Assembly::GetAllAssemblies ПОЛУЧИТЬ
        if (!Assembly$$GetAllAssemblies) {
#ifdef BNM_USE_APPDOMAIN
            DO_API(IL2CPP::Il2CppImage *, il2cpp_get_corlib, ());
            DO_API(IL2CPP::Il2CppClass *, il2cpp_class_from_name, (IL2CPP::Il2CppImage *, const char *, const char *));
            auto assemblyClass = il2cpp_class_from_name(il2cpp_get_corlib(), OBFUSCATE_BNM("System"), OBFUSCATE_BNM("AppDomain"));
            auto getAssembly = LoadClass(assemblyClass).GetMethodByName(OBFUSCATE_BNM("GetAssemblies"), 1);
            if (getAssembly) {
                const int sCount
#if !defined(__aarch64__) && UNITY_VER >= 211
                    = count;
#else
                    = count + 1;
#endif
                // Путь:
                // System.AppDomain.GetAssemblies(bool) ->
                // il2cpp::icalls::mscorlib::System::AppDomain::GetAssemblies ->
                // il2cpp::vm::Assembly::GetAllAssemblies
                BNM_PTR GetAssembliesAdr = HexUtils::FindNextJump(getAssembly.GetOffset(), count);
                Assembly$$GetAllAssemblies = (AssemblyVector *(*)())(HexUtils::FindNextJump(GetAssembliesAdr, sCount));
                LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::GetAllAssemblies через AppDomain в библиотеке: %p."), offsetInLib((void *)Assembly$$GetAllAssemblies));
            } else {
#endif
            // Путь:
            // il2cpp_domain_get_assemblies ->
            // il2cpp::vm::Assembly::GetAllAssemblies
            auto adr = (BNM_PTR) BNM_dlsym(dlLib, OBFUSCATE_BNM("il2cpp_domain_get_assemblies"));
            Assembly$$GetAllAssemblies = (AssemblyVector *(*)())(HexUtils::FindNextJump(adr, count));
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::GetAllAssemblies через domain в библиотеке: %p."), offsetInLib((void *)Assembly$$GetAllAssemblies));
#ifdef BNM_USE_APPDOMAIN
            }
#endif
        }
    }
    void BNM_il2cpp_init(const char *domain_name) {
        old_BNM_il2cpp_init(domain_name);


        InitIl2cppMethods(); // Найти некоторые методы и подменить их
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
        InitNewClasses(); // Создать новые классы и добавить их в il2cpp
        ModifyClasses(); // Изменить существующие классы
#endif
        LibLoaded = true; // Установить LibLoaded в значение "истина"

        // Вызвать все события после загрузки il2cpp
        auto events = GetEvents();
        for (auto event : events) if (event) event();
        events.clear();
    }

#ifndef BNM_DISABLE_AUTO_LOAD
    [[maybe_unused]] __attribute__((constructor))
    void PrepareBNM() {
        // Попробовать получить libil2cpp.so во время загрузки библиотеки
        auto lib = BNM_dlopen(OBFUSCATE_BNM("libil2cpp.so"), RTLD_LAZY);
        if (InitDlLib(lib)) return;
        else BNM_dlclose(lib);

        // Попробовать получить libil2cpp.so на фоне
        BNM_thread loader([]() {
            do {
                if (hardBypass) break;
                auto lib = BNM_dlopen(OBFUSCATE_BNM("libil2cpp.so"), RTLD_LAZY);
                if (InitDlLib(lib)) break;
                else BNM_dlclose(lib);
            } while (true);
        });
        loader.detach();
    }
#endif

    // Проверка, действительна ли библиотека
    bool InitDlLib(void *dl, const char* path, bool external) {
        if (!dl) return false;
        void *init = BNM_dlsym(dl, OBFUSCATE_BNM("il2cpp_init"));
        // Проверить il2cpp_init
        if (init) {
            Dl_info info;
            int ret = BNM_dladdr(init, &info);
            if (!ret) return false;
            // Информация о библиотеке
            if (!path) {
                auto l = strlen(info.dli_fname) + 1;
                auto s = (char *) malloc(sizeof(char) * l);
                memset((void *)s, 0, l);
                strcpy(s, info.dli_fname);
                LibAbsolutePath = s;
            } else LibAbsolutePath = path;

            // Установить LibAbsoluteAddress
            LibAbsoluteAddress = (BNM_PTR)info.dli_fbase;

            // Подменить il2cpp_init, если BNM используется внутренне
            if (!external) HOOK(init, BNM_il2cpp_init, old_BNM_il2cpp_init);

            // Установить dlLib
            dlLib = dl;
        }
        return init;
    }
}
#undef DO_API