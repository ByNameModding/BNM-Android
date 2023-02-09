#include "BNM.hpp"
#include "BNM_data/utf8.h"

#define DO_API(r, n, p) auto (n) = (r (*) p)BNM_dlsym(BNM_Internal::dlLib, OBFUSCATE_BNM(#n))
namespace BNM_Internal {
    using namespace BNM;

    // Internal variables
    static bool LibLoaded = false;
    static void *dlLib{};
    static bool hardBypass = false;
    static const char *LibAbsolutePath{};
    static BNM_PTR LibAbsoluteAddress{};
    static bool HasImageGetCls = false;
    static void (*OnIl2CppLoadedEvent)(){};
    // Methods for new classes and for basic mode
    AssemblyVector *(*Assembly$$GetAllAssemblies)(){};
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES // Speed up IDE if c++ lower then c++17
    IL2CPP::Il2CppClass *(*old_Class$$FromIl2CppType)(IL2CPP::Il2CppType *type){};
    IL2CPP::Il2CppClass *Class$$FromIl2CppType(IL2CPP::Il2CppType *type);
    IL2CPP::Il2CppClass *(*old_Class$$FromName)(IL2CPP::Il2CppImage *image, const char *ns, const char *name){};
    IL2CPP::Il2CppClass *Class$$FromName(IL2CPP::Il2CppImage *image, const char *_namespace, const char *name);
    static std::vector<NEW_CLASSES::NewClass *> *Classes4Add{};
    struct BNMTypeData {
        BNMTypeData() = default;
        BNM_PTR bnm = -0x424e4d;
        IL2CPP::Il2CppClass *cls{};
    };
    void EmptyCtor(){}
    void InitNewClasses();
#endif
    void (*old_Image$$GetTypes)(IL2CPP::Il2CppImage *image, bool exportedOnly, TypeVector *target){};
    void Image$$GetTypes(IL2CPP::Il2CppImage *image, bool exportedOnly, TypeVector *target);
    void (*Class$$Init)(IL2CPP::Il2CppClass *klass){};

    IL2CPP::Il2CppClass *GetClassFromName(const std::string &_namespace, const std::string &_name) {
        DO_API(IL2CPP::Il2CppImage *, il2cpp_assembly_get_image, (IL2CPP::Il2CppAssembly *));
        for (auto assembly : *Assembly$$GetAllAssemblies()) {
            if (!CheckObj(assembly)) continue;
            TypeVector classes;
            auto img = il2cpp_assembly_get_image(assembly);
            Image$$GetTypes(img, false, &classes);
            for (auto cls : classes) {
                if (!CheckObj(cls)) continue;
                Class$$Init(cls);
                if (cls->name == _name && cls->namespaze == _namespace) {
                    classes.clear(); classes.shrink_to_fit();
                    return (IL2CPP::Il2CppClass *)cls;
                }
            }
            classes.clear(); classes.shrink_to_fit();
        }
        return nullptr;
    }

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
    void (*old_BNM_il2cpp_init)(const char *);
    void BNM_il2cpp_init(const char *domain_name);
    void InitIl2cppMethods();
#ifndef BNM_DISABLE_AUTO_LOAD
    [[maybe_unused]] __attribute__((constructor)) void PrepareBNM();
#endif
    bool InitDlLib(void *dl, const char *path = nullptr, bool external = false);
}
namespace BNM {
    // Get/Set internal data
    bool Il2cppLoaded() { return BNM_Internal::LibLoaded; }
    void SetIl2CppLoadEvent(void (*event)()) { BNM_Internal::OnIl2CppLoadedEvent = event; }
    
    char *str2char(const std::string &str) {
        size_t size = str.end() - str.begin();
        if (str.c_str()[size]){
            auto c = (char *)malloc(size);
            std::copy(str.begin(), str.end(), c);
            c[size] = 0;
            return c;
        }
        return (char *)str.c_str();
    }
    namespace MONO_STRUCTS {
        std::string monoString::get_string() {
            if (!this) return OBFUSCATE_BNM("ERROR: monoString is null");
            if (!isAllocated(chars)) return OBFUSCATE_BNM("ERROR: chars is null");
            if (!length) return OBFUSCATE_BNM("ERROR: str is empty");
            return BNM_Internal::Utf16ToUtf8(chars, length);
        }
        const char *monoString::get_const_char() { return str2char(get_string()); }
        const char *monoString::c_str() { return get_const_char(); }
        std::string monoString::str() { return get_string(); }
        [[maybe_unused]] std::string monoString::strO() { return get_string_old(); }
        monoString::operator std::string() { return get_string(); }
        monoString::operator const char *() { return get_const_char(); }
        std::string monoString::get_string_old() {
            if (!this) return OBFUSCATE_BNM("ERROR: monoString is null");
            if (!isAllocated(chars)) return OBFUSCATE_BNM("ERROR: chars is null");
            if (!length) return OBFUSCATE_BNM("ERROR: str is empty");
            return std::wstring_convert<std::codecvt_utf8<IL2CPP::Il2CppChar>, IL2CPP::Il2CppChar>().to_bytes((IL2CPP::Il2CppChar *)chars);
        }
        [[maybe_unused]] unsigned int monoString::getHash() {
            if (!this || !isAllocated(chars)) return 0;
            IL2CPP::Il2CppChar *p = chars;
            unsigned int h = 0;
            for (int i = 0; i < length; ++i)
                h = (h << 5) - h + *p; p++;
            return h;
        }
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
        [[maybe_unused]] monoString *monoString::Empty() { return LoadClass(OBFUSCATES_BNM("System"), OBFUSCATES_BNM("String")).GetFieldByName(OBFUSCATES_BNM("Empty")).setType<monoString *>()(); }

    }
    
    /*** LoadClass ***/

    LoadClass::LoadClass() noexcept = default;
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

    LoadClass::LoadClass(const std::string &_namespace, const std::string &clazz) {
        klass = BNM_Internal::GetClassFromName(_namespace, clazz);
        if (!klass)
            LOGWBNM(OBFUSCATE_BNM("Class: [%s].[%s] - not found (without dll)"), _namespace.c_str(), clazz.c_str());
    }

    LoadClass::LoadClass(const std::string &_namespace, const std::string &_name, const std::string &dllName) {
        DO_API(IL2CPP::Il2CppImage *, il2cpp_assembly_get_image, (const IL2CPP::Il2CppAssembly *));
        IL2CPP::Il2CppImage *dll = nullptr;
        for (auto assembly : *BNM_Internal::Assembly$$GetAllAssemblies())
            if (dllName == il2cpp_assembly_get_image(assembly)->name)
                dll = il2cpp_assembly_get_image(assembly);
        if (!dll) {
            LOGWBNM(OBFUSCATE_BNM("Dll: \"%s\" - not found"), dllName.c_str());
            klass = nullptr;
            return;
        }
        TypeVector classes;
        BNM_Internal::Image$$GetTypes(dll, false, &classes);
        for (auto cls : classes) {
            if (!cls) continue;
            BNM_Internal::Class$$Init(cls);
            if (cls->name == _name && cls->namespaze == _namespace) {
                klass = cls;
                break;
            }
        }
        classes.clear(); classes.shrink_to_fit();
        if (!klass)
            LOGWBNM(OBFUSCATE_BNM("Class: [%s].[%s] - not found (with dll)"), _namespace.c_str(), _name.c_str());
    }

