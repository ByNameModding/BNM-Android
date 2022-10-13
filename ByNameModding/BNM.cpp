#include "BNM.hpp"
#include "BNM_data/utf8.h"

/********** BNM DATA **************/

static bool BNM_LibLoaded = false;
static void *BNM_dlLib{};
static bool BNM_hardBypass = false;
static bool BNM_hardBypassed = false;
static const char *BNM_LibAbsolutePath{};
static DWORD BNM_LibAbsoluteAddress{};
static bool HasImageGetCls = false;
BNM::AssemblyVector *(*Assembly$$GetAllAssemblies)(){};
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES // Speed up IDE if c++ lower then c++17
void Image$$GetTypes(BNM::IL2CPP::Il2CppImage* image, bool exportedOnly, BNM::TypeVector* target);
void (*old_Image$$GetTypes)(BNM::IL2CPP::Il2CppImage* image, bool exportedOnly, BNM::TypeVector* target){};
BNM::IL2CPP::Il2CppClass* (*old_Class$$FromIl2CppType)(BNM::IL2CPP::Il2CppType* type){};
BNM::IL2CPP::Il2CppClass* Class$$FromIl2CppType(BNM::IL2CPP::Il2CppType* type);
BNM::IL2CPP::Il2CppClass* (*old_Class$$FromName)(BNM::IL2CPP::Il2CppImage* image, const char* ns, const char *name){};
BNM::IL2CPP::Il2CppClass* Class$$FromName(BNM::IL2CPP::Il2CppImage* image, const char* _namespace, const char *name);
static std::vector<BNM::NEW_CLASSES::NewClass *> *Classes4Add{};
#else
void (*Image$$GetTypes)(BNM::IL2CPP::Il2CppImage* image, bool exportedOnly, TypeVector* target){};
#endif
void (*Class$$Init)(BNM::IL2CPP::Il2CppClass *klass){};
/********** BNM MACRO CODE **************/

void *get_il2cpp() { return BNM_dlLib; }
bool BNM::Il2cppLoaded() { return BNM_LibLoaded; }

/********** BNM MAIN CODE **************/

#define DO_API(r, n, p) auto (n) = (r (*) p)BNM_dlsym(get_il2cpp(), OBFUSCATE_BNM(#n))

BNM::IL2CPP::Il2CppClass *GetClassFromName(const std::string& _namespace, const std::string& _name) {
    DO_API(BNM::IL2CPP::Il2CppImage*, il2cpp_assembly_get_image, (BNM::IL2CPP::Il2CppAssembly*));
    for (auto assembly : *Assembly$$GetAllAssemblies()) {
        if (!BNM::CheckObj(assembly)) continue;
        BNM::TypeVector classes;
        auto img = il2cpp_assembly_get_image(assembly);
        Image$$GetTypes(img, false, &classes);
        for (auto cls : classes) {
            if (!BNM::CheckObj(cls)) continue;
            Class$$Init(cls);
            if (cls->name == _name && cls->namespaze == _namespace)
                return (BNM::IL2CPP::Il2CppClass *)cls;
        }
    }
    return nullptr;
}

BNM::LoadClass::LoadClass() = default;
BNM::LoadClass::LoadClass(const IL2CPP::Il2CppClass *clazz) { klass = (IL2CPP::Il2CppClass *)clazz; }
BNM::LoadClass::LoadClass(const IL2CPP::Il2CppObject* obj) { klass = obj->klass; }
[[maybe_unused]] BNM::LoadClass::LoadClass(const IL2CPP::Il2CppType* type) {
    DO_API(IL2CPP::Il2CppClass*, il2cpp_class_from_il2cpp_type, (const IL2CPP::Il2CppType*));
    klass = il2cpp_class_from_il2cpp_type(type);
}

