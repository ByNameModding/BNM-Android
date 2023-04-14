#include <list>
#include <codecvt>
#include <shared_mutex>
#include "BNM.hpp"
#include "BNM_data/utf8.h"

#define DO_API(r, n, p) auto (n) = (r (*) p)BNM_dlsym(BNM_Internal::dlLib, OBFUSCATE_BNM(#n))
namespace BNM_Internal {
    using namespace BNM;

    // Внутренние переменные
    static bool LibLoaded = false;
    static void *dlLib{};
    static bool hardBypass = false;
    static const char *LibAbsolutePath{};
    static BNM_PTR LibAbsoluteAddress{};
    static bool HasImageGetCls = false;
    static std::shared_mutex classesFindAccess{}, findClasses{};
    static std::mutex addClass{}, modClass{};
    static std::list<void(*)()> onIl2CppLoaded{};

    std::list<void(*)()> &GetEvents() { return onIl2CppLoaded; }


    // Методы для новых классов и для обычного режима
    AssemblyVector *(*Assembly$$GetAllAssemblies)(){};
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES

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
        auto c = (char *)malloc(size);
        std::copy(str.begin(), str.end(), c);
        c[size] = 0;
        return c;
    }
    return (char *)str.c_str();
}

namespace BNM {
    // Получить/Установить внутренние данные
    bool Il2cppLoaded() { return BNM_Internal::LibLoaded; }

    namespace MONO_STRUCTS {
        std::string monoString::get_string() {
            if (!this) return OBFUSCATE_BNM("ОШИБКА: monoString мёртв");
            if (!IsAllocated(chars)) return OBFUSCATE_BNM("ОШИБКА: символы строки мертвы");
            if (!length) return OBFUSCATE_BNM("");
            return BNM_Internal::Utf16ToUtf8(chars, length);
        }
        std::string monoString::str() { return get_string(); }

        const char *monoString::get_const_char() { return str2char(get_string()); }
        const char *monoString::c_str() { return get_const_char(); }

        std::string monoString::get_string_old() {
            if (!this) return OBFUSCATE_BNM("ОШИБКА: monoString мёртв");
            if (!IsAllocated(chars)) return OBFUSCATE_BNM("ОШИБКА: символы строки мертвы");
            if (!length) return OBFUSCATE_BNM("");
            return std::wstring_convert<std::codecvt_utf8<IL2CPP::Il2CppChar>, IL2CPP::Il2CppChar>().to_bytes((IL2CPP::Il2CppChar *)chars);
        }
        [[maybe_unused]] std::string monoString::strO() { return get_string_old(); }