    Method<void> LoadClass::GetMethodByName(const std::string &name, int parameters) const {
        if (!klass) return {};
        TryInit();
        auto curClass = klass;
        do {
            for (int i = 0; i < curClass->method_count; ++i) {
                auto method = (IL2CPP::MethodInfo *) curClass->methods[i];
                if (name == method->name && (method->parameters_count == parameters || parameters == -1))
                    return {method};
            }
            curClass = curClass->parent;
        } while (curClass);
        LOGWBNM(OBFUSCATE_BNM("Method: [%s]::[%s].[%s], %d - not found"), klass->namespaze, klass->name, name.c_str(), parameters);
        return {};
    }

    Method<void> LoadClass::GetMethodByName(const std::string &name, const std::vector<std::string> &params_names) const {
        if (!klass) return {};
        TryInit();
        DO_API(const char *, il2cpp_method_get_param_name, (IL2CPP::MethodInfo *method, uint32_t index));
        auto curClass = klass;
        size_t paramCount = params_names.size();
        do {
            for (int i = 0; i < curClass->method_count; ++i) {
                auto method = (IL2CPP::MethodInfo *) curClass->methods[i];
                if (name == method->name && method->parameters_count == paramCount) {
                    bool ok = true;
                    for (int g = 0; g < paramCount; ++g)
                        if (il2cpp_method_get_param_name(method, g) != params_names[g]) {
                            ok = false;
                            break;
                        }
                    if (ok) return {method};
                }
            }
            curClass = curClass->parent;
        } while (curClass);
        LOGWBNM(OBFUSCATE_BNM("Method: [%s]::[%s].[%s], %d - not found (using params_names)"), klass->namespaze, klass->name, name.c_str(), paramCount);
        return {};
    }

    Method<void> LoadClass::GetMethodByName(const std::string &name, const std::vector<std::string> &params_names, const std::vector<IL2CPP::Il2CppType *> &params_types) const {
        if (!klass) return {};
        TryInit();
        DO_API(IL2CPP::Il2CppClass *, il2cpp_class_from_il2cpp_type, (const IL2CPP::Il2CppType *));
        DO_API(const char *, il2cpp_method_get_param_name, (IL2CPP::MethodInfo *method, uint32_t index));
        auto curClass = klass;
        size_t paramCount = params_names.size();
        if (paramCount != params_types.size()) return {};
        do {
            for (int i = 0; i < curClass->method_count; ++i) {
                auto method = (IL2CPP::MethodInfo *)curClass->methods[i];
                if (name == method->name && method->parameters_count == paramCount) {
                    bool ok = true;
                    for (int g = 0; g < paramCount; ++g) {
#if UNITY_VER < 212
                        auto param = method->parameters + g;
                    auto cls = il2cpp_class_from_il2cpp_type(param->parameter_type);
#else
                        auto param = method->parameters[g];
                        auto cls = il2cpp_class_from_il2cpp_type(param);
#endif
                        auto param_cls = il2cpp_class_from_il2cpp_type(params_types[g]);
                        if (il2cpp_method_get_param_name(method, g) != params_names[g] || !(!strcmp(cls->name, param_cls->name) && !strcmp(cls->namespaze, param_cls->namespaze))) {
                            ok = false;
                            break;
                        }
                    }
                    if (ok) return {method};
                }
            }
            curClass = curClass->parent;
        } while (curClass);
        LOGWBNM(OBFUSCATE_BNM("Method: [%s]::[%s].[%s], %d - not found (using params_names and params_types)"), klass->namespaze, klass->name, name.c_str(), paramCount);
        return {};
    }

    Method<void> LoadClass::GetMethodByName(const std::string &name, const std::vector<IL2CPP::Il2CppType *> &params_types) const {
        if (!klass) return {};
        TryInit();
        DO_API(IL2CPP::Il2CppClass *, il2cpp_class_from_il2cpp_type, (const IL2CPP::Il2CppType *));
        auto curClass = klass;
        size_t paramCount = params_types.size();
        do {
            for (int i = 0; i < curClass->method_count; ++i) {
                auto method = (IL2CPP::MethodInfo *)curClass->methods[i];
                if (name == method->name && method->parameters_count == paramCount) {
                    bool ok = true;
                    for (int g = 0; g < paramCount; ++g) {
#if UNITY_VER < 212
                        auto param = method->parameters + g;
                    auto cls = il2cpp_class_from_il2cpp_type(param->parameter_type);
#else
                        auto param = method->parameters[g];
                        auto cls = il2cpp_class_from_il2cpp_type(param);
#endif
                        auto param_cls = il2cpp_class_from_il2cpp_type(params_types[g]);
                        if (!(cls->name == param_cls->name && cls->namespaze == param_cls->namespaze)) {
                            ok = false;
                            break;
                        }
                    }
                    if (ok) return {method};
                }
            }
            curClass = curClass->parent;
        } while (curClass);
        LOGWBNM(OBFUSCATE_BNM("Method: [%s]::[%s].[%s], %d - not found (using params_types)"), klass->namespaze, klass->name, name.c_str(), paramCount);
        return {};
    }

    [[maybe_unused]] std::vector<IL2CPP::MethodInfo *> LoadClass::GetMethodsInfo(bool includeParent) const {
        if (!klass) return {};
        TryInit();
        std::vector<IL2CPP::MethodInfo *> ret;
        auto curClass = klass;
        do {
            for (int i = 0; i < curClass->method_count; ++i) ret.emplace_back(Method<void>(curClass->methods[i]).GetInfo());
            if (includeParent) curClass = curClass->parent;
            else curClass = nullptr;
        } while (curClass);
        return ret;
    }

    [[maybe_unused]] std::vector<LoadClass> LoadClass::GetInnerClasses(bool includeParent) const {
        if (!klass) return {};
        TryInit();
        std::vector<LoadClass> ret;
        auto curClass = klass;
        do {
            for (int i = 0; i < curClass->nested_type_count; ++i) ret.emplace_back(curClass->nestedTypes[i]);
            if (includeParent) curClass = curClass->parent;
            else curClass = nullptr;
        } while (curClass);
        return ret;
    }

    [[maybe_unused]] std::vector<IL2CPP::FieldInfo *> LoadClass::GetFieldsInfo(bool includeParent) const {
        if (!klass) return {};
        TryInit();
        std::vector<IL2CPP::FieldInfo *> ret;
        auto curClass = klass;
        do {
            for (int i = 0; i < curClass->field_count; ++i) ret.emplace_back(curClass->fields + i);
            if (includeParent) curClass = curClass->parent;
            else curClass = nullptr;
        } while (curClass);
        return ret;
    }

    [[maybe_unused]] LoadClass LoadClass::GetInnerClass(const std::string &_name) const {
        for (auto cls : GetInnerClasses()) if (_name == cls.klass->name) return cls;
        return {};
    }

    IL2CPP::Il2CppType *LoadClass::GetIl2CppType() const {
        if (!klass) return nullptr;
        TryInit();
#if UNITY_VER > 174
        return (IL2CPP::Il2CppType *)&klass->byval_arg;
#else
        return (IL2CPP::Il2CppType *)klass->byval_arg;
#endif
    }

    [[maybe_unused]] MonoType *LoadClass::GetMonoType() const {
        if (!klass) return nullptr;
        TryInit();
        DO_API(IL2CPP::Il2CppObject *, il2cpp_type_get_object, (IL2CPP::Il2CppType *));
        return (MonoType *) il2cpp_type_get_object(GetIl2CppType());
    }

