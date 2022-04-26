#include "BNM.hpp"
#include "BNM_data/utf8.h"

/********** BNM DATA **************/

static bool BNM_LibLoaded = false;
static void *BNM_dlLib;
static BNM::IL2CPP::Il2CppThread *BNM_CurThread;
BNM::AssemblyVector *(*Assembly$$GetAllAssemblies)();
#if __cplusplus >= 201703 // Speed up IDE if c++ lower then c++17
void Image$$GetTypes(BNM::IL2CPP::Il2CppImage* image, bool exportedOnly, BNM::TypeVector* target);
void (*old_Image$$GetTypes)(BNM::IL2CPP::Il2CppImage* image, bool exportedOnly, BNM::TypeVector* target);
BNM::IL2CPP::Il2CppClass* (*old_Class$$FromIl2CppType)(BNM::IL2CPP::Il2CppType* type);
BNM::IL2CPP::Il2CppClass* Class$$FromIl2CppType(BNM::IL2CPP::Il2CppType* type);
BNM::IL2CPP::Il2CppClass* (*old_Class$$FromName)(BNM::IL2CPP::Il2CppImage* image, const char* _namespace, const char *name);
BNM::IL2CPP::Il2CppClass* Class$$FromName(BNM::IL2CPP::Il2CppImage* image, const char* _namespace, const char *name);

bool (*old_Class$$Init)(BNM::IL2CPP::Il2CppClass *klass);
bool Class$$Init(BNM::IL2CPP::Il2CppClass *klass);
static std::vector<BNM::NEW_CLASSES::NewClass *> *Classes4Add;
#else
void (*Image$$GetTypes)(IL2CPP::Il2CppImage* image, bool exportedOnly, TypeVector* target);
void (*Class$$Init)(IL2CPP::Il2CppClass *klass);

#endif

/********** BNM MACRO CODE **************/

void *get_il2cpp() { return BNM_dlLib; }
bool BNM::Il2cppLoaded() { return BNM_LibLoaded; }
DWORD BNM::GetOffset(IL2CPP::FieldInfo *field) { return field->offset; }
DWORD BNM::GetOffset(IL2CPP::MethodInfo *methodInfo) { return (DWORD) methodInfo->methodPointer; }

/********** BNM MAIN CODE **************/

#define DO_API(r, n, p) auto (n) = (r (*) p)dlsym(get_il2cpp(), OBFUSCATE_BNM(#n))

BNM::IL2CPP::Il2CppClass *GetClassFromName(const std::string& _namespace, const std::string& _name) {
    DO_API(BNM::IL2CPP::Il2CppImage*, il2cpp_assembly_get_image, (BNM::IL2CPP::Il2CppAssembly*));
    for (auto assembly : *Assembly$$GetAllAssemblies()) {
        BNM::TypeVector classes;
        Image$$GetTypes(il2cpp_assembly_get_image(assembly), false, &classes);
        for (auto cls : classes) {
            if (!cls) continue;
            Class$$Init(cls);
            if (cls->name == _name && cls->namespaze == _namespace)
                return (BNM::IL2CPP::Il2CppClass *)cls;
        }
    }
    return nullptr;
}

BNM::LoadClass::LoadClass() = default;
BNM::LoadClass::LoadClass(IL2CPP::Il2CppClass *clazz) { klass = clazz; }
BNM::LoadClass::LoadClass(IL2CPP::Il2CppObject* obj) { klass = obj->klass; }
[[maybe_unused]] BNM::LoadClass::LoadClass(IL2CPP::Il2CppType* type) {
    DO_API(IL2CPP::Il2CppClass*, il2cpp_class_from_il2cpp_type, (const IL2CPP::Il2CppType*));
    klass = il2cpp_class_from_il2cpp_type(type);
}