        [[maybe_unused]] unsigned int monoString::getHash() {
            if (!this || !IsAllocated(chars)) return 0;
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

        [[maybe_unused]] monoString *monoString::Empty() { return LoadClass(OBFUSCATES_BNM("System"), OBFUSCATES_BNM("String")).GetFieldByName(OBFUSCATES_BNM("Empty")).cast<monoString *>()(); }
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

        return (void *) il2cpp_object_new(klass);
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

    IL2CPP::Il2CppObject *LoadClass::ObjNew(IL2CPP::Il2CppClass *cls) {
        DO_API(IL2CPP::Il2CppObject *, il2cpp_object_new, (IL2CPP::Il2CppClass *));
        return il2cpp_object_new(cls);
    }

    IL2CPP::Il2CppArray *LoadClass::ArrayNew(IL2CPP::Il2CppClass *cls, IL2CPP::il2cpp_array_size_t length) {
        DO_API(IL2CPP::Il2CppArray *, il2cpp_array_new, (IL2CPP::Il2CppClass *, IL2CPP::il2cpp_array_size_t));
        return il2cpp_array_new(cls, length);
    }


    /*** RuntimeTypeGetter ***/
    LoadClass RuntimeTypeGetter::ToLC() {
        if (!loadedClass.klass) {
            if (!name) {
                namespaze = OBFUSCATE("System");
                name = OBFUSCATE("Object");
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

    /*** RuntimeMethodGetter ***/
    Method<void> RuntimeMethodGetter::ToMethod() {
        if (!loadedMethod) {
            if (!argTypes.empty()) loadedMethod = type.ToLC().GetMethodByName(name, argTypes);
            if (!loadedMethod && !argNames.empty()) loadedMethod = type.ToLC().GetMethodByName(name, argNames);
            if (!loadedMethod) loadedMethod = type.ToLC().GetMethodByName(name, args);
        }
        return loadedMethod;
    }
    RuntimeMethodGetter::operator Method<void>() { return ToMethod(); }

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

    void InvokeHook(Method<int> m, void *newMet, void **oldMet) {
        if (m) InvokeHook(m.myInfo, newMet, oldMet);
    }

    void InvokeHook(IL2CPP::MethodInfo *m, void *newMet, void **oldMet) {
        if (!m) return;
        if (oldMet) *oldMet = (void *)m->methodPointer;
        m->methodPointer = (void(*)())newMet;
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
        [[maybe_unused]] const Vector3 Vector3::infinityVec = {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
        [[maybe_unused]] const Vector3 Vector3::zero = {0.f, 0.f, 0.f};
        [[maybe_unused]] const Vector3 Vector3::one = {1.f, 1.f, 1.f};
        [[maybe_unused]] const Vector3 Vector3::up = {0.f, 1.f, 0.f};
        [[maybe_unused]] const Vector3 Vector3::down = {0.f, -1.f, 0.f};
        [[maybe_unused]] const Vector3 Vector3::left = {-1.f, 0.f, 0.f};
        [[maybe_unused]] const Vector3 Vector3::right = {1.f, 0.f, 0.f};
        [[maybe_unused]] const Vector3 Vector3::forward = {0.f, 0.f, 1.f};
        [[maybe_unused]] const Vector3 Vector3::back = {0.f, 0.f, -1.f};
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
        // index: 0 - перый, 1 - второй и т. д.
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

    [[maybe_unused]] void HardBypass(JNIEnv *env) {
        if (!env || BNM_Internal::dlLib || BNM_Internal::hardBypass) return;
        BNM_Internal::hardBypass = true;

        // Получить путь до папки libs, используя JNI
        jclass activityThread = env->FindClass(OBFUSCATE_BNM("android/app/ActivityThread"));
        auto context = env->CallObjectMethod(env->CallStaticObjectMethod(activityThread, env->GetStaticMethodID(activityThread, OBFUSCATE_BNM("currentActivityThread"), OBFUSCATE_BNM("()Landroid/app/ActivityThread;"))), env->GetMethodID(activityThread, OBFUSCATE_BNM("getApplication"), OBFUSCATE_BNM("()Landroid/app/Application;")));
        auto appInfo = env->CallObjectMethod(context, env->GetMethodID(env->GetObjectClass(context), OBFUSCATE_BNM("getApplicationInfo"), OBFUSCATE_BNM("()Landroid/content/pm/ApplicationInfo;")));
        std::string path = env->GetStringUTFChars((jstring)env->GetObjectField(appInfo, env->GetFieldID(env->GetObjectClass(appInfo), OBFUSCATE_BNM("nativeLibraryDir"), OBFUSCATE_BNM("Ljava/lang/String;"))), nullptr);

        // Попробовать загрузить il2cpp по этому пути
        auto libPath = str2char(path + OBFUSCATE_BNM("/libil2cpp.so"));
        auto dl = BNM_dlopen(libPath, RTLD_LAZY);
        if (!BNM_Internal::InitDlLib(dl, libPath)) LOGEBNM(OBFUSCATE_BNM("Тяжёлый обход не удался!"));
    }

    namespace External {
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

    // Подмена `FromIl2CppType`, чтобы предотвраить вылет il2cpp при попытке получить класс из типа, созданого BNM
    IL2CPP::Il2CppClass *Class$$FromIl2CppType(IL2CPP::Il2CppType *type) {
        // Проверить, верен ли тип
        if (!type) return nullptr;

        // Проверить, создан ли тип BNM
        if (isBNMType(type)) return ((BNMTypeData *)type->data.dummy)->cls;

        return old_Class$$FromIl2CppType(type);
    }

    // Подмена `FromName`, чтобы предотвраить вылет il2cpp при попытке найти класс, созданный BNM
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
                    // Найдено, остановливаем for
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

        // Номер меньше 0, занчит, это сборка BNM
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
        std::lock_guard lock(modClass);
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

                    // Установить возвращяемый тип
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
                            strcat((char *)newParam->name, OBFUSCATE_BNM("аргумент")); // Первод чтобы был
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

                        // Создать аргуметны
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
                        // Мы не можен без лишнего кода установить имена аргументов, потому что они перемещены в метаданные
                        method->myInfo->methodMetadataHandle = nullptr;
                    }
#endif
                    newMethods[oldCount] = method->myInfo;
                    LOGDBNM(OBFUSCATE_BNM("[ModifyClasses] Добвален %sметод %s %d к %s."), method->IsStatic() ? OBFUSCATE_BNM("статический ") : OBFUSCATE_BNM(""), name, argsCount, lc.str().c_str());
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
                    auto len = strlen(name);
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

                    if (field->IsStatic()) {
                        // Установить адрес
                        field->offset = newField->offset = newStaticFieldsSize;

                        // Получить адрес следующего статического поля
                        newStaticFieldsSize += field->GetSize();
                    } else {
                        // Установить адрес
                        field->offset = newField->offset = currentAddress;

                        // Получить адрес следующего поля
                        currentAddress += field->GetSize();
                    }

                    // Установить информацию
                    field->myInfo = newField;

                    // Следующий!
                    newField++;
                    LOGDBNM(OBFUSCATE_BNM("[ModifyClasses] Добвалено %sполе %s к %s."), field->IsStatic() ? OBFUSCATE_BNM("статическое ") : OBFUSCATE_BNM(""), name, lc.str().c_str());
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

    void InitNewClasses() {
        std::lock_guard lock(addClass);
        if (!classes4Add) return;
        for (auto& klass : *classes4Add) {
            IL2CPP::Il2CppImage *curImg = nullptr;

            auto className = klass->GetName();
            auto classNamespace = klass->GetNamespace();

            // Проверка, существует ли уже класс в il2cpp
            {
                LoadClass existLS;

                DO_API(IL2CPP::Il2CppImage *, il2cpp_assembly_get_image, (const IL2CPP::Il2CppAssembly *));

                auto assemblies = BNM_Internal::Assembly$$GetAllAssemblies();

                // Попробовать найти образ
                auto dllName = klass->GetDllName();
                for (auto assembly : *assemblies) {
                    auto image = il2cpp_assembly_get_image(assembly);
                    if (!strcmp(dllName, image->nameNoExt)) {
                        curImg = image;
                        break;
                    }
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
                        if (!strcmp(cls->name, className) && !strcmp(cls->namespaze, classNamespace)) {
                            existLS = cls;
                            break;
                        }
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

            // Требуется для переопределения виртуальных методов, но они еще не полностью реализованы
            std::vector<IL2CPP::VirtualInvokeData> newVTable;
            for (uint16_t i = 0; i < parent->vtable_count; ++i) newVTable.push_back(parent->vtable[i]);

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

                    // Установить флаги метода
                    method->myInfo->flags = 0x0006 | 0x0080; // PUBLIC | HIDE_BY_SIG
                    if (method->IsStatic()) method->myInfo->flags |= 0x0010; // |= STATIC

                    // BNM не поддерживает generic-методы
                    method->myInfo->is_generic = false;

                    // Установить возвращяемый тип
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
                            strcat((char *)newParam->name, OBFUSCATE_BNM("аргумент")); // Первод чтобы был
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

                        // Создать аргуметны
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
                        types.clear(); types.shrink_to_fit();
                    }
#if UNITY_VER >= 212
                    else {
                        // Мы не можен без лишнего кода установить имена аргументов, потому что они перемещены в метаданные
                        method->myInfo->methodMetadataHandle = nullptr;
                    }
#endif
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

            // Установить методы, если они существуют
            if (!methods4Add.empty()) {
                // Установить новый созданный класс
                for (int i = 0; i < methods4Add.size(); ++i)
                    ((IL2CPP::MethodInfo *)methods[i])->kls = klass->myClass;

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
            // Создать поля
            klass->myClass->field_count = fields4Add.size();
            if (klass->myClass->field_count > 0) {
                // Создать список полей
                auto fields = (IL2CPP::FieldInfo *)calloc(klass->myClass->field_count, sizeof(IL2CPP::FieldInfo));

                // Получить первое поле
                IL2CPP::FieldInfo *newField = fields;
                if (!fields4Add.empty()) {

                    // Сортировка для получения правильного порядка статичексих полей
                    std::sort(fields4Add.begin(), fields4Add.end(), [](NEW_CLASSES::NewField *&lhs, NEW_CLASSES::NewField *&rhs){
                        return lhs->GetCppOffset() < rhs->GetCppOffset();
                    });

                    for (auto &field : fields4Add) {
                        // Создать имя
                        auto name = field->GetName();
                        auto len = strlen(name);
                        newField->name = (char *) malloc(sizeof(char) * len);
                        memset((void *)newField->name, 0, len);
                        strcpy((char *)newField->name, name);

                        // Копировать тип
                        newField->type = BNM_I2C_NEW(Il2CppType, *field->GetType().ToIl2CppType());

                        // Добавить флаги видимости
                        ((IL2CPP::Il2CppType*)newField->type)->attrs |= field->GetAttributes(); // PUBLIC (и STATIC, если ститическое)

                        // Создать знак (token)
                        newField->token = newField->type->attrs;

                        // Установить новый класс
                        newField->parent = klass->myClass;

                        if (!field->IsStatic()) {
                            // Установить адрес
                            newField->offset = field->GetOffset();
                        } else {
                            auto cppOffset = field->GetCppOffset();

                            // Установить staticFieldsAddress, если не установлен
                            if (!klass->staticFieldsAddress) klass->staticFieldsAddress = cppOffset;

                            // Установить адрес поля
                            newField->offset = klass->staticFieldOffset;

                            // Получить следующий адрес
                            klass->staticFieldOffset += field->GetSize();
                        }

                        // Установить информацию
                        field->myInfo = newField;

                        // Следующий!
                        newField++;
                    }
                    // Очистить fields4Add
                    fields4Add.clear(); fields4Add.shrink_to_fit();

                    // Установить staticFieldsAddress
                    klass->myClass->static_fields = (void *)klass->staticFieldsAddress;

                    // Установить staticFieldOffset
                    klass->myClass->static_fields_size = klass->staticFieldOffset;
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
            memcpy(klass->myClass->typeHierarchy, parent->typeHierarchy, parent->typeHierarchyDepth * sizeof(IL2CPP::Il2CppClass *));
            klass->myClass->typeHierarchy[klass->myClass->typeHierarchyDepth - 1] = klass->myClass;

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
            auto interfaces = klass->GetInterfaces();
            if (!interfaces.empty()) {
                klass->myClass->interfaces_count = interfaces.size();
                klass->myClass->implementedInterfaces = (IL2CPP::Il2CppClass **)calloc(interfaces.size(), sizeof(IL2CPP::Il2CppClass *));
                for (int i = 0; i < interfaces.size(); ++i) klass->myClass->implementedInterfaces[i] = interfaces[i].ToIl2CppClass();
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
            klass->myClass->initializationExceptionGCHandle = 0;
#if UNITY_VER < 212
            klass->myClass->has_initialization_error = 0;
#endif
            klass->myClass->naturalAligment = 0;
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
#if UNITY_VER >= 203
            klass->myClass->size_init_pending = 0;
#endif
#if UNITY_VER < 212
            klass->myClass->cctor_finished = 0;
#endif
            klass->myClass->cctor_thread = 0;

            // Установить класс в bnmTypeData для получения его из типа
            bnmTypeData->cls = klass->myClass;

            // Добавить класс к списку созданых классов
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
        std::shared_lock lock(findClasses);
        
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


        //! il2cpp::vm::Image::ClassFromName ПОДМЕНИТЬ
        if (!old_Class$$FromName) {
            // Путь:
            // il2cpp::vm::Class::FromName ->
            // il2cpp::vm::Image::ClassFromName
            auto from_name_adr = HexUtils::FindNextJump(HexUtils::FindNextJump((BNM_PTR) BNM_dlsym(dlLib, OBFUSCATE_BNM("il2cpp_class_from_name")), count), count);
            HOOK(from_name_adr, Class$$FromName, old_Class$$FromName);
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::FromName в библиотеке: %p."), offsetInLib((void *)from_name_adr));
        }
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
        }).detach();
    }
#endif

    // Проверка, действительна ли библиотека
    bool InitDlLib(void *dl, const char* path, bool external) {
        if (!dl) return false;
        void *init = BNM_dlsym(dl, OBFUSCATE_BNM("il2cpp_init"));
        // Проверить il2cpp_init
        if (init) {
            Dl_info info;
            BNM_dladdr(init, &info);
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