    void *LoadClass::CreateNewInstance() const {
        if (!klass) return nullptr;
        TryInit();
        if ((klass->flags & (0x00000080 | 0x00000020)))
            LOGWBNM(OBFUSCATE_BNM("You trying to create instance of abstract or interface class %s?\nIn c# this is impossible."), GetClassName().c_str());
        DO_API(IL2CPP::Il2CppObject *, il2cpp_object_new, (IL2CPP::Il2CppClass *));
        return (void *) il2cpp_object_new(klass);
    }

    IL2CPP::Il2CppClass *LoadClass::GetIl2CppClass() const {
        TryInit();
        return klass;
    }

    [[maybe_unused]] std::string LoadClass::GetClassName() const {
        if (klass) {
            TryInit();
            return OBFUSCATES_BNM("[") + klass->namespaze + OBFUSCATES_BNM("]::[") + klass->name + OBFUSCATES_BNM("]");
        }
        return OBFUSCATES_BNM("Uninitialized class");
    }

    void LoadClass::TryInit() const { if (klass) BNM_Internal::Class$$Init(klass); }

    IL2CPP::Il2CppArray *LoadClass::ArrayNew(IL2CPP::Il2CppClass *cls, IL2CPP::il2cpp_array_size_t length) {
        DO_API(IL2CPP::Il2CppArray *, il2cpp_array_new, (IL2CPP::Il2CppClass *, IL2CPP::il2cpp_array_size_t));
        return il2cpp_array_new(cls, length);
    }

    IL2CPP::Il2CppObject *LoadClass::ObjNew(IL2CPP::Il2CppClass *cls) {
        DO_API(IL2CPP::Il2CppObject *, il2cpp_object_new, (IL2CPP::Il2CppClass *));
        return il2cpp_object_new(cls);
    }

    IL2CPP::Il2CppObject *LoadClass::ObjBox(IL2CPP::Il2CppClass *klass, void *data) {
        DO_API(IL2CPP::Il2CppObject *, il2cpp_value_box, (IL2CPP::Il2CppClass *, void *));
        return il2cpp_value_box(klass, data);
    }

    Field<int> LoadClass::GetFieldByName(const std::string &name) const {
        if (!klass) return nullptr;
        TryInit();
        auto curClass = klass;
        do {
            for (int i = 0; i < curClass->field_count; ++i) {
                auto field = curClass->fields + i;
                if (name == field->name)
                    return field;
            }
            curClass = curClass->parent;
        } while (curClass);
        LOGWBNM(OBFUSCATE_BNM("Field: [%s]::[%s].(%s) - not found"), klass->namespaze, klass->name, name.c_str());
        return {};
    }

    bool LoadClass::ClassExists(const std::string &_namespace, const std::string &_name, const std::string &dllName) {
        DO_API(IL2CPP::Il2CppImage *, il2cpp_assembly_get_image, (const IL2CPP::Il2CppAssembly *));
        IL2CPP::Il2CppImage *dll = nullptr;
        for (auto assembly : *BNM_Internal::Assembly$$GetAllAssemblies())
            if (dllName == il2cpp_assembly_get_image(assembly)->name)
                dll = il2cpp_assembly_get_image(assembly);
        if (!dll)
            return false;
        TypeVector classes;
        BNM_Internal::Image$$GetTypes(dll, false, &classes);
        auto ret = std::any_of(classes.begin(), classes.end(), [&_namespace, &_name](IL2CPP::Il2CppClass *cls){ return cls->namespaze == _namespace && cls->name == _name; });
        classes.clear(); classes.shrink_to_fit();
        return ret;
    }

    [[maybe_unused]] LoadClass LoadClass::GetArrayClass() const {
        if (!klass) return {};
        TryInit();
        DO_API(IL2CPP::Il2CppClass *, il2cpp_array_class_get, (IL2CPP::Il2CppClass *, uint32_t));
        return il2cpp_array_class_get(klass, 1);
    }

    [[maybe_unused]] Property<bool> LoadClass::GetPropertyByName(const std::string &name, bool warning) {
        if (!klass) return {};
        TryInit();
        auto get = GetMethodByName(OBFUSCATES_BNM("get_") + name);
        auto set = GetMethodByName(OBFUSCATES_BNM("set_") + name);
        if (!get && !set){
            LOGWBNM(OBFUSCATE_BNM("Property %s.[%s] not found"), GetClassName().c_str(), name.c_str());
            return {};
        }
        if (!get && warning)
            LOGWBNM(OBFUSCATE_BNM("Property %s.[%s] without get"), GetClassName().c_str(), name.c_str());
        if (!set && warning)
            LOGWBNM(OBFUSCATE_BNM("Property %s.[%s] without set"), GetClassName().c_str(), name.c_str());
        return {get, set};
    }

    /*** TypeFinder ***/
    LoadClass TypeFinder::ToLC() const {
        auto ret = LoadClass(_namespace, name);
        return isArray ? ret.GetArrayClass() : ret;
    }
    IL2CPP::Il2CppType *TypeFinder::ToIl2CppType() const {
        return (IL2CPP::Il2CppType *)ToLC().GetIl2CppType();
    }
    IL2CPP::Il2CppClass *TypeFinder::ToIl2CppClass() const {
        return ToLC().GetIl2CppClass();
    }
    TypeFinder::operator LoadClass() const {
        return ToLC();
    }
    TypeFinder::operator IL2CPP::Il2CppType*() const {
        return ToIl2CppType();
    }
    TypeFinder::operator IL2CPP::Il2CppClass*() const {
        return ToIl2CppClass();
    }

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
    