[[maybe_unused]] BNM::LoadClass::LoadClass(const MonoType *type) {
    DO_API(IL2CPP::Il2CppClass*, il2cpp_class_from_il2cpp_type, (const IL2CPP::Il2CppType*));
    klass = il2cpp_class_from_il2cpp_type(type->type);
}
BNM::LoadClass::LoadClass(const std::string& _namespace, const std::string& clazz) {
    klass = GetClassFromName(_namespace, clazz);
    if (!klass)
        LOGIBNM(OBFUSCATE_BNM("Class: [%s].[%s] - not found (without dll)"), _namespace.c_str(), clazz.c_str());
}
BNM::LoadClass::LoadClass(const std::string& _namespace, const std::string& _name, const std::string& dllName) {
    DO_API(IL2CPP::Il2CppImage*, il2cpp_assembly_get_image, (const IL2CPP::Il2CppAssembly*));
    IL2CPP::Il2CppImage *dll = nullptr;
    for (auto assembly : *Assembly$$GetAllAssemblies())
        if (dllName == il2cpp_assembly_get_image(assembly)->name)
            dll = il2cpp_assembly_get_image(assembly);
    if (!dll) {
        LOGIBNM(OBFUSCATE_BNM("Dll: \"%s\" - not found"), dllName.c_str());
        klass = nullptr;
        return;
    }
    TypeVector classes;
    Image$$GetTypes(dll, false, &classes);
    for (auto cls : classes) {
        if (!cls) continue;
        Class$$Init(cls);
        if (cls->name == _name && cls->namespaze == _namespace) {
            klass = cls;
            break;
        }
    }
    if (!klass)
        LOGIBNM(OBFUSCATE_BNM("Class: [%s].[%s] - not found (with dll)"), _namespace.c_str(), _name.c_str());
}
#ifdef BNM_DEPRECATED
[[maybe_unused]] BNM::IL2CPP::FieldInfo *BNM::LoadClass::GetFieldInfoByName(const std::string& name) const {
    if (!klass) return nullptr;
    return GetFieldByName(name).myInfo;
}
[[maybe_unused]] DWORD BNM::LoadClass::GetFieldOffset(const std::string& name) const {
    if (!klass) return 0;
    return GetFieldByName(name).GetOffset();
}
[[maybe_unused]] BNM::IL2CPP::MethodInfo *BNM::LoadClass::GetMethodInfoByName(const std::string& name, int parameters) const {
    return GetMethodByName(name, parameters).GetInfo();
}
[[maybe_unused]] DWORD BNM::LoadClass::GetMethodOffsetByName(const std::string& name, int parameters) const {
    return GetMethodByName(name, parameters).GetOffset();
}
[[maybe_unused]] BNM::IL2CPP::MethodInfo *BNM::LoadClass::GetMethodInfoByName(const std::string& name, const std::vector<std::string>& params_names) const {
    return GetMethodByName(name, params_names).GetInfo();
}
[[maybe_unused]] DWORD BNM::LoadClass::GetMethodOffsetByName(const std::string& name, const std::vector<std::string>& params_names) const {
    return GetMethodByName(name, params_names).GetOffset();
}
[[maybe_unused]] BNM::IL2CPP::MethodInfo *BNM::LoadClass::GetMethodInfoByName(const std::string &name, const std::vector<std::string> &params_names, const std::vector<IL2CPP::Il2CppType *> &params_types) const {
    return GetMethodByName(name, params_names, params_types).GetInfo();
}
[[maybe_unused]] DWORD BNM::LoadClass::GetMethodOffsetByName(const std::string &name, const std::vector<std::string> &params_names, const std::vector<IL2CPP::Il2CppType *> &params_types) const {
    return GetMethodByName(name, params_names, params_types).GetOffset();
}
[[maybe_unused]] BNM::IL2CPP::MethodInfo *BNM::LoadClass::GetMethodInfoByName(const std::string &name, const std::vector<IL2CPP::Il2CppType *> &params_types) const {
    return GetMethodByName(name, params_types).GetInfo();
}
[[maybe_unused]] DWORD BNM::LoadClass::GetMethodOffsetByName(const std::string &name, const std::vector<IL2CPP::Il2CppType *> &params_types) const {
    return GetMethodByName(name, params_types).GetOffset();
}
#endif
BNM::Method<void> BNM::LoadClass::GetMethodByName(const std::string &name, int parameters) const {
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
BNM::Method<void> BNM::LoadClass::GetMethodByName(const std::string &name, const std::vector<std::string> &params_names) const {
    if (!klass) return {};
    TryInit();
    DO_API(const char*, il2cpp_method_get_param_name, (IL2CPP::MethodInfo* method, uint32_t index));
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
BNM::Method<void> BNM::LoadClass::GetMethodByName(const std::string &name, const std::vector<std::string> &params_names, const std::vector<BNM::IL2CPP::Il2CppType *> &params_types) const {
    if (!klass) return {};
    TryInit();
    DO_API(IL2CPP::Il2CppClass*, il2cpp_class_from_il2cpp_type, (const IL2CPP::Il2CppType*));
    DO_API(const char*, il2cpp_method_get_param_name, (IL2CPP::MethodInfo* method, uint32_t index));
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
BNM::Method<void> BNM::LoadClass::GetMethodByName(const std::string& name, const std::vector<IL2CPP::Il2CppType *>& params_types) const {
    if (!klass) return {};
    TryInit();
    DO_API(IL2CPP::Il2CppClass*, il2cpp_class_from_il2cpp_type, (const IL2CPP::Il2CppType*));
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
[[maybe_unused]] std::vector<BNM::IL2CPP::MethodInfo*> BNM::LoadClass::GetMethodsInfo(bool includeParent) const {
    if (!klass) return {};
    TryInit();
    std::vector<IL2CPP::MethodInfo*> ret;
    auto curClass = klass;
    do {
        for (int i = 0; i < curClass->method_count; ++i) ret.emplace_back(BNM::Method(curClass->methods[i]).GetInfo());
        if (includeParent) curClass = curClass->parent;
        else curClass = nullptr;
    } while (curClass);
    return ret;
}

[[maybe_unused]] std::vector<BNM::LoadClass> BNM::LoadClass::GetInnerClasses(bool includeParent) const {
    if (!klass) return {};
    TryInit();
    std::vector<BNM::LoadClass> ret;
    auto curClass = klass;
    do {
        for (int i = 0; i < curClass->nested_type_count; ++i) ret.emplace_back(curClass->nestedTypes[i]);
        if (includeParent) curClass = curClass->parent;
        else curClass = nullptr;
    } while (curClass);
    return ret;
}
[[maybe_unused]] std::vector<BNM::IL2CPP::FieldInfo*> BNM::LoadClass::GetFieldsInfo(bool includeParent) const {
    if (!klass) return {};
    TryInit();
    std::vector<BNM::IL2CPP::FieldInfo*> ret;
    auto curClass = klass;
    do {
        for (int i = 0; i < curClass->field_count; ++i) ret.emplace_back(curClass->fields + i);
        if (includeParent) curClass = curClass->parent;
        else curClass = nullptr;
    } while (curClass);
    return ret;
}

[[maybe_unused]] BNM::LoadClass BNM::LoadClass::GetInnerClass(const std::string& _name) const {
    for (auto cls : GetInnerClasses()) if (_name == cls.klass->name) return cls;
    return {};
}

BNM::IL2CPP::Il2CppType *BNM::LoadClass::GetIl2CppType() const {
    if (!klass) return nullptr;
    TryInit();
#if UNITY_VER > 174
    return (BNM::IL2CPP::Il2CppType *)&klass->byval_arg;
#else
    return (IL2CPP::Il2CppType *)klass->byval_arg;
#endif
}
[[maybe_unused]] BNM::MonoType *BNM::LoadClass::GetMonoType() const {
    if (!klass) return nullptr;
    TryInit();
    DO_API(BNM::IL2CPP::Il2CppObject*, il2cpp_type_get_object, (BNM::IL2CPP::Il2CppType*));
    return (BNM::MonoType *) il2cpp_type_get_object(GetIl2CppType());
}
void *BNM::LoadClass::CreateNewInstance() const {
    if (!klass) return nullptr;
    TryInit();
    DO_API(BNM::IL2CPP::Il2CppObject*, il2cpp_object_new, (BNM::IL2CPP::Il2CppClass*));
    return (void *) il2cpp_object_new(klass);
}
BNM::IL2CPP::Il2CppClass *BNM::LoadClass::GetIl2CppClass() const {
    TryInit();
    return klass;
}
[[maybe_unused]] std::string BNM::LoadClass::GetClassName() const {
    if (klass) {
        TryInit();
        return OBFUSCATES_BNM("[") + klass->namespaze + OBFUSCATES_BNM("]::[") + klass->name + OBFUSCATES_BNM("]");
    }
    return OBFUSCATES_BNM("Uninitialized class");
}
#ifdef BNM_DEPRECATED
BNM::LoadClass BNM::LoadClass::WithMethodName(const std::string& _namespace, const std::string& _name, const std::string& methodName) {
    DO_API(BNM::IL2CPP::Il2CppImage*, il2cpp_assembly_get_image, (const BNM::IL2CPP::Il2CppAssembly*));
    for (auto assembly : *Assembly$$GetAllAssemblies()) {
        BNM::TypeVector classes;
        if (!BNM::CheckObj(assembly)) continue;
        Image$$GetTypes(il2cpp_assembly_get_image(assembly), false, &classes);
        for (auto cls : classes) {
            if (!BNM::CheckObj(cls)) continue;
            Class$$Init(cls);
            if (BNM::CheckObj(cls->name) == _name && BNM::CheckObj(cls->namespaze) == _namespace) {
                for (int i = 0; i < cls->method_count; ++i) {
                    auto method = (IL2CPP::MethodInfo *)cls->methods[i];
                    if (method && methodName == method->name)
                        return {cls};
                }
            }
        }
    }
    return {};
}
#endif
void BNM::LoadClass::TryInit() const {
    Class$$Init(klass);
}
BNM::IL2CPP::Il2CppArray *BNM::LoadClass::ArrayNew(IL2CPP::Il2CppClass* cls, IL2CPP::il2cpp_array_size_t length) {
    DO_API(IL2CPP::Il2CppArray*, il2cpp_array_new, (IL2CPP::Il2CppClass*, IL2CPP::il2cpp_array_size_t));
    return il2cpp_array_new(cls, length);
}
BNM::IL2CPP::Il2CppObject *BNM::LoadClass::ObjNew(IL2CPP::Il2CppClass* cls) {
    DO_API(IL2CPP::Il2CppObject*, il2cpp_object_new, (IL2CPP::Il2CppClass*));
    return il2cpp_object_new(cls);
}
BNM::IL2CPP::Il2CppObject *BNM::LoadClass::ObjBox(IL2CPP::Il2CppClass* klass, void *data) {
    DO_API(IL2CPP::Il2CppObject*, il2cpp_value_box, (IL2CPP::Il2CppClass*, void*));
    return il2cpp_value_box(klass, data);
}

BNM::Field<int> BNM::LoadClass::GetFieldByName(const std::string &name) const {
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

bool BNM::LoadClass::ClassExists(const std::string& _namespace, const std::string& _name, const std::string& dllName) {
    DO_API(IL2CPP::Il2CppImage*, il2cpp_assembly_get_image, (const IL2CPP::Il2CppAssembly*));
    IL2CPP::Il2CppImage *dll = nullptr;
    for (auto assembly : *Assembly$$GetAllAssemblies())
        if (dllName == il2cpp_assembly_get_image(assembly)->name)
            dll = il2cpp_assembly_get_image(assembly);
    if (!dll)
        return false;
    TypeVector classes;
    Image$$GetTypes(dll, false, &classes);
    return std::any_of(classes.begin(), classes.end(), [&_namespace, &_name](IL2CPP::Il2CppClass* cls){ return cls->namespaze == _namespace && cls->name == _name; });
}

[[maybe_unused]] BNM::LoadClass BNM::LoadClass::GetArrayClass() const {
    if (!klass) return {};
    TryInit();
    DO_API(BNM::IL2CPP::Il2CppClass*, il2cpp_array_class_get, (BNM::IL2CPP::Il2CppClass*, uint32_t));
    return il2cpp_array_class_get(klass, 1);
}

[[maybe_unused]] BNM::Property<bool> BNM::LoadClass::GetPropertyByName(const std::string &name, bool warning) {
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

void BNM::PRIVATE_FILED_UTILS::GetStaticValue(IL2CPP::FieldInfo* info, void *value) {
    DO_API(void, il2cpp_field_static_get_value, (IL2CPP::FieldInfo*, void*));
    return il2cpp_field_static_get_value(info, value);
}
void BNM::PRIVATE_FILED_UTILS::SetStaticValue(IL2CPP::FieldInfo* info, void *value) {
    DO_API(void, il2cpp_field_static_set_value, (IL2CPP::FieldInfo*, void*));
    return il2cpp_field_static_set_value(info, value);
}
BNM::LoadClass BNM::TypeFinder::ToLC() const {
#ifdef BNM_DEPRECATED
    auto ret = withMethodName ? LoadClass::WithMethodName(_namespace, name, methodName) : LoadClass(_namespace, name);
#else
    auto ret = LoadClass(_namespace, name);
#endif
    return isArray ? ret.GetArrayClass() : ret;
}
BNM::IL2CPP::Il2CppType *BNM::TypeFinder::ToIl2CppType() const {
    return (IL2CPP::Il2CppType *)ToLC().GetIl2CppType();
}
BNM::IL2CPP::Il2CppClass* BNM::TypeFinder::ToIl2CppClass() const {
    return ToLC().GetIl2CppClass();
}
BNM::TypeFinder::operator LoadClass() const {
    return ToLC();
}
BNM::TypeFinder::operator IL2CPP::Il2CppType*() const {
    return ToIl2CppType();
}
BNM::TypeFinder::operator IL2CPP::Il2CppClass*() const {
    return ToIl2CppClass();
}
#if __cplusplus < 201703
namespace std {
    template< class T, class U >
    constexpr bool is_same_v = is_same<T, U>::value; // For C++11 and C++14
    template <class _Tp>
    constexpr bool is_pointer_v = is_pointer<_Tp>::value; // For C++11 and C++14
}
#endif

#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
BNM::NEW_CLASSES::NewMethod::NewMethod() = default;
BNM::NEW_CLASSES::NewField::NewField() = default;
BNM::NEW_CLASSES::NewClass::NewClass() = default;
void BNM::NEW_CLASSES::NewClass::AddNewField(NewField *field, bool isStatic) {
    if (!isStatic) Fields4Add.push_back(field);
    else {
        if (staticFieldOffset == 0x0) staticFieldsAddress = field->cppOffset;
        field->offset = staticFieldOffset;
        staticFieldOffset += field->size;
        StaticFields4Add.push_back(field);
    }
}
void BNM::NEW_CLASSES::NewClass::AddNewMethod(NewMethod *method) {
    Methods4Add.push_back(method);
}
struct BNMTypeData {
    BNMTypeData() = default;
    DWORD bnm = -0x424e4d;
    BNM::IL2CPP::Il2CppClass *cls{};
};

struct {
    void addClass(BNM::IL2CPP::Il2CppImage* image, BNM::IL2CPP::Il2CppClass *cls) {
        return addClass((DWORD)image, cls);
    }
    void addClass(DWORD image, BNM::IL2CPP::Il2CppClass *cls) {
        std::lock_guard<std::shared_mutex> lock(mtx);
        map[image].emplace_back(cls);
    }
    void forEachByImage(DWORD image, const std::function<bool(BNM::IL2CPP::Il2CppClass *)>& func) {
        std::lock_guard<std::shared_mutex> lock(mtx);
        if (map[image].empty()) return;
        for (auto &item: map[image]) {
            if (func(item))
                break;
        }
    }
    void forEachByImage(BNM::IL2CPP::Il2CppImage* image, const std::function<bool(BNM::IL2CPP::Il2CppClass *)>& func) {
        return forEachByImage((DWORD)image, func);
    }

    void forEach(const std::function<bool(BNM::IL2CPP::Il2CppImage *, std::vector<BNM::IL2CPP::Il2CppClass *>)>& func) {
        std::lock_guard<std::shared_mutex> lock(mtx);
        for (auto [img, classes] : map) {
            if (func((BNM::IL2CPP::Il2CppImage *)img, classes))
                break;
        }
    }
private:
    std::shared_mutex mtx;
    std::map<DWORD, std::vector<BNM::IL2CPP::Il2CppClass *>> map;
} BNMClassesMap;
BNM::IL2CPP::Il2CppImage *makeOrGetImage(BNM::NEW_CLASSES::NewClass *cls) {
    std::string name = cls->DllName;
    DO_API(BNM::IL2CPP::Il2CppImage*, il2cpp_assembly_get_image, (const BNM::IL2CPP::Il2CppAssembly*));
    BNM::AssemblyVector *LoadedAssemblies = Assembly$$GetAllAssemblies();
    for (auto assembly : *LoadedAssemblies) {
        BNM::IL2CPP::Il2CppImage *img = il2cpp_assembly_get_image(assembly);
        if (img->nameNoExt == name)
            return img;
    }
    auto newImg = new BNM::IL2CPP::Il2CppImage();
    newImg->nameNoExt = (char*)calloc(name.size(), sizeof(char));
    strcpy((char*)newImg->nameNoExt, name.c_str());
    std::string nameExt = (name + OBFUSCATES_BNM(".dll"));
    newImg->name = (char*)calloc(nameExt.size(), sizeof(char));
    strcpy((char*)newImg->name, nameExt.c_str());
#if UNITY_VER > 182
    newImg->assembly = nullptr;
    newImg->customAttributeCount = 0;
#if UNITY_VER < 201
    newImg->customAttributeStart = -1;
#endif
#endif
#if UNITY_VER > 201
    newImg->metadataHandle = (BNM::IL2CPP::Il2CppMetadataImageHandle)malloc(sizeof(BNM::IL2CPP::Il2CppImageDefinition));
    ((BNM::IL2CPP::Il2CppImageDefinition*)newImg->metadataHandle)->typeStart = -1;
    ((BNM::IL2CPP::Il2CppImageDefinition*)newImg->metadataHandle)->exportedTypeStart = -1;
    ((BNM::IL2CPP::Il2CppImageDefinition*)newImg->metadataHandle)->typeCount = 0;
    ((BNM::IL2CPP::Il2CppImageDefinition*)newImg->metadataHandle)->exportedTypeCount = 0;
    ((BNM::IL2CPP::Il2CppImageDefinition*)newImg->metadataHandle)->nameIndex = -1;
    ((BNM::IL2CPP::Il2CppImageDefinition*)newImg->metadataHandle)->assemblyIndex = -1;
    ((BNM::IL2CPP::Il2CppImageDefinition*)newImg->metadataHandle)->customAttributeCount = 0;
    ((BNM::IL2CPP::Il2CppImageDefinition*)newImg->metadataHandle)->customAttributeStart = -1;
#else
    newImg->typeStart = -1;
    newImg->exportedTypeStart = -1;
#endif
    newImg->typeCount = 0;
    newImg->exportedTypeCount = 0;
    newImg->token = 1;
    auto newAsm = new BNM::IL2CPP::Il2CppAssembly();
#if UNITY_VER > 174
    newAsm->image = newImg;
    newAsm->image->assembly = newAsm;
    newAsm->aname.name = (char*)calloc(name.size(), sizeof(char));
    strcpy((char*)newAsm->aname.name, name.c_str());
#else
    static int newAsmCount = 1;
    newImg->assemblyIndex = newAsm->imageIndex = -newAsmCount;
    newAsmCount++;
#endif
    newAsm->aname.major = 0;
    newAsm->aname.minor = 0;
    newAsm->aname.build = 0;
    newAsm->aname.revision = 0;
    newImg->nameToClassHashTable = (decltype(newImg->nameToClassHashTable))-0x424e4d;
    newAsm->referencedAssemblyStart = -1;
    newAsm->referencedAssemblyCount = 0;
    Assembly$$GetAllAssemblies()->push_back(newAsm);
    LOGIBNM(OBFUSCATE_BNM("Added new assembly: [%s]"), name.c_str());
    return newImg;
}
bool isBNMType(BNM::IL2CPP::Il2CppType *type) {
    if (BNM::CheckObj(type->data.dummy))
        return ((BNMTypeData *)type->data.dummy)->bnm == -0x424e4d;
    return false;
}
BNM::IL2CPP::Il2CppClass* Class$$FromIl2CppType(BNM::IL2CPP::Il2CppType* type) {
    if (!type) return nullptr;
    if (isBNMType(type))
        return ((BNMTypeData *)type->data.dummy)->cls;
    return old_Class$$FromIl2CppType(type);
}
BNM::IL2CPP::Il2CppClass* Class$$FromName(BNM::IL2CPP::Il2CppImage* image, const char* _namespace, const char *name) {
    if (!image || !name[0]) return nullptr;
    BNM::IL2CPP::Il2CppClass *ret = nullptr;
    if (image->nameToClassHashTable != (decltype(image->nameToClassHashTable))-0x424e4d)
        ret = old_Class$$FromName(image, _namespace, name);
    if (!ret)
        BNMClassesMap.forEachByImage(image, [_namespace, name, &ret](BNM::IL2CPP::Il2CppClass *BNM_class) -> bool {
            if (!strcmp(_namespace, BNM_class->namespaze) && !strcmp(name, BNM_class->name)) {
                ret = BNM_class;
                return true;
            }
            return false;
        });
    return ret;
}
void Image$$GetTypes(BNM::IL2CPP::Il2CppImage* image, bool, BNM::TypeVector* target) {
    if (!image || !target) return;
    if (image->nameToClassHashTable != (decltype(image->nameToClassHashTable))-0x424e4d) {
        if (HasImageGetCls) {
            DO_API(BNM::IL2CPP::Il2CppClass*, il2cpp_image_get_class, (BNM::IL2CPP::Il2CppImage*, decltype(image->typeCount)));
            auto typeCount = image->typeCount;
            for (decltype(image->typeCount) i = 0; i < typeCount; ++i) {
                auto type = il2cpp_image_get_class(image, i);
                if (OBFUSCATES_BNM("<Module>") == type->name) continue;
                target->push_back(type);
            }
        } else old_Image$$GetTypes(image, false, target);
    }
    BNMClassesMap.forEachByImage(image, [&target](BNM::IL2CPP::Il2CppClass *BNM_class) -> bool {
        target->push_back(BNM_class);
        return false;
    });
}
// Need due Image and Assembly in 2017.x- has index instead of pointer to Image and Assembly
#if UNITY_VER <= 174
BNM::IL2CPP::Il2CppImage*(*old_GetImageFromIndex)(BNM::IL2CPP::ImageIndex index);
BNM::IL2CPP::Il2CppImage* new_GetImageFromIndex(BNM::IL2CPP::ImageIndex index) {
    if (index < 0) {
        BNM::IL2CPP::Il2CppImage* ret = nullptr;
        BNMClassesMap.forEach([index, &ret](BNM::IL2CPP::Il2CppImage *img, const std::vector<BNM::IL2CPP::Il2CppClass*>& classes) -> bool {
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
BNM::IL2CPP::Il2CppAssembly* new_Assembly_Load(const char *name) {
    DO_API(BNM::IL2CPP::Il2CppImage*, il2cpp_assembly_get_image, (const BNM::IL2CPP::Il2CppAssembly*));
    for (auto asmb : *Assembly$$GetAllAssemblies()) {
        auto img = il2cpp_assembly_get_image(asmb);
        if (!strcmp(img->name, name) || !strcmp(img->nameNoExt, name))
            return asmb;
    }
    return nullptr;
}
#endif
void BNM::NEW_CLASSES::AddNewClass(NEW_CLASSES::NewClass *klass) {
    if (!Classes4Add)
        Classes4Add = new std::vector<BNM::NEW_CLASSES::NewClass *>();
    Classes4Add->push_back(klass);
}
void InitNewClasses() {
    if (!Classes4Add) return;
    for (auto klass : *Classes4Add) {
        if (BNM::LoadClass::ClassExists(klass->myNamespace, klass->Name, klass->DllName)) {
            LOGWBNM(OBFUSCATE_BNM("[%s] [%s]::[%s] already exist, it can't be added it to il2cpp! Please check code."), klass->DllName, klass->myNamespace, klass->Name);
            continue;
        }
        auto typeByVal = new BNM::IL2CPP::Il2CppType();
        typeByVal->type = (BNM::IL2CPP::Il2CppTypeEnum)klass->classType;
        typeByVal->pinned = 0;
        typeByVal->byref = 0;
        typeByVal->num_mods = 0;
        auto typeThis = new BNM::IL2CPP::Il2CppType();
        typeThis->type = (BNM::IL2CPP::Il2CppTypeEnum)klass->classType;
        typeThis->pinned = 0;
        typeThis->byref = 1;
        typeThis->num_mods = 0;
        BNM::IL2CPP::Il2CppImage *curImg = makeOrGetImage(klass);
        auto bnmTypeData = new BNMTypeData();
        typeByVal->data.dummy = (void *)bnmTypeData; // For il2cpp::vm::Class::FromIl2CppType
        typeThis->data.dummy = (void *)bnmTypeData; // For il2cpp::vm::Class::FromIl2CppType
        BNM::IL2CPP::Il2CppClass *parent = BNM::LoadClass(klass->BaseNamespace, klass->BaseName).GetIl2CppClass();
        if (!parent) parent = BNM::GetType<BNM::IL2CPP::Il2CppObject *>().ToIl2CppClass();
        Class$$Init(parent);
        std::vector<BNM::IL2CPP::VirtualInvokeData> newVTable;
        std::vector<BNM::IL2CPP::Il2CppRuntimeInterfaceOffsetPair> newInterOffsets;
        if (parent->interfaceOffsets)
            for (uint16_t i = 0; i < parent->interface_offsets_count; ++i)
                newInterOffsets.push_back(parent->interfaceOffsets[i]);
        for (uint16_t i = 0; i < parent->vtable_count; ++i) newVTable.push_back(parent->vtable[i]);
        const BNM::IL2CPP::MethodInfo **methods = nullptr;
        if (!klass->Methods4Add.empty()) {
            methods = (const BNM::IL2CPP::MethodInfo **) calloc(klass->Methods4Add.size(), sizeof(BNM::IL2CPP::MethodInfo *));
            for (int i = 0; i < klass->Methods4Add.size(); ++i) {
                auto method = klass->Methods4Add[i];
                method->thisMethod->name = (char*)calloc(strlen(method->Name), sizeof(char));
                strcpy((char*)method->thisMethod->name, method->Name);
                method->thisMethod->flags = 0x0006 | 0x0080;
                if (method->isStatic) method->thisMethod->flags |= 0x0010; // PUBLIC | HIDE_BY_SIG | (isStatic ? STATIC : NONE)
                method->thisMethod->is_generic = false;
                method->thisMethod->return_type = method->ret_type.ToIl2CppType();
#if UNITY_VER < 212
                for (int p = 0; p < method->thisMethod->parameters_count; ++p) {
                    auto newParam = new BNM::IL2CPP::ParameterInfo();
                    auto name = OBFUSCATES_BNM("arg") + std::to_string(p);
                    newParam->name = (char*)calloc(name.size(), sizeof(char));
                    strcpy((char*)newParam->name, name.c_str());
                    newParam->position = p;
                    if (method->args_types && !method->args_types->empty() && p < method->args_types->size())
                        newParam->parameter_type = (*method->args_types)[p].ToIl2CppType();
                }
#else
                method->thisMethod->parameters = nullptr;
#endif
                methods[i] = method->thisMethod;
            }
        }
        klass->thisClass = (BNM::IL2CPP::Il2CppClass*)malloc(sizeof(BNM::IL2CPP::Il2CppClass) + newVTable.size() * sizeof(BNM::IL2CPP::VirtualInvokeData));
#if UNITY_VER > 174
#define kls klass
#define typeSymbol *
#else
#define typeSymbol
#define kls declaring_type
#endif
        if (!klass->Methods4Add.empty()) {
            for (int i = 0; i < klass->Methods4Add.size(); ++i)
                ((BNM::IL2CPP::MethodInfo *)methods[i])->kls = klass->thisClass;
            klass->thisClass->method_count = klass->Methods4Add.size();
            klass->thisClass->methods = methods;
            klass->Methods4Add.clear();
        } else {
            klass->thisClass->method_count = 0;
            klass->thisClass->methods = nullptr;
        }
        klass->thisClass->parent = parent;
        klass->thisClass->typeHierarchyDepth = parent->typeHierarchyDepth + 1;
        klass->thisClass->typeHierarchy = (BNM::IL2CPP::Il2CppClass**)calloc(klass->thisClass->typeHierarchyDepth, sizeof(BNM::IL2CPP::Il2CppClass*));
        memcpy(klass->thisClass->typeHierarchy, parent->typeHierarchy, parent->typeHierarchyDepth * sizeof(BNM::IL2CPP::Il2CppClass*));
        klass->thisClass->typeHierarchy[klass->thisClass->typeHierarchyDepth - 1] = klass->thisClass;
        klass->thisClass->image = curImg;
        klass->thisClass->name = (char*)calloc(strlen(klass->Name), sizeof(char));
        strcpy((char*)klass->thisClass->name, klass->Name);
        klass->thisClass->namespaze = (char*)calloc(strlen(klass->myNamespace), sizeof(char));
        strcpy((char*)klass->thisClass->namespaze, klass->myNamespace);
        klass->thisClass->byval_arg = typeSymbol typeByVal;
        klass->thisClass->this_arg = typeSymbol typeThis;
#undef kls
#undef typeSymbol
        klass->thisClass->declaringType = nullptr;
        klass->thisClass->flags = klass->thisClass->parent->flags &~0x00000080; // TYPE_ATTRIBUTE_ABSTRACT
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
        newVTable.clear();
        klass->thisClass->interface_offsets_count = newInterOffsets.size();
        klass->thisClass->interfaceOffsets = (BNM::IL2CPP::Il2CppRuntimeInterfaceOffsetPair*)(calloc(newInterOffsets.size(), sizeof(BNM::IL2CPP::Il2CppRuntimeInterfaceOffsetPair)));
        for (int i = 0; i < newInterOffsets.size(); ++i)
            klass->thisClass->interfaceOffsets[i] = newInterOffsets[i];
        newInterOffsets.clear();
        if (!klass->Interfaces.empty()) {
            auto interfaces = klass->Interfaces;
            klass->thisClass->interfaces_count = interfaces.size();
            klass->thisClass->implementedInterfaces = (BNM::IL2CPP::Il2CppClass**)calloc(interfaces.size(), sizeof(BNM::IL2CPP::Il2CppClass *));
            for (int i = 0; i < interfaces.size(); ++i) klass->thisClass->implementedInterfaces[i] = interfaces[i];
            interfaces.clear();
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
#if UNITY_VER < 212
        klass->thisClass->has_initialization_error = 0;
#endif
        klass->thisClass->naturalAligment = 0;
#endif
        klass->thisClass->initializationExceptionGCHandle = 0;
        klass->thisClass->init_pending = 0;
#if UNITY_VER < 202
        klass->thisClass->genericContainerIndex = -1;
#else
        klass->thisClass->genericContainerHandle = nullptr;
        klass->thisClass->typeMetadataHandle = nullptr;
#endif
#if UNITY_VER < 211
        klass->thisClass->valuetype = 1;
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
        klass->thisClass->thread_static_fields_size = 0;
        klass->thisClass->cctor_started = 0;
        klass->thisClass->cctor_finished = 0;
        klass->thisClass->cctor_thread = 0;
        klass->thisClass->field_count = klass->Fields4Add.size() + klass->StaticFields4Add.size();
        if (klass->thisClass->field_count > 0) {
            auto fields = (BNM::IL2CPP::FieldInfo *)calloc(klass->thisClass->field_count, sizeof(BNM::IL2CPP::FieldInfo));
            BNM::IL2CPP::FieldInfo* newField = fields;
            if (!klass->Fields4Add.empty()) {
                for (auto field : klass->Fields4Add) {
                    newField->name = (char*)calloc(strlen(field->Name), sizeof(char));
                    strcpy((char*)newField->name, field->Name);
                    newField->type = field->type.ToIl2CppType();
                    newField->parent = klass->thisClass;
                    newField->offset = field->offset;
                    newField->token = (int)newField->type->attrs | (int)field->attributes; // PUBLIC
                    newField++;
                }
                klass->Fields4Add.clear();
            }
            if (!klass->StaticFields4Add.empty()) {
                for (auto field : klass->StaticFields4Add) {
                    newField->name = (char*)calloc(strlen(field->Name), sizeof(char));
                    strcpy((char*)newField->name, field->Name);
                    newField->type = field->type.ToIl2CppType();
                    newField->parent = klass->thisClass;
                    newField->offset = field->offset;
                    newField->token = (int)newField->type->attrs | (int)field->attributes; // PUBLIC | STATIC
                    newField++;
                }
                klass->StaticFields4Add.clear();
                klass->thisClass->static_fields = (void *)klass->staticFieldsAddress;
                klass->thisClass->static_fields_size = klass->staticFieldOffset;
            } else {
                klass->thisClass->static_fields_size = 0;
                klass->thisClass->static_fields = nullptr;
            }
            klass->thisClass->fields = fields;
        } else {
            klass->thisClass->static_fields_size = 0;
            klass->thisClass->static_fields = nullptr;
            klass->thisClass->fields = nullptr;
        }
        bnmTypeData->cls = klass->thisClass;
        BNMClassesMap.addClass(curImg, klass->thisClass);
        LOGIBNM(OBFUSCATE_BNM("[InitNewClasses] Added new class (%p): [%s]::[%s] parent is [%s]::[%s] to [%s]"), klass->thisClass, klass->thisClass->namespaze, klass->thisClass->name, parent->namespaze, parent->name, klass->thisClass->image->name);
    }
    Classes4Add->clear();
}
#endif

namespace BNM::HexUtils {
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
    DWORD HexStr2DWORD(const std::string &hex) { return strtoull(hex.c_str(), nullptr, 16); }
#if defined(__ARM_ARCH_7A__)
    bool Is_B_BL_Hex(const std::string& hex) {
        DWORD hexW = HexStr2DWORD(ReverseHexString(FixHexString(hex)));
        return (hexW & 0x0A000000) == 0x0A000000;
    }
#elif defined(__aarch64__)
    bool Is_B_BL_Hex(const std::string& hex) {
        DWORD hexW = HexStr2DWORD(ReverseHexString(FixHexString(hex)));
        return (hexW & 0xFC000000) == 0x14000000 || (hexW & 0xFC000000) == 0x94000000;
    }
#elif defined(__i386__)
    bool Is_x86_call_hex(const std::string &hex) { return hex.substr(0, 2) == OBFUSCATE_BNM("E8"); }
#else
#warning "Call or B BL hex checker support only arm64, arm and x86"
#endif
    std::string ReadMemory(DWORD address, size_t len) {
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
    bool Decode_Branch_or_Call_Hex(const std::string &hex, DWORD offset, DWORD &outOffset) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
        if (!Is_B_BL_Hex(hex)) return false;
#if defined(__aarch64__)
        int add = 0;
#else
        int add = 8;
#endif
        outOffset = ((int32_t)(((((HexStr2DWORD(ReverseHexString(FixHexString(hex)))) & (((uint32_t)1 << 24) - 1) << 0) >> 0) << 2) << (32 - 26)) >> (32 - 26)) + offset + add;
#elif defined(__i386__)
        if (!Is_x86_call_hex(hex)) return false;
        outOffset = offset + HexStr2DWORD(ReverseHexString(FixHexString(hex)).substr(0, 8)) + 5;
#else
        #warning "Decode_Branch_or_Call_Hex support only arm64, arm and x86"
        return false;
#endif
        return true;
    }
    DWORD FindNext_B_BL_offset(DWORD start, int index) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
        int offset = 0;
        std::string curHex = ReadMemory(start, 4);
        DWORD outOffset = 0;
        bool out;
        while (!(out = Decode_Branch_or_Call_Hex(curHex, start + offset, outOffset)) || index != 1) {
            offset += 4;
            curHex = ReadMemory(start + offset, 4);
            if (out) index--;
        }
        return outOffset;
#elif defined(__i386__)
        int offset = 0;
        std::string curHex = ReadMemory(start, 1);
        DWORD outOffset = 0;
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
void InitIl2cppMethods() {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
    int count = 1;
#elif defined(__i386__) || defined(__x86_64__)
    int count = 2;
#endif
    //! il2cpp::vm::Class::Init GET
    if (!Class$$Init) {
        Class$$Init = (decltype(Class$$Init))  BNM::HexUtils::FindNext_B_BL_offset(BNM::HexUtils::FindNext_B_BL_offset((DWORD) BNM_dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_array_new_specific")), count), count);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::Init in lib: %p"), BNM::offsetInLib((void *)Class$$Init));
    }

    //! il2cpp_image_get_class CHECK (Unity 2018+)
    {
        DO_API(void,il2cpp_image_get_class,());
        HasImageGetCls = il2cpp_image_get_class != nullptr;
    }

    //! il2cpp::vm::Image::GetTypes HOOK AND GET (OR ONLY GET)
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
    if (!old_Image$$GetTypes && !HasImageGetCls) {
#else
    if (!Image$$GetTypes && !HasImageGetCls) {
#endif
        DO_API(const BNM::IL2CPP::Il2CppImage*, il2cpp_get_corlib, ());
        DO_API(BNM::IL2CPP::Il2CppClass*, il2cpp_class_from_name, (const BNM::IL2CPP::Il2CppImage*, const char*, const char *));
        DWORD GetTypesAdr = 0;
        auto assemblyClass = il2cpp_class_from_name(il2cpp_get_corlib(), OBFUSCATE_BNM("System.Reflection"), OBFUSCATE_BNM("Assembly"));
        GetTypesAdr = BNM::LoadClass(assemblyClass).GetMethodByName(OBFUSCATE_BNM("GetTypes"), 1).GetOffset(); // We can use LoadClass here by Il2CppClass, because for methods we need only it.
        const int sCount
#if UNITY_VER >= 211
                = count;
#elif UNITY_VER > 174
                = count + 1;
#else
        = count + 2;

#endif
        auto Image$$GetTypes_t = BNM::HexUtils::FindNext_B_BL_offset(BNM::HexUtils::FindNext_B_BL_offset(BNM::HexUtils::FindNext_B_BL_offset(GetTypesAdr, count), sCount), count);
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
        HOOK(Image$$GetTypes_t, Image$$GetTypes, old_Image$$GetTypes);
#else
        Image$$GetTypes = (decltype(Image$$GetTypes)) Image$$GetTypes_t;
#endif
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Image::GetTypes in lib: %p"), BNM::offsetInLib((void *)Image$$GetTypes_t));
    } else if (HasImageGetCls) {
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] game has il2cpp_image_get_class. BNM will use it"));
    }
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
    //! il2cpp::vm::Class::FromIl2CppType HOOK
    if (!old_Class$$FromIl2CppType) {
        auto from_type_adr = BNM::HexUtils::FindNext_B_BL_offset((DWORD) BNM_dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_class_from_type")), count);
        HOOK(from_type_adr, Class$$FromIl2CppType, old_Class$$FromIl2CppType);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::FromIl2CppType in lib: %p"), BNM::offsetInLib((void *)from_type_adr));
    }
    //! il2cpp::vm::Class::FromName HOOK
    if (!old_Class$$FromName) {
        auto from_name_adr = BNM::HexUtils::FindNext_B_BL_offset(BNM::HexUtils::FindNext_B_BL_offset((DWORD) BNM_dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_class_from_name")), count), count);
        HOOK(from_name_adr, Class$$FromName, old_Class$$FromName);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::FromName in lib: %p"), BNM::offsetInLib((void *)from_name_adr));
    }
#if UNITY_VER <= 174
    //! il2cpp::vm::MetadataCache::GetImageFromIndex HOOK
        if (!old_GetImageFromIndex) {
            auto GetImageFromIndexOffset = HexUtils::FindNext_B_BL_offset(HexUtils::FindNext_B_BL_offset((DWORD)BNM_dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_assembly_get_image")), count), count);
            HOOK(GetImageFromIndexOffset, new_GetImageFromIndex, old_GetImageFromIndex);
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::MetadataCache::GetImageFromIndex in lib: %p"), BNM::offsetInLib((void *)GetImageFromIndexOffset));
        }
        static void *old_AssemblyLoad;
        //! il2cpp::vm::Assembly::Load HOOK
        if (!old_AssemblyLoad) {
            DWORD AssemblyLoadOffset = HexUtils::FindNext_B_BL_offset((DWORD)BNM_dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_domain_assembly_open")), count);
            HOOK(AssemblyLoadOffset, new_Assembly_Load, old_AssemblyLoad);
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::Load in lib: %p"), BNM::offsetInLib((void *)AssemblyLoadOffset));
        }
#endif
#endif
    //! il2cpp::vm::Assembly::GetAllAssemblies GET
    if (!Assembly$$GetAllAssemblies) {
#ifdef BNM_USE_APPDOMAIN
        DO_API(BNM::IL2CPP::Il2CppImage*, il2cpp_get_corlib, ());
        DO_API(BNM::IL2CPP::Il2CppClass*, il2cpp_class_from_name, (BNM::IL2CPP::Il2CppImage*, const char*, const char*));
        auto assemblyClass = il2cpp_class_from_name(il2cpp_get_corlib(), OBFUSCATE_BNM("System"), OBFUSCATE_BNM("AppDomain"));
        auto getAssembly = BNM::LoadClass(assemblyClass).GetMethodByName(OBFUSCATE_BNM("GetAssemblies"), 1);
        if (getAssembly) {
            const int sCount
#if !defined(__aarch64__) && UNITY_VER >= 211
                = count;
#else
                = count + 1;
#endif
            DWORD GetTypesAdr = BNM::HexUtils::FindNext_B_BL_offset(getAssembly.GetOffset(), count);
            Assembly$$GetAllAssemblies = (BNM::AssemblyVector *(*)())(BNM::HexUtils::FindNext_B_BL_offset(GetTypesAdr, sCount));
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::GetAllAssemblies by AppDomain in lib: %p"), BNM::offsetInLib((void *)Assembly$$GetAllAssemblies));
        } else {
#endif
        auto adr = (DWORD) BNM_dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_domain_get_assemblies"));
        Assembly$$GetAllAssemblies = (BNM::AssemblyVector *(*)())(BNM::HexUtils::FindNext_B_BL_offset(adr, count));
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::GetAllAssemblies by domain in lib: %p"), BNM::offsetInLib((void *)Assembly$$GetAllAssemblies));
#ifdef BNM_USE_APPDOMAIN
        }
#endif
    }
}
void (*old_BNM_il2cpp_init)(const char*);
void BNM_il2cpp_init(const char* domain_name) {
    old_BNM_il2cpp_init(domain_name);
    InitIl2cppMethods();
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
    InitNewClasses();
#endif
    BNM_LibLoaded = true;
}
#ifndef BNM_DISABLE_AUTO_LOAD
[[maybe_unused]] __attribute__((constructor))
void PrepareBNM() {
    std::thread([]() {
        do {
            if (BNM_hardBypass) { BNM_hardBypassed = true; break; }
            BNM_dlLib = BNM_dlopen(OBFUSCATE_BNM("libil2cpp.so"), RTLD_LAZY);
            if (BNM_dlLib) {
                void *init = BNM_dlsym(BNM_dlLib, OBFUSCATE_BNM("il2cpp_init"));
                if (init) {
                    Dl_info info;
                    BNM_dladdr(init, &info);
                    auto l = strlen(info.dli_fname);
                    char *s = (char *)malloc(l);
                    strcpy(s, info.dli_fname);
                    s[l] = 0;
                    BNM_LibAbsolutePath = s;
                    BNM_LibAbsoluteAddress = (DWORD)info.dli_fbase;
                    HOOK(init, BNM_il2cpp_init, old_BNM_il2cpp_init);
                    break;
                }
                BNM_dlclose(BNM_dlLib);
            }
        } while (true);
    }).detach();
}
#endif
#ifndef _WIN32

[[maybe_unused]] void BNM::HardBypass(JNIEnv *env) {
    if (!env || BNM_hardBypass) return;
    BNM_hardBypass = true;
#ifndef BNM_DISABLE_AUTO_LOAD
    while (!BNM_hardBypassed && !BNM_dlLib) usleep(1); // Just in case
#endif
    jclass activityThread = env->FindClass(OBFUSCATE_BNM("android/app/ActivityThread"));
    auto context = env->CallObjectMethod(env->CallStaticObjectMethod(activityThread, env->GetStaticMethodID(activityThread, OBFUSCATE_BNM("currentActivityThread"), OBFUSCATE_BNM("()Landroid/app/ActivityThread;"))), env->GetMethodID(activityThread, OBFUSCATE_BNM("getApplication"), OBFUSCATE_BNM("()Landroid/app/Application;")));
    auto appInfo = env->CallObjectMethod(context, env->GetMethodID(env->GetObjectClass(context), OBFUSCATE_BNM("getApplicationInfo"), OBFUSCATE_BNM("()Landroid/content/pm/ApplicationInfo;")));
    std::string path = env->GetStringUTFChars((jstring)env->GetObjectField(appInfo, env->GetFieldID(env->GetObjectClass(appInfo), OBFUSCATE_BNM("nativeLibraryDir"), OBFUSCATE_BNM("Ljava/lang/String;"))), nullptr);
    BNM_LibAbsolutePath = BNM::str2char(path + OBFUSCATE_BNM("/libil2cpp.so"));
    BNM_dlLib = BNM_dlopen(BNM_LibAbsolutePath, RTLD_LAZY);
    if (BNM_dlLib) {
        auto init = BNM_dlsym(BNM_dlLib, OBFUSCATE_BNM("il2cpp_init"));
        HOOK(init, BNM_il2cpp_init, old_BNM_il2cpp_init);
        Dl_info info{}; BNM_dladdr(init, &info);
        BNM_LibAbsoluteAddress = (DWORD)info.dli_fbase;
    } else LOGEBNM(OBFUSCATE_BNM("Can't hard bypass!"));
}
#endif
typedef std::basic_string<BNM::IL2CPP::Il2CppChar> string16;
std::string Utf16ToUtf8(BNM::IL2CPP::Il2CppChar* utf16String, size_t length) {
    std::string utf8String;
    utf8String.reserve(length);
    utf8::unchecked::utf16to8(utf16String, utf16String + length, std::back_inserter(utf8String));
    return utf8String;
}
string16 Utf8ToUtf16(const char* utf8String, size_t length) {
    string16 utf16String;
    if (utf8::is_valid(utf8String, utf8String + length)) {
        utf16String.reserve(length);
        utf8::unchecked::utf8to16(utf8String, utf8String + length, std::back_inserter(utf16String));
    }
    return utf16String;
}
std::string BNM::MONO_STRUCTS::monoString::get_string() {
    if (!this) return OBFUSCATE_BNM("ERROR: monoString is null");
    if (!isAllocated(chars)) return OBFUSCATE_BNM("ERROR: chars is null");
    return Utf16ToUtf8(chars, length);
}
const char *BNM::MONO_STRUCTS::monoString::get_const_char() { return str2char(get_string()); }
const char *BNM::MONO_STRUCTS::monoString::c_str() { return get_const_char(); }
std::string BNM::MONO_STRUCTS::monoString::str() { return get_string(); }
[[maybe_unused]] std::string BNM::MONO_STRUCTS::monoString::strO() { return get_string_old(); }
BNM::MONO_STRUCTS::monoString::operator std::string() { return get_string(); }
BNM::MONO_STRUCTS::monoString::operator const char *() { return get_const_char(); }
std::string BNM::MONO_STRUCTS::monoString::get_string_old() {
    if (!this) return OBFUSCATE_BNM("ERROR: monoString is null");
    if (!isAllocated(chars)) return OBFUSCATE_BNM("ERROR: chars is null");
    return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(std::wstring(chars, chars + length));
}
[[maybe_unused]] unsigned int BNM::MONO_STRUCTS::monoString::getHash() {
    if (!this || !isAllocated(chars)) return 0;
    IL2CPP::Il2CppChar* p = chars;
    unsigned int h = 0;
    for (int i = 0; i < length; ++i)
        h = (h << 5) - h + *p; p++;
    return h;
}
BNM::MONO_STRUCTS::monoString *BNM::MONO_STRUCTS::monoString::Create(const char *str) {
    const size_t length = strlen(str);
    const size_t utf16Size = sizeof(IL2CPP::Il2CppChar) * length;
    auto ret = (monoString*)malloc(sizeof(monoString) + utf16Size);
    ret->length = (int)length;
    string16 u16 = Utf8ToUtf16(str, ret->length);
    memcpy(ret->chars, &u16[0], utf16Size);
    u16.clear();
    auto empty = Empty();
    if (empty) ret->klass = empty->klass;
    return (monoString*)ret;
}
[[maybe_unused]] BNM::MONO_STRUCTS::monoString *BNM::MONO_STRUCTS::monoString::Create(const std::string& str) { return Create(str2char(str)); }
[[maybe_unused]] BNM::MONO_STRUCTS::monoString *BNM::MONO_STRUCTS::monoString::Empty() { return LoadClass(OBFUSCATES_BNM("System"), OBFUSCATES_BNM("String")).GetFieldByName(OBFUSCATES_BNM("Empty")).setType<monoString*>()(); }
[[maybe_unused]] bool BNM::AttachIl2Cpp() {
    if (CurrentIl2CppThread()) return false;
    DO_API(BNM::IL2CPP::Il2CppDomain*, il2cpp_domain_get, ());
    DO_API(BNM::IL2CPP::Il2CppThread*, il2cpp_thread_attach, (BNM::IL2CPP::Il2CppDomain*));
    il2cpp_thread_attach(il2cpp_domain_get());
    return true;
}
[[maybe_unused]] void BNM::DetachIl2Cpp() {
    auto thread = CurrentIl2CppThread();
    if (!thread) return;
    DO_API(void, il2cpp_thread_detach, (BNM::IL2CPP::Il2CppThread*));
    il2cpp_thread_detach(thread);
}
[[maybe_unused]] BNM::IL2CPP::Il2CppThread* BNM::CurrentIl2CppThread() {
    DO_API(BNM::IL2CPP::Il2CppThread*, il2cpp_thread_current, ());
    return il2cpp_thread_current();
}
BNM::MONO_STRUCTS::monoString *BNM::CreateMonoString(const char *str) {
    DO_API(BNM::MONO_STRUCTS::monoString*, il2cpp_string_new, (const char *str));
    return il2cpp_string_new(str);
}
[[maybe_unused]] BNM::MONO_STRUCTS::monoString *BNM::CreateMonoString(const std::string& str) { return CreateMonoString(str2char(str)); }
void *BNM::getExternMethod(const std::string& str) {
    DO_API(void*, il2cpp_resolve_icall, (const char *));
    auto c_str =  BNM::str2char(str);
    auto ret = il2cpp_resolve_icall(c_str);
    if (!ret) LOGWBNM(OBFUSCATE_BNM("Can't get extern %s. Please check code."), c_str);
    return ret;
}
char *BNM::str2char(const std::string& str) {
    size_t size = str.end() - str.begin();
    if (str.c_str()[size]){
        auto c = (char *)malloc(size);
        std::copy(str.begin(), str.end(), c);
        c[size] = 0;
        return c;
    }
    return (char *)str.c_str();
}
[[maybe_unused]] void *BNM::UNITY_STRUCTS::RaycastHit::get_Collider() const {
    if (!m_Collider || m_Collider < 0) return {};
#if UNITY_VER > 174
    static void *(*FromId)(int);
    if (!FromId)
        InitFunc(FromId, getExternMethod(OBFUSCATE_BNM("UnityEngine.Object::FindObjectFromInstanceID(System.Int32)")));
    return FromId(m_Collider);
#else
    return m_Collider;
#endif
}
[[maybe_unused]] std::string BNM::GetLibIl2CppPath() {
    if (!get_il2cpp()) return OBFUSCATE_BNM("libil2cpp not found!");
    return BNM_LibAbsolutePath;
}
[[maybe_unused]] DWORD BNM::GetLibIl2CppOffset() {
    return BNM_LibAbsoluteAddress;
}
void *BNM::offsetInLib(void *offsetInMemory) {
    Dl_info info; BNM_dladdr(offsetInMemory, &info);
    return (void *) ((DWORD) offsetInMemory - (DWORD) info.dli_fbase);
}
[[maybe_unused]] void *BNM::GetLibIl2CppDlInst() {
    if (!get_il2cpp()) return nullptr;
    return get_il2cpp();
}