[[maybe_unused]] BNM::LoadClass::LoadClass(MonoType *type) {
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
BNM::IL2CPP::FieldInfo *BNM::LoadClass::GetFieldInfoByName(const std::string& name) const {
    if (!klass) return nullptr;
    TryInit();
    DO_API(IL2CPP::FieldInfo *, il2cpp_class_get_field_from_name, (IL2CPP::Il2CppClass*, const char*));
    return il2cpp_class_get_field_from_name(klass, name.c_str());
}
DWORD BNM::LoadClass::GetFieldOffset(const std::string& name) const {
    if (!klass) return 0;
    return GetOffset(GetFieldInfoByName(name));
}
BNM::IL2CPP::MethodInfo *BNM::LoadClass::GetMethodInfoByName(const std::string& name, int paramCount) const {
    if (!klass) return nullptr;
    TryInit();
    DO_API(IL2CPP::MethodInfo*, il2cpp_class_get_method_from_name, (IL2CPP::Il2CppClass*, const char*, int));
    return il2cpp_class_get_method_from_name(klass, name.c_str(), paramCount);
}
[[maybe_unused]] DWORD BNM::LoadClass::GetMethodOffsetByName(const std::string& name, int paramCount) const {
    if (!klass) return 0;
    IL2CPP::MethodInfo *res = GetMethodInfoByName(name, paramCount);
    if (!res) {
        LOGIBNM(OBFUSCATE_BNM("Method: [%s].[%s]::[%s], %d - not found"), klass->namespaze, klass->name, name.c_str(), paramCount);
        return 0;
    }
    return GetOffset(res);
}
BNM::IL2CPP::MethodInfo *BNM::LoadClass::GetMethodInfoByName(const std::string& name, const std::vector<std::string>& params_names) const {
    if (!klass) return nullptr;
    TryInit();
    size_t paramCount = params_names.size();
    for (int i = 0; i < klass->method_count; i++) {
        auto method = (IL2CPP::MethodInfo *) klass->methods[i];
        if (name == method->name && method->parameters_count == paramCount) {
            bool ok = true;
            for (int i = 0; i < paramCount; i++) {
                auto param = method->parameters + i;
                if (param->name != params_names[i]) {
                    ok = false;
                    break;
                }
            }
            if (ok)
                return method;
        }
    }
    return nullptr;
}
[[maybe_unused]] DWORD BNM::LoadClass::GetMethodOffsetByName(const std::string& name, const std::vector<std::string>& params_names) const {
    if (!klass) return 0;
    size_t paramCount = params_names.size();
    IL2CPP::MethodInfo *res = GetMethodInfoByName(name, params_names);
    if (!res) {
        LOGIBNM(OBFUSCATE_BNM("Method: [%s].[%s]::[%s], %d - not found"), klass->namespaze, klass->name, name.c_str(), paramCount);
        return 0;
    }
    return GetOffset(res);
}
BNM::IL2CPP::MethodInfo *BNM::LoadClass::GetMethodInfoByName(const std::string &name, const std::vector<std::string> &params_names, const std::vector<IL2CPP::Il2CppType *> &params_types) const {
    if (!klass) return nullptr;
    TryInit();
    DO_API(IL2CPP::Il2CppClass*, il2cpp_class_from_il2cpp_type, (const IL2CPP::Il2CppType*));
    size_t paramCount = params_names.size();
    if (paramCount != params_types.size()) return nullptr;
    for (int i = 0; i < klass->method_count; i++) {
        auto method = (IL2CPP::MethodInfo *)klass->methods[i];
        if (name == method->name && method->parameters_count == paramCount) {
            bool ok = true;
            for (int i = 0; i < paramCount; i++) {
                auto param = method->parameters + i;
                auto cls = il2cpp_class_from_il2cpp_type(param->parameter_type);
                auto param_cls = il2cpp_class_from_il2cpp_type(params_types[i]);
                if (param->name != params_names[i] || !(!strcmp(cls->name, param_cls->name) && !strcmp(cls->namespaze, param_cls->namespaze))) {
                    ok = false;
                    break;
                }
            }
            if (ok)
                return method;
        }
    }
    return nullptr;
}

[[maybe_unused]] DWORD BNM::LoadClass::GetMethodOffsetByName(const std::string &name, const std::vector<std::string> &params_names, const std::vector<IL2CPP::Il2CppType *> &params_types) const {
    if (!klass) return 0;
    size_t paramCount = params_names.size();
    if (paramCount != params_types.size()) return 0;
    IL2CPP::MethodInfo *res = GetMethodInfoByName(name, params_names, params_types);
    if (!res) {
        LOGIBNM(OBFUSCATE_BNM("Method: [%s].[%s]::[%s], %d - not found"), klass->namespaze, klass->name, name.c_str(), paramCount);
        return 0;
    }
    return GetOffset(res);
}
BNM::IL2CPP::MethodInfo *BNM::LoadClass::GetMethodInfoByName(const std::string &name, const std::vector<IL2CPP::Il2CppType *> &params_types) const {
    if (!klass) return nullptr;
    TryInit();
    DO_API(IL2CPP::Il2CppClass*, il2cpp_class_from_il2cpp_type, (const IL2CPP::Il2CppType*));
    size_t paramCount = params_types.size();
    for (int i = 0; i < klass->method_count; i++) {
        auto method = (IL2CPP::MethodInfo *)klass->methods[i];
        if (name == method->name && method->parameters_count == paramCount) {
            bool ok = true;
            for (int i = 0; i < paramCount; i++) {
                auto param = method->parameters + i;
                auto cls = il2cpp_class_from_il2cpp_type(param->parameter_type);
                auto param_cls = il2cpp_class_from_il2cpp_type(params_types[i]);
                if (!(cls->name == param_cls->name && cls->namespaze == param_cls->namespaze)) {
                    ok = false;
                    break;
                }
            }
            if (ok)
                return method;
        }
    }
    return nullptr;
}
[[maybe_unused]] DWORD BNM::LoadClass::GetMethodOffsetByName(const std::string &name, const std::vector<IL2CPP::Il2CppType *> &params_types) const {
    if (!klass) return 0;
    size_t paramCount = params_types.size();
    IL2CPP::MethodInfo *res = GetMethodInfoByName(name, params_types);
    if (!res) {
        LOGIBNM(OBFUSCATE_BNM("Method: [%s].[%s]::[%s], %d - not found"), klass->namespaze, klass->name, name.c_str(), paramCount);
        return 0;
    }
    return GetOffset(res);
}
BNM::IL2CPP::Il2CppType *BNM::LoadClass::GetIl2CppType() const {
    if (!klass) return nullptr;
    TryInit();
#if UNITY_VER > 174
    return (IL2CPP::Il2CppType *)&klass->byval_arg;
#else
    return (IL2CPP::Il2CppType *)klass->byval_arg;
#endif
}
[[maybe_unused]] BNM::MonoType *BNM::LoadClass::GetMonoType() const {
    if (!klass) return nullptr;
    TryInit();
    DO_API(IL2CPP::Il2CppObject*, il2cpp_type_get_object, (IL2CPP::Il2CppType*));
    return (MonoType *) il2cpp_type_get_object(GetIl2CppType());
}
void *BNM::LoadClass::CreateNewInstance() const {
    if (!klass) return nullptr;
    TryInit();
    DO_API(IL2CPP::Il2CppObject*, il2cpp_object_new, (IL2CPP::Il2CppClass*));
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
    return OBFUSCATES_BNM("Class not loaded!");
}
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
                for (int i = 0; i < cls->method_count; i++) {
                    auto method = (IL2CPP::MethodInfo *)cls->methods[i];
                    if (method && methodName == method->name)
                        return LoadClass(cls);
                }
            }
        }
    }
    return {};
}

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
    DO_API(IL2CPP::Il2CppObject*, il2cpp_value_box, (IL2CPP::Il2CppClass* , void*));
    return il2cpp_value_box(klass, data);
}