    /*** BNM methods ***/
    using namespace MONO_STRUCTS;
    [[maybe_unused]] bool AttachIl2Cpp() {
        if (CurrentIl2CppThread()) return false;
        DO_API(IL2CPP::Il2CppDomain *, il2cpp_domain_get, ());
        DO_API(IL2CPP::Il2CppThread *, il2cpp_thread_attach, (IL2CPP::Il2CppDomain *));
        il2cpp_thread_attach(il2cpp_domain_get());
        return true;
    }
    [[maybe_unused]] void DetachIl2Cpp() {
        auto thread = CurrentIl2CppThread();
        if (!thread) return;
        DO_API(void, il2cpp_thread_detach, (IL2CPP::Il2CppThread *));
        il2cpp_thread_detach(thread);
    }
    [[maybe_unused]] IL2CPP::Il2CppThread *CurrentIl2CppThread() {
        DO_API(IL2CPP::Il2CppThread *, il2cpp_thread_current, ());
        return il2cpp_thread_current();
    }
    monoString *CreateMonoString(const char *str) {
        DO_API(monoString *, il2cpp_string_new, (const char *str));
        return il2cpp_string_new(str);
    }
    [[maybe_unused]] monoString *CreateMonoString(const std::string &str) { return CreateMonoString(str2char(str)); }
    void *getExternMethod(const std::string &str) {
        DO_API(void *, il2cpp_resolve_icall, (const char *));
        auto c_str = str2char(str);
        auto ret = il2cpp_resolve_icall(c_str);
        if (!ret) LOGWBNM(OBFUSCATE_BNM("Can't get extern %s. Please check code."), c_str);
        return ret;
    }
    [[maybe_unused]] std::string GetLibIl2CppPath() {
        if (!BNM_Internal::dlLib) return OBFUSCATE_BNM("libil2cpp not found!");
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
        if (m.init) InvokeHook(m.myInfo, newMet, oldMet);
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
            if (!FromId) InitFunc(FromId, getExternMethod(OBFUSCATE_BNM("UnityEngine.Object::FindObjectFromInstanceID")));
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
        std::string ReverseHexString(const std::string &hex) {
            std::string out;
            for (unsigned int i = 0; i < hex.length(); i += 2) out.insert(0, hex.substr(i, 2));
            return out;
        }
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
        BNM_PTR HexStr2BNM_PTR(const std::string &hex) { return strtoull(hex.c_str(), nullptr, 16); }
#if defined(__ARM_ARCH_7A__)
        bool Is_B_BL_Hex(const std::string &hex) {
            BNM_PTR hexW = HexStr2BNM_PTR(ReverseHexString(FixHexString(hex)));
            return (hexW & 0x0A000000) == 0x0A000000;
        }
#elif defined(__aarch64__)
        bool Is_B_BL_Hex(const std::string &hex) {
            BNM_PTR hexW = HexStr2BNM_PTR(ReverseHexString(FixHexString(hex)));
            return (hexW & 0xFC000000) == 0x14000000 || (hexW & 0xFC000000) == 0x94000000;
        }
#elif defined(__i386__) || defined(__x86_64__)
        bool Is_x86_call_hex(const std::string &hex) { return hex.substr(0, 2) == OBFUSCATE_BNM("E8"); }
#else
#warning "Call or B BL hex checker support only arm64, arm, x86 and x86_64"
#endif
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

        /*
         * Branch decoding based on
         * https://github.com/aquynh/capstone/
        */
        bool Decode_Branch_or_Call_Hex(const std::string &hex, BNM_PTR offset, BNM_PTR &outOffset) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
            if (!Is_B_BL_Hex(hex)) return false;
#if defined(__aarch64__)
            int add = 0;
#else
            int add = 8;
#endif
            outOffset = ((int32_t)(((((HexStr2BNM_PTR(ReverseHexString(FixHexString(hex)))) & (((uint32_t)1 << 24) - 1) << 0) >> 0) << 2) << (32 - 26)) >> (32 - 26)) + offset + add;
#elif defined(__i386__) || defined(__x86_64__)
            if (!Is_x86_call_hex(hex)) return false;
            outOffset = offset + HexStr2BNM_PTR(ReverseHexString(FixHexString(hex)).substr(0, 8)) + 5;
#else
#warning "Decode_Branch_or_Call_Hex support only arm64, arm, x86 and x86_64"
            return false;
#endif
            return true;
        }

        BNM_PTR FindNext_B_BL_offset(BNM_PTR start, int index) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
            int offset = 0;
            std::string curHex = ReadMemory(start, 4);
            BNM_PTR outOffset = 0;
            bool out;
            while (!(out = Decode_Branch_or_Call_Hex(curHex, start + offset, outOffset)) || index != 1) {
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
            while (!(out = Is_x86_call_hex(curHex)) || index != 1) {
                offset += 1;
                curHex = ReadMemory(start + offset, 1);
                if (out) index--;
            }
            Decode_Branch_or_Call_Hex(ReadMemory(start + offset, 5), start + offset, outOffset);
            return outOffset;
#endif
        }
    }
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
    namespace NEW_CLASSES {
        NewMethod::NewMethod() noexcept = default;
        NewField::NewField() noexcept = default;
        NewClass::NewClass() noexcept = default;
        void NewClass::AddNewField(NewField *field, bool isStatic) {
            if (!isStatic) Fields4Add.push_back(field);
            else {
                if (staticFieldOffset == 0x0) staticFieldsAddress = field->cppOffset;
                field->offset = staticFieldOffset;
                staticFieldOffset += field->size;
                Fields4Add.push_back(field);
            }
        }
        void NewClass::AddNewMethod(NewMethod *method) {
            Methods4Add.push_back(method);
        }
        void AddNewClass(NEW_CLASSES::NewClass *klass) {
            if (!BNM_Internal::Classes4Add)
                BNM_Internal::Classes4Add = new std::vector<NEW_CLASSES::NewClass *>();
            BNM_Internal::Classes4Add->push_back(klass);
        }
    }
#endif
    [[maybe_unused]] void HardBypass(JNIEnv *env) {
        if (!env || BNM_Internal::dlLib || BNM_Internal::hardBypass) return;
        BNM_Internal::hardBypass = true;
        jclass activityThread = env->FindClass(OBFUSCATE_BNM("android/app/ActivityThread"));
        auto context = env->CallObjectMethod(env->CallStaticObjectMethod(activityThread, env->GetStaticMethodID(activityThread, OBFUSCATE_BNM("currentActivityThread"), OBFUSCATE_BNM("()Landroid/app/ActivityThread;"))), env->GetMethodID(activityThread, OBFUSCATE_BNM("getApplication"), OBFUSCATE_BNM("()Landroid/app/Application;")));
        auto appInfo = env->CallObjectMethod(context, env->GetMethodID(env->GetObjectClass(context), OBFUSCATE_BNM("getApplicationInfo"), OBFUSCATE_BNM("()Landroid/content/pm/ApplicationInfo;")));
        std::string path = env->GetStringUTFChars((jstring)env->GetObjectField(appInfo, env->GetFieldID(env->GetObjectClass(appInfo), OBFUSCATE_BNM("nativeLibraryDir"), OBFUSCATE_BNM("Ljava/lang/String;"))), nullptr);
        auto libPath = str2char(path + OBFUSCATE_BNM("/libil2cpp.so"));
        auto dl = BNM_dlopen(libPath, RTLD_LAZY);
        if (!BNM_Internal::InitDlLib(dl, libPath)) LOGEBNM(OBFUSCATE_BNM("Can't hard bypass!"));
    }
    namespace External {
        [[maybe_unused]] void LoadBNM(void *dl) {
            if (BNM_Internal::InitDlLib(dl, nullptr, true)) {
                BNM_Internal::InitIl2cppMethods();
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
                BNM_Internal::InitNewClasses();
#endif
                BNM_Internal::LibLoaded = true;
                if (BNM_Internal::OnIl2CppLoadedEvent) BNM_Internal::OnIl2CppLoadedEvent();
            } else LOGWBNM(OBFUSCATE_BNM("[External::LoadBNM] dl is null or wrong, can't load BNM"));
        }
        [[maybe_unused]] void ForceLoadBNM(void *dl) {
            BNM_Internal::dlLib = dl;
            BNM_Internal::InitIl2cppMethods();
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
            BNM_Internal::InitNewClasses();
#endif
            BNM_Internal::LibLoaded = true;
            if (BNM_Internal::OnIl2CppLoadedEvent) BNM_Internal::OnIl2CppLoadedEvent();
        }
    }
}
namespace BNM_Internal {
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
    struct {
        void addClass(IL2CPP::Il2CppImage *image, IL2CPP::Il2CppClass *cls) {
            return addClass((BNM_PTR)image, cls);
        }
        void addClass(BNM_PTR image, IL2CPP::Il2CppClass *cls) {
            map[image].emplace_back(cls);
        }
        void forEachByImage(BNM_PTR image, const std::function<bool(IL2CPP::Il2CppClass *)> &func) {
            if (map[image].empty()) return;
            for (auto &item: map[image]) {
                if (func(item))
                    break;
            }
        }
        void forEachByImage(IL2CPP::Il2CppImage *image, const std::function<bool(IL2CPP::Il2CppClass *)> &func) {
            return forEachByImage((BNM_PTR)image, func);
        }
    
        void forEach(const std::function<bool(IL2CPP::Il2CppImage *, std::vector<IL2CPP::Il2CppClass *>)> &func) {
            for (auto [img, classes] : map) {
                if (func((IL2CPP::Il2CppImage *)img, classes))
                    break;
            }
        }
    private:
        std::map<BNM_PTR, std::vector<IL2CPP::Il2CppClass *>> map;
    } BNMClassesMap;
#define BNM_I2C_NEW(type, ...) new IL2CPP::type(__VA_ARGS__)
    IL2CPP::Il2CppImage *makeOrGetImage(NEW_CLASSES::NewClass *cls) {
        DO_API(IL2CPP::Il2CppImage *, il2cpp_assembly_get_image, (const IL2CPP::Il2CppAssembly *));
        AssemblyVector *LoadedAssemblies = Assembly$$GetAllAssemblies();
        for (auto assembly : *LoadedAssemblies) {
            IL2CPP::Il2CppImage *img = il2cpp_assembly_get_image(assembly);
            if (!strcmp(img->nameNoExt, cls->DllName)) return img;
        }
        auto nameLen = strlen(cls->DllName) + 1;
        auto newImg = BNM_I2C_NEW(Il2CppImage);
        newImg->nameNoExt = new char[nameLen];
        memset((void *)newImg->nameNoExt, 0, nameLen);
        strcpy((char *)newImg->nameNoExt, cls->DllName);
        auto extLen = nameLen + 4;
        newImg->name = new char[extLen];
        memset((void *)newImg->name, 0, extLen);
        strcpy((char *)newImg->name, cls->DllName);
        strcat((char *)newImg->name, OBFUSCATE_BNM(".dll"));
#if UNITY_VER > 182
        newImg->assembly = nullptr;
        newImg->customAttributeCount = 0;
#if UNITY_VER < 201
        newImg->customAttributeStart = -1;
#endif
#endif
#if UNITY_VER > 201
        auto handle  = (IL2CPP::Il2CppImageDefinition *)malloc(sizeof(IL2CPP::Il2CppImageDefinition));
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
        newImg->typeStart = -1;
        newImg->exportedTypeStart = -1;
#endif
        newImg->typeCount = 0;
        newImg->exportedTypeCount = 0;
        newImg->token = 1;
        auto newAsm = BNM_I2C_NEW(Il2CppAssembly);
#if UNITY_VER > 174
        newAsm->image = newImg;
        newAsm->image->assembly = newAsm;
        newAsm->aname.name = newImg->name;
#else
        static int newAsmCount = 1;
        newImg->assemblyIndex = newAsm->imageIndex = -newAsmCount;
        newAsmCount++;
#endif
        newAsm->aname.major = 0;
        newAsm->aname.minor = 0;
        newAsm->aname.build = 0;
        newAsm->aname.revision = 0;
        newImg->nameToClassHashTable = (decltype(newImg->nameToClassHashTable)) -0x424e4d;
        newAsm->referencedAssemblyStart = -1;
        newAsm->referencedAssemblyCount = 0;
        Assembly$$GetAllAssemblies()->push_back(newAsm);
        LOGIBNM(OBFUSCATE_BNM("Added new assembly: [%s]"), cls->DllName);
        return newImg;
    }
    bool isBNMType(IL2CPP::Il2CppType *type) {
        if (CheckObj(type->data.dummy)) return ((BNMTypeData *)type->data.dummy)->bnm == -0x424e4d;
        return false;
    }
    IL2CPP::Il2CppClass *Class$$FromIl2CppType(IL2CPP::Il2CppType *type) {
        if (!type) return nullptr;
        if (isBNMType(type)) return ((BNMTypeData *)type->data.dummy)->cls;
        return old_Class$$FromIl2CppType(type);
    }
    IL2CPP::Il2CppClass *Class$$FromName(IL2CPP::Il2CppImage *image, const char *_namespace, const char *name) {
        if (!image || !name[0]) return nullptr;
        IL2CPP::Il2CppClass *ret = nullptr;
        if (image->nameToClassHashTable != (decltype(image->nameToClassHashTable))-0x424e4d)
            ret = old_Class$$FromName(image, _namespace, name);
        if (!ret)
            BNMClassesMap.forEachByImage(image, [_namespace, name, &ret](IL2CPP::Il2CppClass *BNM_class) -> bool {
                if (!strcmp(_namespace, BNM_class->namespaze) && !strcmp(name, BNM_class->name)) {
                    ret = BNM_class;
                    return true;
                }
                return false;
            });
        return ret;
    }
    // Need due Image and Assembly in 2017.x- has index instead of pointer to Image and Assembly
    #if UNITY_VER <= 174
    IL2CPP::Il2CppImage *(*old_GetImageFromIndex)(IL2CPP::ImageIndex index);
    IL2CPP::Il2CppImage *new_GetImageFromIndex(IL2CPP::ImageIndex index) {
        if (index < 0) {
            IL2CPP::Il2CppImage *ret = nullptr;
            BNMClassesMap.forEach([index, &ret](IL2CPP::Il2CppImage *img, const std::vector<IL2CPP::Il2CppClass *> &classes) -> bool {
                if (img->assemblyIndex == index) {
                    ret = img;
                    return true;
                }
                return false;
            });
            return ret;
        }
        return old_GetImageFromIndex(index);
    }
    // All requests redirected to BNM and they are processed by it
    IL2CPP::Il2CppAssembly *new_Assembly_Load(const char *name) {
        DO_API(IL2CPP::Il2CppImage *, il2cpp_assembly_get_image, (const IL2CPP::Il2CppAssembly *));
        for (auto asmb : *Assembly$$GetAllAssemblies()) {
            auto img = il2cpp_assembly_get_image(asmb);
            if (!strcmp(img->name, name) || !strcmp(img->nameNoExt, name))
                return asmb;
        }
        return nullptr;
    }
    #endif
    void InitNewClasses() {
        if (!Classes4Add) return;
        for (auto& klass : *Classes4Add) {
            if (LoadClass::ClassExists(klass->myNamespace, klass->Name, klass->DllName)) {
                LOGWBNM(OBFUSCATE_BNM("[%s] [%s]::[%s] already exist, it can't be added it to il2cpp! Please check code."), klass->DllName, klass->myNamespace, klass->Name);
                continue;
            }
            auto typeByVal = BNM_I2C_NEW(Il2CppType);
            typeByVal->type = (IL2CPP::Il2CppTypeEnum)klass->classType;
            typeByVal->attrs = 0x1; // Public
            typeByVal->pinned = 0;
            typeByVal->byref = 0;
            typeByVal->num_mods = 0;
            auto typeThis = BNM_I2C_NEW(Il2CppType);
            typeThis->type = (IL2CPP::Il2CppTypeEnum)klass->classType;
            typeThis->attrs = 0x1; // Public
            typeThis->pinned = 0;
            typeThis->byref = 1;
            typeThis->num_mods = 0;
            IL2CPP::Il2CppImage *curImg = makeOrGetImage(klass);
            auto bnmTypeData = new BNMTypeData();
            typeByVal->data.dummy = (void *)bnmTypeData; // For il2cpp::vm::Class::FromIl2CppType
            typeThis->data.dummy = (void *)bnmTypeData; // For il2cpp::vm::Class::FromIl2CppType
            IL2CPP::Il2CppClass *parent = LoadClass(klass->BaseNamespace, klass->BaseName).GetIl2CppClass();
            if (!parent) parent = GetType<IL2CPP::Il2CppObject *>().ToIl2CppClass();
            Class$$Init(parent);
            std::vector<IL2CPP::VirtualInvokeData> newVTable;
            std::vector<IL2CPP::Il2CppRuntimeInterfaceOffsetPair> newInterOffsets;
            if (parent->interfaceOffsets)
                for (uint16_t i = 0; i < parent->interface_offsets_count; ++i)
                    newInterOffsets.push_back(parent->interfaceOffsets[i]);
            for (uint16_t i = 0; i < parent->vtable_count; ++i) newVTable.push_back(parent->vtable[i]);
            const IL2CPP::MethodInfo **methods = nullptr;
            if (!klass->Methods4Add.empty()) {
                bool needCtor = std::any_of(klass->Methods4Add.begin(), klass->Methods4Add.end(), [](NEW_CLASSES::NewMethod *met) {
                    return !strcmp(met->Name, OBFUSCATE_BNM(".ctor"));
                });
                methods = (const IL2CPP::MethodInfo **) calloc(klass->Methods4Add.size() + needCtor, sizeof(IL2CPP::MethodInfo *));
                for (int i = 0; i < klass->Methods4Add.size(); ++i) {
                    auto &method = klass->Methods4Add[i];
                    method->thisMethod = BNM_I2C_NEW(MethodInfo);
                    method->thisMethod->methodPointer = (IL2CPP::Il2CppMethodPointer)method->address;
                    method->thisMethod->invoker_method = (IL2CPP::InvokerMethod)method->invoker_address;
                    method->thisMethod->parameters_count = method->argsCount;
                    auto len = strlen(method->Name) + 1;
                    method->thisMethod->name = new char[len];
                    memset((void *)method->thisMethod->name, 0, len);
                    strcpy((char *)method->thisMethod->name, method->Name);
                    method->thisMethod->flags = 0x0006 | 0x0080; // PUBLIC | HIDE_BY_SIG
                    if (method->isStatic) method->thisMethod->flags |= 0x0010; // |= STATIC
                    method->thisMethod->is_generic = false;
                    method->thisMethod->return_type = method->ret_type.ToIl2CppType();
                    if (method->argsCount) {
#if UNITY_VER < 212
                        method->thisMethod->parameters = (IL2CPP::ParameterInfo *)calloc(method->argsCount, sizeof(IL2CPP::ParameterInfo));
                        auto newParam = (IL2CPP::ParameterInfo *)method->thisMethod->parameters;
                        for (int p = 0; p < method->argsCount; ++p) {
                            len = (OBFUSCATES_BNM("arg") + std::to_string(p)).size();
                            newParam->name = new char[len + 1];
                            memset((void *)newParam->name, 0, len + 1);
                            strcat((char *)newParam->name, OBFUSCATE_BNM("arg"));
                            strcpy((char *)newParam->name, std::to_string(p).c_str());
                            newParam->position = p;
                            if (!method->args_types.empty() && p < method->args_types.size()) {
                                newParam->parameter_type = method->args_types[p].ToIl2CppType();
                                newParam->token = newParam->parameter_type->attrs | p;
                            } else {
                                newParam->parameter_type = nullptr;
                                newParam->token = p;
                            }
                            ++newParam;
                        }
#else
                        auto **params = new IL2CPP::Il2CppType*[method->argsCount];
                        method->thisMethod->parameters = (const IL2CPP::Il2CppType **)params;
                        for (int p = 0; p < method->argsCount; ++p) {
                            auto newParam = BNM_I2C_NEW(Il2CppType);
                            if (p < method->args_types.size())
                                memcpy(newParam, method->args_types[p].ToIl2CppType(), sizeof(IL2CPP::Il2CppType));
                            newParam->data.dummy = nullptr;
                            params[p] = newParam;
                        }
#endif
                    }
#if UNITY_VER >= 212
                    else {
                        // We can't normally without hooks set args name, because names moved to metadata
                        method->thisMethod->methodMetadataHandle = nullptr;
                    }
#endif
                    methods[i] = method->thisMethod;
                }
                if (needCtor) {
                    auto method = BNM_I2C_NEW(MethodInfo);
                    method->name = new char[6];
                    memset((void *)method->name, 0, 6);
                    strcpy((char *)method->name, OBFUSCATE_BNM(".ctor"));
                    method->parameters_count = 0;
                    method->parameters = nullptr;
                    method->return_type = GetType<void>().ToIl2CppType();
                    method->is_generic = false;
                    method->flags = 0x0006 | 0x0080; // PUBLIC | HIDE_BY_SIG
                    method->methodPointer = EmptyCtor;
                    // We use static because this is just empty .ctor
                    method->invoker_method = (IL2CPP::InvokerMethod)NEW_CLASSES::GetNewStaticMethodCalls<decltype(&EmptyCtor)>::invoke;
                    methods[klass->Methods4Add.size()] = method;
                }
            }
#if UNITY_VER > 174
#define kls klass
#define typeSymbol *
#else
#define kls declaring_type
#define typeSymbol
#endif
            klass->thisClass = (IL2CPP::Il2CppClass *)malloc(sizeof(IL2CPP::Il2CppClass) + newVTable.size() * sizeof(IL2CPP::VirtualInvokeData));
            if (!klass->Methods4Add.empty()) {
                for (int i = 0; i < klass->Methods4Add.size(); ++i)
                    ((IL2CPP::MethodInfo *)methods[i])->kls = klass->thisClass;
                klass->thisClass->method_count = klass->Methods4Add.size();
                klass->thisClass->methods = methods;
                klass->Methods4Add.clear(); klass->Methods4Add.shrink_to_fit();
            } else {
                klass->thisClass->method_count = 0;
                klass->thisClass->methods = nullptr;
            }
            klass->thisClass->parent = parent;
            klass->thisClass->typeHierarchyDepth = parent->typeHierarchyDepth + 1;
            klass->thisClass->typeHierarchy = (IL2CPP::Il2CppClass **)calloc(klass->thisClass->typeHierarchyDepth, sizeof(IL2CPP::Il2CppClass *));
            memcpy(klass->thisClass->typeHierarchy, parent->typeHierarchy, parent->typeHierarchyDepth * sizeof(IL2CPP::Il2CppClass *));
            klass->thisClass->typeHierarchy[klass->thisClass->typeHierarchyDepth - 1] = klass->thisClass;
            klass->thisClass->image = curImg;
            auto len = strlen(klass->Name) + 1;
            klass->thisClass->name = new char[len];
            memset((void *)klass->thisClass->name, 0, len);
            strcpy((char *)klass->thisClass->name, klass->Name);
            len = strlen(klass->myNamespace) + 1;
            klass->thisClass->namespaze = new char[len];
            memset((void *)klass->thisClass->namespaze, 0, len);
            strcpy((char *)klass->thisClass->namespaze, klass->myNamespace);
            klass->thisClass->byval_arg = typeSymbol typeByVal;
            klass->thisClass->this_arg = typeSymbol typeThis;
#undef kls
#undef typeSymbol
            klass->thisClass->declaringType = nullptr;
            klass->thisClass->flags = klass->thisClass->parent->flags & ~0x00000080; // TYPE_ATTRIBUTE_ABSTRACT
            klass->thisClass->element_class = klass->thisClass;
            klass->thisClass->castClass = klass->thisClass;
#if UNITY_VER > 174
            klass->thisClass->klass = klass->thisClass;
#endif
            klass->thisClass->native_size = -1;
            klass->thisClass->actualSize = klass->size;
            klass->thisClass->instance_size = klass->size;
            klass->thisClass->element_size = 0;
            klass->thisClass->vtable_count = newVTable.size();
            for (int i = 0; i < newVTable.size(); ++i) klass->thisClass->vtable[i] = newVTable[i];
            newVTable.clear(); newVTable.shrink_to_fit();
            klass->thisClass->interface_offsets_count = newInterOffsets.size();
            klass->thisClass->interfaceOffsets = (IL2CPP::Il2CppRuntimeInterfaceOffsetPair *)(calloc(newInterOffsets.size(), sizeof(IL2CPP::Il2CppRuntimeInterfaceOffsetPair)));
            for (int i = 0; i < newInterOffsets.size(); ++i)
                klass->thisClass->interfaceOffsets[i] = newInterOffsets[i];
            newInterOffsets.clear(); newInterOffsets.shrink_to_fit();
            if (!klass->Interfaces.empty()) {
                klass->thisClass->interfaces_count = klass->Interfaces.size();
                klass->thisClass->implementedInterfaces = (IL2CPP::Il2CppClass **)calloc(klass->Interfaces.size(), sizeof(IL2CPP::Il2CppClass *));
                for (int i = 0; i < klass->Interfaces.size(); ++i) klass->thisClass->implementedInterfaces[i] = klass->Interfaces[i].ToIl2CppClass();
                klass->Interfaces.clear(); klass->Interfaces.shrink_to_fit();
            } else {
                klass->thisClass->interfaces_count = 0;
                klass->thisClass->implementedInterfaces = nullptr;
            }
            klass->thisClass->gc_desc = nullptr;
            klass->thisClass->generic_class = nullptr;
            klass->thisClass->genericRecursionDepth = 1;
            klass->thisClass->initialized = 1;
#if UNITY_VER > 182
            klass->thisClass->initialized_and_no_error = 1;
            klass->thisClass->initializationExceptionGCHandle = 0;
#if UNITY_VER < 212
            klass->thisClass->has_initialization_error = 0;
#endif
            klass->thisClass->naturalAligment = 0;
#endif
            klass->thisClass->init_pending = 0;
#if UNITY_VER < 202
            klass->thisClass->genericContainerIndex = -1;
#else
            klass->thisClass->genericContainerHandle = nullptr;
            klass->thisClass->typeMetadataHandle = nullptr;
#endif
#if UNITY_VER < 211
            klass->thisClass->valuetype = 1; // I can set to 0, but there are some other places where it using in il2cpp
#endif
            klass->thisClass->interopData = nullptr;
            klass->thisClass->nestedTypes = nullptr;
            klass->thisClass->properties = nullptr;
            klass->thisClass->rgctx_data = nullptr;
            klass->thisClass->has_references = 0;
            klass->thisClass->has_finalize = 0;
            klass->thisClass->size_inited = klass->thisClass->is_vtable_initialized = 1;
            klass->thisClass->has_cctor = 0;
            klass->thisClass->enumtype = 0;
            klass->thisClass->minimumAlignment = 8;
            klass->thisClass->is_generic = 0;
            klass->thisClass->rank = 0;
            klass->thisClass->nested_type_count = 0;
            klass->thisClass->thread_static_fields_offset = 0;
            klass->thisClass->thread_static_fields_size = -1;
            klass->thisClass->cctor_started = 0;
#if UNITY_VER >= 203
            klass->thisClass->size_init_pending = 0;
#endif
#if UNITY_VER < 212
            klass->thisClass->cctor_finished = 0;
#endif
            klass->thisClass->cctor_thread = 0;
            klass->thisClass->field_count = klass->Fields4Add.size();
            if (klass->thisClass->field_count > 0) {
                auto fields = (IL2CPP::FieldInfo *)calloc(klass->thisClass->field_count, sizeof(IL2CPP::FieldInfo));
                IL2CPP::FieldInfo *newField = fields;
                if (!klass->Fields4Add.empty()) {
                    for (auto &field : klass->Fields4Add) {
                        len = strlen(field->Name);
                        newField->name = new char[len];
                        memset((void *)newField->name, 0, len);
                        strcpy((char *)newField->name, field->Name);
                        newField->type = BNM_I2C_NEW(Il2CppType, *field->type.ToIl2CppType()); // Copy type
                        newField->parent = klass->thisClass;
                        newField->offset = field->offset;
                        ((IL2CPP::Il2CppType*)newField->type)->attrs |= field->attributes; // PUBLIC and STATIC if static
                        newField->token = newField->type->attrs | field->attributes;
                        newField++;
                    }
                    klass->Fields4Add.clear(); klass->Fields4Add.shrink_to_fit();
                    klass->thisClass->static_fields = (void *)klass->staticFieldsAddress;
                    klass->thisClass->static_fields_size = klass->staticFieldOffset;
                }
                klass->thisClass->fields = fields;
            } else {
                klass->thisClass->static_fields_size = 0;
                klass->thisClass->static_fields = nullptr;
                klass->thisClass->fields = nullptr;
            }
            bnmTypeData->cls = klass->thisClass;
            BNMClassesMap.addClass(curImg, klass->thisClass);
            klass->type = LoadClass(klass->thisClass);
            LOGIBNM(OBFUSCATE_BNM("[InitNewClasses] Added new class (%p): [%s]::[%s] parent is [%s]::[%s] to [%s]"), klass->thisClass, klass->thisClass->namespaze, klass->thisClass->name, parent->namespaze, parent->name, klass->thisClass->image->name);
        }
        Classes4Add->clear(); Classes4Add->shrink_to_fit();
        delete Classes4Add;
        Classes4Add = nullptr;
    }
#endif
    void Image$$GetTypes(IL2CPP::Il2CppImage *image, bool, TypeVector *target) {
        if (!image || !target) return;
        if (image->nameToClassHashTable != (decltype(image->nameToClassHashTable))-0x424e4d) {
            if (HasImageGetCls) {
                DO_API(IL2CPP::Il2CppClass *, il2cpp_image_get_class, (IL2CPP::Il2CppImage *, decltype(image->typeCount)));
                auto typeCount = image->typeCount;
                for (decltype(image->typeCount) i = 0; i < typeCount; ++i) {
                    auto type = il2cpp_image_get_class(image, i);
                    if (OBFUSCATES_BNM("<Module>") == type->name) continue;
                    target->push_back(type);
                }
            } else old_Image$$GetTypes(image, false, target);
        }
    #if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
        BNMClassesMap.forEachByImage(image, [&target](IL2CPP::Il2CppClass *BNM_class) -> bool {
            target->push_back(BNM_class);
            return false;
        });
    #endif
    }