BNM::LoadClass BNM::TypeFinder::ToLC() const {
    if (byNameOnly) {
        return LoadClass(OBFUSCATE_BNM("System"), name);
    } else if (withMethodName) {
        return LoadClass::WithMethodName(_namespace, name, methodName);
    } else {
        return LoadClass(_namespace, name);
    }
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

#if __cplusplus >= 201703
BNM::NEW_CLASSES::NewMethod::NewMethod() = default;
BNM::NEW_CLASSES::NewField::NewField() = default;
BNM::NEW_CLASSES::NewClass::NewClass() = default;
void BNM::NEW_CLASSES::NewClass::AddNewField(NewField *field, bool isStatic) {
    if (!isStatic)
        Fields4Add.push_back(field);
    else {
        if (staticFieldOffset == 0x0)
            staticFieldsAddress = field->cppOffset;
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
BNM::IL2CPP::Il2CppImage *makeOrGetImage(const std::string& name) {
    DO_API(BNM::IL2CPP::Il2CppImage*, il2cpp_assembly_get_image, (const BNM::IL2CPP::Il2CppAssembly*));
    BNM::AssemblyVector *LoadedAssemblies = Assembly$$GetAllAssemblies();
    for (auto assembly : *LoadedAssemblies) {
        BNM::IL2CPP::Il2CppImage *img = il2cpp_assembly_get_image(assembly);
        if (img->nameNoExt == name)
            return img;
    }
    auto newImg = new BNM::IL2CPP::Il2CppImage();
    newImg->nameNoExt = (char*)calloc(name.size() + 1, sizeof(char));
    strcpy((char*)newImg->nameNoExt, name.c_str());
    std::string nameExt = (name + OBFUSCATES_BNM(".dll"));
    newImg->name = (char*)calloc(nameExt.size() + 1, sizeof(char));
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
    auto newAsm = new BNM::IL2CPP::Il2CppAssembly();
#if UNITY_VER > 174
    newAsm->image = newImg;
    newAsm->image->assembly = newAsm;
    newAsm->aname.name = (char*)calloc(name.size() + 1, sizeof(char));
    strcpy((char*)newAsm->aname.name, name.c_str());
#else
    static int newAsmCount = 1;
    newImg->assemblyIndex = newAsm->imageIndex = -newAsmCount;
    newAsmCount++;
#endif
    newAsm->aname.major = 1;
    newAsm->aname.minor = 0;
    newAsm->aname.build = 0;
    newAsm->aname.revision = 0;
    newImg->nameToClassHashTable = (decltype(newImg->nameToClassHashTable))-0x424e4d;
    newAsm->referencedAssemblyStart = -1;
    newAsm->referencedAssemblyCount = 0;
    Assembly$$GetAllAssemblies()->push_back(newAsm);
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
    if (((DWORD)image->nameToClassHashTable) != -0x424e4d)
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
bool Class$$Init(BNM::IL2CPP::Il2CppClass *klass) {
    if (!klass) return false;
    if (((DWORD)klass->image->nameToClassHashTable) != -0x424e4d)
        return old_Class$$Init(klass);
    return true;
}

void Image$$GetTypes(BNM::IL2CPP::Il2CppImage* image, bool, BNM::TypeVector* target) {
    if (!image || !target) return;
    if (((DWORD)image->nameToClassHashTable) != -0x424e4d)
        old_Image$$GetTypes(image, false, target);
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
void InitNewClasses(std::vector<BNM::NEW_CLASSES::NewClass *> *classes) {
    if (!classes) return;
    for (auto klass : *classes) {
        auto type = new BNM::IL2CPP::Il2CppType();
        type->type = (BNM::IL2CPP::Il2CppTypeEnum)klass->classType;
        type->pinned = 0;
        type->byref = 0;
        type->num_mods = 0;
        BNM::IL2CPP::Il2CppImage *curImg = makeOrGetImage(klass->DllName);
        curImg->typeCount++;
        auto bnmTypeData = new BNMTypeData();
        type->data.dummy = (void *)bnmTypeData; // For il2cpp::vm::Class::FromIl2CppType
        BNM::IL2CPP::Il2CppClass *parent = BNM::LoadClass(klass->BaseNamespace, klass->BaseName).GetIl2CppClass();
        if (!parent)
            parent = BNM::GetType<BNM::IL2CPP::Il2CppObject *>().ToIl2CppClass();
        Class$$Init(parent);
        std::vector<BNM::IL2CPP::VirtualInvokeData> newVTable;
        std::vector<BNM::IL2CPP::Il2CppRuntimeInterfaceOffsetPair> newInterOffsets;
        if (parent->interfaceOffsets)
            for (uint16_t nI = 0; nI < parent->interface_offsets_count; ++nI)
                newInterOffsets.push_back(parent->interfaceOffsets[nI]);
        for (uint16_t v = 0; v < parent->vtable_count; ++v)
            newVTable.push_back(parent->vtable[v]);
        const BNM::IL2CPP::MethodInfo **methods = nullptr;
        if (!klass->Methods4Add.empty()) {
            methods = (const BNM::IL2CPP::MethodInfo **) calloc(klass->Methods4Add.size(), sizeof(BNM::IL2CPP::MethodInfo *));
            for (int m = 0; m < klass->Methods4Add.size(); m++) {
                auto method = klass->Methods4Add[m];
                method->thisMethod->name = method->Name;
                method->thisMethod->flags = 0x0006 | 0x0080 | (method->isStatic ? 0x0010 : 0); // PUBLIC | HIDE_BY_SIG | (isStatic ? STATIC : NONE)
                method->thisMethod->is_generic = false;
                method->thisMethod->return_type = method->ret_type.ToIl2CppType();
                for (int ip = 0; ip < method->thisMethod->parameters_count; ip++) {
                    auto newParam = new BNM::IL2CPP::ParameterInfo();
                    newParam->name = (OBFUSCATES_BNM("arg") + to_string(ip)).c_str();
                    newParam->position = ip;
                    if (method->args_types && !method->args_types->empty() && ip < method->args_types->size())
                        newParam->parameter_type = (*method->args_types)[ip].ToIl2CppType();
                }
                methods[m] = method->thisMethod;
            }
        }
        klass->thisClass = (BNM::IL2CPP::Il2CppClass*)malloc(sizeof(BNM::IL2CPP::Il2CppClass) + newVTable.size() * sizeof(BNM::IL2CPP::VirtualInvokeData));
        if (!klass->Methods4Add.empty()) {
            for (int im = 0; im < klass->Methods4Add.size(); im++)
                ((BNM::IL2CPP::MethodInfo *)methods[im])
#if UNITY_VER > 174
                        ->klass = klass->thisClass;
#else
            ->declaring_type = klass->thisClass;
#endif
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
        klass->thisClass->name = klass->Name;
        klass->thisClass->namespaze = klass->myNamespace;
        klass->thisClass->byval_arg = klass->thisClass->this_arg =
#if UNITY_VER > 174
                *type;
#else
        type;
#endif
        klass->thisClass->flags = klass->thisClass->parent->flags & ~0x00000080; // TYPE_ATTRIBUTE_ABSTRACT
        klass->thisClass->element_class = klass->thisClass;
        klass->thisClass->castClass = klass->thisClass;
#if UNITY_VER > 174
        klass->thisClass->klass = klass->thisClass;
#endif
        klass->thisClass->native_size -1;
        klass->thisClass->actualSize = klass->size;
        klass->thisClass->instance_size = klass->size;
        klass->thisClass->vtable_count = newVTable.size();
        for (int vi = 0; vi < newVTable.size(); vi++)
            klass->thisClass->vtable[vi] = newVTable[vi];
        newVTable.clear();
        klass->thisClass->interface_offsets_count = newInterOffsets.size();
        klass->thisClass->interfaceOffsets = (BNM::IL2CPP::Il2CppRuntimeInterfaceOffsetPair*)(calloc(newInterOffsets.size(), sizeof(BNM::IL2CPP::Il2CppRuntimeInterfaceOffsetPair)));
        for (int ii = 0; ii < newInterOffsets.size(); ii++)
            klass->thisClass->interfaceOffsets[ii] = newInterOffsets[ii];
        newInterOffsets.clear();
        if (!klass->Interfaces.empty()) {
            auto interfaces = klass->Interfaces;
            klass->thisClass->interfaces_count = interfaces.size();
            klass->thisClass->implementedInterfaces = (BNM::IL2CPP::Il2CppClass**)calloc(interfaces.size(), sizeof(BNM::IL2CPP::Il2CppClass*));
            for (int ii = 0; ii < interfaces.size(); ii++)
                klass->thisClass->implementedInterfaces[ii] = interfaces[ii];
            interfaces.clear();
        }
        klass->thisClass->generic_class = nullptr;
        klass->thisClass->genericRecursionDepth = 1;
        klass->thisClass->initialized = 1;
#if UNITY_VER > 182
        klass->thisClass->initialized_and_no_error = 1;
        klass->thisClass->has_initialization_error = 0;
        klass->thisClass->naturalAligment = 8;
#endif
        klass->thisClass->init_pending = 0;
#if UNITY_VER < 202
        klass->thisClass->genericContainerIndex = -1;
#else
        klass->thisClass->genericContainerHandle = nullptr;
        klass->thisClass->typeMetadataHandle = nullptr;
#endif
#if UNITY_VER == 202
        klass->thisClass->valuetype = 1;
#endif
        klass->thisClass->token = -1;
        klass->thisClass->has_references = 0;
        klass->thisClass->has_finalize = 0;
        klass->thisClass->size_inited = 1;
        klass->thisClass->has_cctor = 0;
        klass->thisClass->enumtype = 0;
        klass->thisClass->minimumAlignment = 8;
        klass->thisClass->is_generic = 0;
        klass->thisClass->field_count = klass->Fields4Add.size() + klass->StaticFields4Add.size();
        if (klass->thisClass->field_count > 0) {
            auto fields = (BNM::IL2CPP::FieldInfo *)calloc(klass->thisClass->field_count, sizeof(BNM::IL2CPP::FieldInfo));
            BNM::IL2CPP::FieldInfo* newField = fields;
            if (!klass->Fields4Add.empty()) {
                for (int f = 0; f < klass->Fields4Add.size(); f++) {
                    auto field = klass->Fields4Add[f];
                    newField->name = field->Name;
                    newField->type = field->type.ToIl2CppType();
                    newField->parent = klass->thisClass;
                    newField->offset = field->offset;
                    newField->token = (int)newField->type->attrs | (int)field->attributes; // PUBLIC
                    newField++;
                }
                klass->Fields4Add.clear();
            }
            if (!klass->StaticFields4Add.empty()) {
                for (int sf = 0; sf < klass->StaticFields4Add.size(); sf++) {
                    auto field = klass->StaticFields4Add[sf];
                    newField->name = field->Name;
                    newField->type = field->type.ToIl2CppType();
                    newField->parent = klass->thisClass;
                    newField->offset = field->offset;
                    newField->token = (int)newField->type->attrs | (int)field->attributes; // PUBLIC | STATIC
                    newField++;
                }
                klass->StaticFields4Add.clear();
                klass->thisClass->static_fields = (void *)klass->staticFieldsAddress;
                klass->thisClass->static_fields_size = klass->staticFieldOffset;
            }
            klass->thisClass->fields = fields;
        }
        bnmTypeData->cls = klass->thisClass;
        BNMClassesMap.addClass(curImg, klass->thisClass);
        LOGIBNM(OBFUSCATE_BNM("[InitNewClasses] Added new class: [%s]::[%s] parent is [%s]::[%s] to [%s]"), klass->thisClass->namespaze, klass->thisClass->name, parent->namespaze, parent->name, klass->thisClass->image->name);
    }
    Classes4Add->clear();
}
#endif

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
            if (str[i] == ' ')
                str.erase(i, 1);
        return str;
    }
    DWORD HexStr2DWORD(const std::string &hex) { return strtoull(hex.c_str(), nullptr, 16); }
    bool Is_B_BL_Hex_arm64(const std::string &hex) {
        DWORD hexW = HexStr2DWORD(ReverseHexString(FixHexString(hex)));
        return (hexW & 0xFC000000) == 0x14000000 || (hexW & 0xFC000000) == 0x94000000;
    }
    bool Is_B_BL_Hex(const std::string &hex) {
        DWORD hexW = HexStr2DWORD(ReverseHexString(FixHexString(hex)));
        return ((hexW & 0xFC000000) == 0x14000000 || (hexW & 0xFC000000) == 0x94000000) || (hexW & 0x0A000000) == 0x0A000000;
    }
    std::string ReadMemory(DWORD address, size_t len) {
        char temp[len];
        memset(temp, 0, len);
        const size_t bufferLen = len * 2 + 1;
        char buffer[bufferLen];
        memset(buffer, 0, bufferLen);
        std::string ret;
        if (memcpy(temp, (void *)address, len) == nullptr)
            return ret;
        for (int i = 0; i < len; i++)
            sprintf(&buffer[i * 2], OBFUSCATE_BNM("%02X"), (unsigned char) temp[i]);
        ret += buffer;
        return ret;
    }
    bool Is_x86_call_hex(const std::string &hex) { return hex.substr(0, 2) == OBFUSCATE_BNM("E8"); }
/*
 * Branch decoding based on
 * https://github.com/aquynh/capstone/
*/
    bool Decode_Branch_or_Call_Hex(const std::string &hex, DWORD offset, DWORD *outOffset) {
        bool arm;
        if (!(arm = Is_B_BL_Hex(hex)) && !Is_x86_call_hex(hex)) return false;
        if (arm)
            *outOffset = ((int32_t)(((((HexStr2DWORD(ReverseHexString(FixHexString(hex)))) & (((uint32_t)1 << 24) - 1) << 0) >> 0) << 2) << (32 - 26)) >> (32 - 26)) + offset + (Is_B_BL_Hex_arm64(hex) ? 0 : 8);
        else
            *outOffset = offset + HexStr2DWORD(ReverseHexString(FixHexString(hex)).substr(0, 8)) + 5;
        return true;
    }
    DWORD FindNext_B_BL_offset(DWORD start, int index = 1) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
        int offset = 0;
        std::string curHex = ReadMemory(start, 4);
        DWORD outOffset = 0;
        bool out;
        while (!(out = Decode_Branch_or_Call_Hex(curHex, start + offset, &outOffset)) || index != 1) {
            offset += 4;
            curHex = ReadMemory(start + offset, 4);
            if (out)
                index--;
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
            if (out)
                index--;
        }
        Decode_Branch_or_Call_Hex(ReadMemory(start + offset, 5), start + offset, &outOffset);
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
    //! il2cpp::vm::Class::Init HOOK
#if __cplusplus >= 201703
    if (!old_Class$$Init) {
#else
        if (!Class$$Init) {
#endif
        auto Class$$Init_t = HexUtils::FindNext_B_BL_offset(HexUtils::FindNext_B_BL_offset((DWORD)dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_array_new_specific")), count), count);
#if __cplusplus >= 201703
        HOOK(Class$$Init_t, Class$$Init, old_Class$$Init);
#else
        Class$$Init = (decltype(Class$$Init)) Class$$Init_t;
#endif
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::Init in lib: 0x%x"), BNM::offsetInLib(Class$$Init_t));
    }
    //! il2cpp::vm::Image::GetTypes HOOK AND GET (OR ONLY GET)
#if __cplusplus >= 201703
    if (!old_Image$$GetTypes) {
#else
        if (!Image$$GetTypes) {
#endif
        DO_API(const BNM::IL2CPP::Il2CppImage*, il2cpp_get_corlib, ());
        DO_API(BNM::IL2CPP::Il2CppClass*, il2cpp_class_from_name, (const BNM::IL2CPP::Il2CppImage*, const char*, const char *));
        DO_API(const BNM::IL2CPP::MethodInfo*, il2cpp_class_get_method_from_name, (BNM::IL2CPP::Il2CppClass*, const char*, int));
        DWORD GetTypesAdr = 0;
        auto assemblyClass = il2cpp_class_from_name(il2cpp_get_corlib(), OBFUSCATE_BNM("System.Reflection"), OBFUSCATE_BNM("Assembly"));
        Class$$Init(assemblyClass);
        for (int i = 0; i < assemblyClass->method_count; i++) {
            const BNM::IL2CPP::MethodInfo *method = assemblyClass->methods[i];
            if (!BNM::CheckObj(method)) continue;
            if (!strcmp(OBFUSCATE_BNM("GetTypes"), method->name) && method->parameters_count == 1) {
                GetTypesAdr = (DWORD) method->methodPointer;
                break;
            }
        }
        const int sCount
#if UNITY_VER > 174
                = count + 1;
#else
        = count + 2;
#endif
        auto Image$$GetTypes_t = HexUtils::FindNext_B_BL_offset(HexUtils::FindNext_B_BL_offset(HexUtils::FindNext_B_BL_offset(GetTypesAdr, count), sCount), count);
#if __cplusplus >= 201703
        HOOK(Image$$GetTypes_t, Image$$GetTypes, old_Image$$GetTypes);
#else
        Image$$GetTypes = (decltype(Image$$GetTypes)) Image$$GetTypes_t;
#endif
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Image::GetTypes in lib: 0x%x"), BNM::offsetInLib(Image$$GetTypes_t));
    }
#if __cplusplus >= 201703
    //! il2cpp::vm::Class::FromIl2CppType HOOK
    if (!old_Class$$FromIl2CppType) {
        auto from_type_adr = HexUtils::FindNext_B_BL_offset((DWORD)dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_class_from_type")), count);
        HOOK(from_type_adr, Class$$FromIl2CppType, old_Class$$FromIl2CppType);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::FromIl2CppType in lib: 0x%x"), BNM::offsetInLib(from_type_adr));
    }
    //! il2cpp::vm::Class::FromName HOOK
    if (!old_Class$$FromName) {
        auto from_name_adr = HexUtils::FindNext_B_BL_offset(HexUtils::FindNext_B_BL_offset((DWORD)dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_class_from_name")), count), count);
        HOOK(from_name_adr, Class$$FromName, old_Class$$FromName);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::FromName in lib: 0x%x"), BNM::offsetInLib(from_name_adr));
    }
#if UNITY_VER <= 174
    //! il2cpp::vm::MetadataCache::GetImageFromIndex HOOK
        if (!old_GetImageFromIndex) {
            auto GetImageFromIndexOffset = HexUtils::FindNext_B_BL_offset(HexUtils::FindNext_B_BL_offset((DWORD)dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_assembly_get_image")), count), count);
            HOOK(GetImageFromIndexOffset, new_GetImageFromIndex, old_GetImageFromIndex);
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::MetadataCache::GetImageFromIndex in lib: 0x%x"), BNM::offsetInLib(GetImageFromIndexOffset));
        }
        static void *old_AssemblyLoad;
        //! il2cpp::vm::Assembly::Load HOOK
        if (!old_AssemblyLoad) {
            DWORD AssemblyLoadOffset = HexUtils::FindNext_B_BL_offset((DWORD)dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_domain_assembly_open")), count);
            HOOK(AssemblyLoadOffset, new_Assembly_Load, old_AssemblyLoad);
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::Load in lib: 0x%x"), BNM::offsetInLib(AssemblyLoadOffset));
        }
#endif
#endif
    //! il2cpp::vm::Assembly::GetAllAssemblies GET
    if (!Assembly$$GetAllAssemblies) {
        DO_API(BNM::IL2CPP::Il2CppImage*, il2cpp_get_corlib, ());
        DO_API(BNM::IL2CPP::Il2CppClass*, il2cpp_class_from_name, (BNM::IL2CPP::Il2CppImage*, const char*, const char*));
        DO_API(BNM::IL2CPP::MethodInfo*, il2cpp_class_get_method_from_name, (BNM::IL2CPP::Il2CppClass*, const char*, int));
        auto assemblyClass = il2cpp_class_from_name(il2cpp_get_corlib(), OBFUSCATE_BNM("System"), OBFUSCATE_BNM("AppDomain"));
        Class$$Init(assemblyClass);
        const BNM::IL2CPP::MethodInfo *getAssembly = nullptr;
        for (int i = 0; i < assemblyClass->method_count; i++) {
            const BNM::IL2CPP::MethodInfo *method = assemblyClass->methods[i];
            if (!BNM::CheckObj(method)) continue;
            if (!strcmp(OBFUSCATE_BNM("GetAssemblies"), method->name) && method->parameters_count == 1) {
                getAssembly = method;
                break;
            }
        }
        if (getAssembly) {
            DWORD GetTypesAdr = HexUtils::FindNext_B_BL_offset((DWORD) getAssembly->methodPointer, count);
            Assembly$$GetAllAssemblies = (BNM::AssemblyVector *(*)())(HexUtils::FindNext_B_BL_offset(GetTypesAdr, count+1));
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::GetAllAssemblies by AppDomain in lib: 0x%x"), BNM::offsetInLib((DWORD)Assembly$$GetAllAssemblies));
        } else {
            auto adr = (DWORD)dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_domain_get_assemblies"));
            Assembly$$GetAllAssemblies = (BNM::AssemblyVector *(*)())(HexUtils::FindNext_B_BL_offset(adr, count));
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::GetAllAssemblies by domain in lib: 0x%x"), BNM::offsetInLib((DWORD)Assembly$$GetAllAssemblies));
        }
    }
}
void (*old_BNM_il2cpp_init)(const char*);
void BNM_il2cpp_init(const char* domain_name) {
    old_BNM_il2cpp_init(domain_name);
    InitIl2cppMethods();
#if __cplusplus >= 201703
    InitNewClasses(Classes4Add);
#endif
    BNM_LibLoaded = true;
}
[[maybe_unused]] __attribute__((constructor))
void PrepareBNM() {
    std::thread([]() {
        do {
            BNM_dlLib = dlopen(OBFUSCATE_BNM("libil2cpp.so"), RTLD_LAZY);
            if (BNM_dlLib) {
                void *init = dlsym(BNM_dlLib, OBFUSCATE_BNM("il2cpp_init"));
                if (init) {
                    HOOK(init, BNM_il2cpp_init, old_BNM_il2cpp_init);
                    break;
                }
                dlclose(BNM_dlLib);
            }
        } while (true);
    }).detach();
}
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
    if (!this)
        return OBFUSCATE_BNM("ERROR: monoString is null");
    if (!isAllocated(chars))
        return OBFUSCATE_BNM("ERROR: chars is null");
    return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(std::wstring(chars, chars + length));
}
[[maybe_unused]] unsigned int BNM::MONO_STRUCTS::monoString::getHash() {
    if (!this || !isAllocated(chars))
        return 0;
    IL2CPP::Il2CppChar* p = chars;
    unsigned int h = 0;
    for (int i = 0; i < length; i++)
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
    if (monoString *empty = Empty())
        ret->klass = empty->klass;
    return (monoString*)ret;
}
BNM::MONO_STRUCTS::monoString *BNM::MONO_STRUCTS::monoString::Create(const std::string& str) { return Create(str2char(str)); }
[[maybe_unused]] BNM::MONO_STRUCTS::monoString *BNM::MONO_STRUCTS::monoString::Empty() { return LoadClass(OBFUSCATES_BNM("System"), OBFUSCATES_BNM("String")).GetFieldByName<monoString *>(OBFUSCATES_BNM("Empty"))(); }
[[maybe_unused]] void BNM::AttachIl2Cpp() {
    if (BNM_CurThread) return;
    DO_API(BNM::IL2CPP::Il2CppDomain*, il2cpp_domain_get, ());
    DO_API(BNM::IL2CPP::Il2CppThread*, il2cpp_thread_attach, (BNM::IL2CPP::Il2CppDomain*));
    BNM_CurThread = il2cpp_thread_attach(il2cpp_domain_get());
}
[[maybe_unused]] void BNM::DetachIl2Cpp() {
    if (!BNM_CurThread) return;
    DO_API(BNM::IL2CPP::Il2CppDomain*, il2cpp_domain_get, ());
    DO_API(void, il2cpp_thread_detach, (BNM::IL2CPP::Il2CppThread*));
    il2cpp_thread_detach(BNM_CurThread);
    BNM_CurThread = nullptr;
}
BNM::MONO_STRUCTS::monoString *BNM::CreateMonoString(const char *str) {
    DO_API(BNM::MONO_STRUCTS::monoString*, il2cpp_string_new, (const char *str));
    return il2cpp_string_new(str);
}
[[maybe_unused]] BNM::MONO_STRUCTS::monoString *BNM::CreateMonoString(const std::string& str) { return CreateMonoString(str2char(str)); }
void *BNM::getExternMethod(const std::string& str) {
    DO_API(void*, il2cpp_resolve_icall, (const char *));
    return il2cpp_resolve_icall(str.c_str());
}
char *BNM::str2char(const std::string& str) {
    char *c = new char[str.size() + 1];
    std::strcpy(c, str.data());
    return c;
}
[[maybe_unused]] void *BNM::UNITY_STRUCTS::RaycastHit::get_Collider() const {
#if UNITY_VER > 174
    static void *(*FromId)(int);
    if (!FromId)
        InitFunc(FromId, getExternMethod(OBFUSCATE_BNM("UnityEngine.Object::FindObjectFromInstanceID(System.Int32)")));
    return FromId(m_Collider);
#else
    return (void *)m_Collider;
#endif
}