    void InitIl2cppMethods() {
    #if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
        int count = 1;
    #elif defined(__i386__) || defined(__x86_64__)
        int count = 2;
    #endif
        //! il2cpp::vm::Class::Init GET
        if (!Class$$Init) {
            Class$$Init = (decltype(Class$$Init))  HexUtils::FindNext_B_BL_offset(HexUtils::FindNext_B_BL_offset((BNM_PTR) BNM_dlsym(dlLib, OBFUSCATE_BNM("il2cpp_array_new_specific")), count), count);
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::Init in lib: %p"), offsetInLib((void *)Class$$Init));
        }

        //! il2cpp_image_get_class CHECK (Unity 2018+)
        {
            DO_API(void,il2cpp_image_get_class,());
            HasImageGetCls = il2cpp_image_get_class != nullptr;
        }

        //! il2cpp::vm::Image::GetTypes HOOK AND GET (OR ONLY GET)
        if (!old_Image$$GetTypes && !HasImageGetCls) {
            DO_API(const IL2CPP::Il2CppImage *, il2cpp_get_corlib, ());
            DO_API(IL2CPP::Il2CppClass *, il2cpp_class_from_name, (const IL2CPP::Il2CppImage *, const char *, const char *));
            BNM_PTR GetTypesAdr = 0;
            auto assemblyClass = il2cpp_class_from_name(il2cpp_get_corlib(), OBFUSCATE_BNM("System.Reflection"), OBFUSCATE_BNM("Assembly"));
            GetTypesAdr = LoadClass(assemblyClass).GetMethodByName(OBFUSCATE_BNM("GetTypes"), 1).GetOffset(); // We can use LoadClass here by Il2CppClass, because for methods we need only it.
            const int sCount
    #if UNITY_VER >= 211
            = count;
    #elif UNITY_VER > 174
            = count + 1;
    #else
            = count + 2;
    #endif
            auto Image$$GetTypes_t = HexUtils::FindNext_B_BL_offset(HexUtils::FindNext_B_BL_offset(HexUtils::FindNext_B_BL_offset(GetTypesAdr, count), sCount), count);
    #if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
            HOOK(Image$$GetTypes_t, Image$$GetTypes, old_Image$$GetTypes);
    #else
            old_Image$$GetTypes = (decltype(old_Image$$GetTypes)) Image$$GetTypes_t;
    #endif
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Image::GetTypes in lib: %p"), offsetInLib((void *)Image$$GetTypes_t));
        } else if (HasImageGetCls) {
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] game has il2cpp_image_get_class. BNM will use it"));
        }
    #if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
        //! il2cpp::vm::Class::FromIl2CppType HOOK
        if (!old_Class$$FromIl2CppType) {
            auto from_type_adr = HexUtils::FindNext_B_BL_offset((BNM_PTR) BNM_dlsym(dlLib, OBFUSCATE_BNM("il2cpp_class_from_type")), count);
            HOOK(from_type_adr, Class$$FromIl2CppType, old_Class$$FromIl2CppType);
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::FromIl2CppType in lib: %p"), offsetInLib((void *)from_type_adr));
        }
        //! il2cpp::vm::Class::FromName HOOK
        if (!old_Class$$FromName) {
            auto from_name_adr = HexUtils::FindNext_B_BL_offset(HexUtils::FindNext_B_BL_offset((BNM_PTR) BNM_dlsym(dlLib, OBFUSCATE_BNM("il2cpp_class_from_name")), count), count);
            HOOK(from_name_adr, Class$$FromName, old_Class$$FromName);
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::FromName in lib: %p"), offsetInLib((void *)from_name_adr));
        }
    #if UNITY_VER <= 174
        //! il2cpp::vm::MetadataCache::GetImageFromIndex HOOK
            if (!old_GetImageFromIndex) {
                auto GetImageFromIndexOffset = HexUtils::FindNext_B_BL_offset(HexUtils::FindNext_B_BL_offset((BNM_PTR)BNM_dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_assembly_get_image")), count), count);
                HOOK(GetImageFromIndexOffset, new_GetImageFromIndex, old_GetImageFromIndex);
                LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::MetadataCache::GetImageFromIndex in lib: %p"), offsetInLib((void *)GetImageFromIndexOffset));
            }
            static void *old_AssemblyLoad;
            //! il2cpp::vm::Assembly::Load HOOK
            if (!old_AssemblyLoad) {
                BNM_PTR AssemblyLoadOffset = HexUtils::FindNext_B_BL_offset((BNM_PTR)BNM_dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_domain_assembly_open")), count);
                HOOK(AssemblyLoadOffset, new_Assembly_Load, old_AssemblyLoad);
                LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::Load in lib: %p"), offsetInLib((void *)AssemblyLoadOffset));
            }
    #endif
    #endif
        //! il2cpp::vm::Assembly::GetAllAssemblies GET
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
                BNM_PTR GetTypesAdr = HexUtils::FindNext_B_BL_offset(getAssembly.GetOffset(), count);
                Assembly$$GetAllAssemblies = (AssemblyVector *(*)())(HexUtils::FindNext_B_BL_offset(GetTypesAdr, sCount));
                LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::GetAllAssemblies by AppDomain in lib: %p"), offsetInLib((void *)Assembly$$GetAllAssemblies));
            } else {
    #endif
            auto adr = (BNM_PTR) BNM_dlsym(dlLib, OBFUSCATE_BNM("il2cpp_domain_get_assemblies"));
            Assembly$$GetAllAssemblies = (AssemblyVector *(*)())(HexUtils::FindNext_B_BL_offset(adr, count));
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::GetAllAssemblies by domain in lib: %p"), offsetInLib((void *)Assembly$$GetAllAssemblies));
    #ifdef BNM_USE_APPDOMAIN
            }
    #endif
        }
    }
    void BNM_il2cpp_init(const char *domain_name) {
        old_BNM_il2cpp_init(domain_name);
        InitIl2cppMethods();
    #if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
        InitNewClasses();
    #endif
        LibLoaded = true;
        if (OnIl2CppLoadedEvent) OnIl2CppLoadedEvent();
    }
#ifndef BNM_DISABLE_AUTO_LOAD
    [[maybe_unused]] __attribute__((constructor))
    void PrepareBNM() {
        auto lib = BNM_dlopen(OBFUSCATE_BNM("libil2cpp.so"), RTLD_LAZY);
        if (InitDlLib(lib)) return;
        else BNM_dlclose(lib);
        std::thread([]() {
            do {
                if (hardBypass) break;
                auto lib = BNM_dlopen(OBFUSCATE_BNM("libil2cpp.so"), RTLD_LAZY);
                if (InitDlLib(lib)) break;
                else BNM_dlclose(lib);
            } while (true);
        }).detach();
    }
#endif

    bool InitDlLib(void *dl, const char* path, bool external) {
        if (!dl) return false;
        void *init = BNM_dlsym(dl, OBFUSCATE_BNM("il2cpp_init"));
        if (init) {
            Dl_info info;
            BNM_dladdr(init, &info);
            if (!path) {
                auto l = strlen(info.dli_fname) + 1;
                auto s = new char[l];
                memset((void *)s, 0, l);
                strcpy(s, info.dli_fname);
                LibAbsolutePath = s;
            } else LibAbsolutePath = path;
            LibAbsoluteAddress = (BNM_PTR)info.dli_fbase;
            if (!external) HOOK(init, BNM_il2cpp_init, old_BNM_il2cpp_init);
            dlLib = dl;
        }
        return init;
    }
}
#undef DO_API