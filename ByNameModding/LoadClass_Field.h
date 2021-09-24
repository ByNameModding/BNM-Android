#pragma once
using namespace std;
#include "macros.h"
template<typename T>
struct Field {
    static bool CheckIfStatic(FieldInfo *fieldInfo) {
        if (!fieldInfo || !fieldInfo->type)
            return false;

        if ((fieldInfo->type->attrs & 0x10) != 0)
            return true;

        if (fieldInfo->offset == -1)
            LOGIBNM(OBFUSCATE_BNM("Thread static fields is not supported!"));
        return false;
    }

    FieldInfo *thiz;
    bool init;
    bool thread_static;
    void *instance;
    bool statik;
    Field() {};

    Field(FieldInfo *thiz_, void *_instance = NULL) {
        if (init = (thiz_ != NULL)) {
            statik = CheckIfStatic(thiz_);
            if (!statik){
                init = IsNativeObjectAlive(_instance);
                instance = _instance;
            }
            thiz = thiz_;
            thread_static = thiz->offset == -1;
            if (statik && !thiz->parent->static_fields &&
                thiz->parent->static_fields_size) /*** Init static fields ***/
            {
                thiz->parent->static_fields = calloc(1, thiz->parent->static_fields_size);
            }
        }
    }

    DWORD get_offset() {
        return thiz->offset;
    }

    T get() {
        if (!init || thread_static || (statik && (!thiz->parent || !thiz->parent->static_fields))) return T();
        if (statik) {
            return *(T *) ((uint64_t) thiz->parent->static_fields + thiz->offset);
        }
        return *(T *) ((uint64_t) instance + thiz->offset);
    }

    void set(T val) {
        if (!init || thread_static || (statik && (!thiz->parent || !thiz->parent->static_fields))) return;
        if (statik) {
            *(T *) ((uint64_t) thiz->parent->static_fields + thiz->offset) = val;
            return;
        }
        *(T *) ((uint64_t) instance + thiz->offset) = val;
    }

    operator T() {
        return get();
    }

    Field operator=(T val) {
        set(val);
        return *this;
    }


    void setInstance(void *val) {
        init = IsNativeObjectAlive(val);
        instance = val;
    }

    void setInstanceDanger(void *val) {
        init = true;
        instance = val;
    }

    bool operator==(Field other) {
        if (!init) return false;
        return (thiz->type == other.type && thiz->parent == other.parent &&
                thiz->offset == other.offset && thiz->name == other.name &&
                thiz->token == other.token);
    }

    T operator()() {
        return get();
    }
};

class LoadClass {
    Il2CppClass *GetClassFromName(string _namespace, string _name) {
        DO_API(Il2CppClass*, il2cpp_class_from_name,
               (const Il2CppImage * image, const char* namespaze, const char *name));
        DO_API(Il2CppDomain*, il2cpp_domain_get, ());
        DO_API(const Il2CppAssembly**, il2cpp_domain_get_assemblies,
               (const Il2CppDomain * domain, size_t * size));
        DO_API(const Il2CppImage*, il2cpp_assembly_get_image,
               (const Il2CppAssembly * assembly));
        auto dom = il2cpp_domain_get();
        if (!dom) {
            return nullptr;
        }
        size_t assemb_count;
        const Il2CppAssembly **allAssemb = il2cpp_domain_get_assemblies(dom, &assemb_count);
        if (!_new) {
            for (int i = 0; i < assemb_count; i++) {
                auto assemb = allAssemb[i];
                auto img = il2cpp_assembly_get_image(assemb);
                if (!img) {
                    continue;
                }
                auto klass = il2cpp_class_from_name(img, _namespace.c_str(), _name.c_str());
                if (klass) {
                    return klass;
                }
            }
        } else {
            Metadata *metadata = MetaDataUtils::CrateMetadata();
            uint32_t imageCount = metadata->header->imagesCount / sizeof(Il2CppImageDefinition);
            for (int imageIndex = 0; imageIndex < imageCount; ++imageIndex) {
                const Il2CppImageDefinition *imageDefinition = metadata->getImageDefinitionByIndex(
                        imageIndex);
                uint32_t typeEnd = imageDefinition->typeStart + imageDefinition->typeCount;
                for (int32_t i = imageDefinition->typeStart; i < typeEnd; i++) {
                    const Il2CppTypeDefinition *typeDefinition = metadata->getTypeDefinitionByIndex(
                            i);
                    auto name = metadata->GetStringFromIndex(typeDefinition->nameIndex);
                    auto namespace_ = metadata->GetStringFromIndex(typeDefinition->namespaceIndex);
                    if (CheckObj(name) && CheckObj(namespace_)) {
                        if (name == _name && _namespace == namespace_) {
                            ClassIndex = i;
                            auto thisclass = metadata->GetTypeInfoFromTypeDefinitionIndex(i);
                            return thisclass;
                        }
                    }
                }
            }
        }
        return nullptr;
    }

    bool _new;
public:
    Il2CppClass *klass;

    int32_t ClassIndex;

    LoadClass() {};

    LoadClass(Il2CppClass *clazz, bool _new = false) {
        this->_new = _new;
        klass = clazz;
    }

    LoadClass(string namespaze, string clazz, bool _new = false) {
        Il2CppClass *thisclass;
        this->_new = _new;
        int try_count = 0;
        do {
            thisclass = GetClassFromName(namespaze, clazz);
            if (try_count == 2 && !thisclass) {
                LOGIBNM(OBFUSCATE_BNM("Class: %s, %s - not founded"), namespaze.c_str(),
                        clazz.c_str());
                break;
            } else if (!thisclass) {
                sleep(1);
                try_count++;
            }
        } while (!thisclass);
        klass = thisclass;
    }

    LoadClass(string _namespace, string _name, string dllname, bool _new = false) {
        this->_new = _new;
        Il2CppClass *thisclass = nullptr;
        if (!_new) {
            DO_API(const Il2CppAssembly*, il2cpp_domain_assembly_open,
                   (Il2CppDomain * domain, const char *name));
            DO_API(const Il2CppImage*, il2cpp_assembly_get_image,
                   (const Il2CppAssembly * assembly));
            DO_API(Il2CppDomain*, il2cpp_domain_get, ());
            DO_API(Il2CppClass*, il2cpp_class_from_name,
                   (const Il2CppImage * image, const char* namespaze, const char *name));
            auto *dll = il2cpp_assembly_get_image(
                    il2cpp_domain_assembly_open(il2cpp_domain_get(), dllname.c_str()));
            int try_count = 0;
            do {
                thisclass = il2cpp_class_from_name(dll, _namespace.c_str(), _name.c_str());
                if (try_count == 2 && !thisclass) {
                    LOGIBNM(OBFUSCATE_BNM("Class: %s, %s - not founded"), _namespace.c_str(),
                            _name.c_str());
                    return;
                } else if (!thisclass) {
                    sleep(1);
                    try_count++;
                }
            } while (!thisclass);
        } else {
            Metadata *metadata = MetaDataUtils::CrateMetadata();
            int try_count = 0;
            do {
                uint32_t imageCount = metadata->header->imagesCount / sizeof(Il2CppImageDefinition);
                for (int imageIndex = 0; imageIndex < imageCount; ++imageIndex) {
                    if (thisclass) break;
                    const Il2CppImageDefinition *imageDefinition = metadata->getImageDefinitionByIndex(
                            imageIndex);
                    auto imdefname = metadata->GetStringFromIndex(imageDefinition->nameIndex);
                    if (!isNOT_Allocated(imdefname) &&
                        (std::string(imdefname).find(std::string(dllname)) != std::string::npos ||
                         dllname == imdefname)) {
                        uint32_t typeEnd = imageDefinition->typeStart + imageDefinition->typeCount;
                        for (int32_t i = imageDefinition->typeStart; i < typeEnd; i++) {
                            const Il2CppTypeDefinition *typeDefinition = metadata->getTypeDefinitionByIndex(
                                    i);
                            auto name = metadata->GetStringFromIndex(typeDefinition->nameIndex);
                            auto namespace_ = metadata->GetStringFromIndex(
                                    typeDefinition->namespaceIndex);
                            if (CheckObj(name) && CheckObj(namespace_)) {
                                if (name == _name && namespace_ == _namespace) {
                                    thisclass = metadata->GetTypeInfoFromTypeDefinitionIndex(i);
                                    ClassIndex = i;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (try_count == 2 && !thisclass) {
                    LOGIBNM(OBFUSCATE_BNM("Class: %s, %s - not founded"), _namespace.c_str(),
                            _name.c_str());
                    break;
                } else if (!thisclass) {
                    sleep(1);
                    try_count++;
                }
            } while (!thisclass);
        }
        klass = thisclass;
        if (!klass)
            LOGIBNM(OBFUSCATE_BNM("Class: %s, %s - not founded"), _namespace.c_str(),
                    _name.c_str());
    }

    FieldInfo *GetFieldInfoByName(string name) {
        if (!klass) return nullptr;
        if (_new) {
            Metadata *metadata = MetaDataUtils::CrateMetadata();
            FieldIndex start = klass->typeDefinition->fieldStart;
            FieldIndex end = start + klass->typeDefinition->field_count;
            for (FieldIndex index = start; index < end; ++index) {
                const Il2CppFieldDefinition *fieldDefinition = metadata->getFieldDefinitionByIndex(
                        index);
                auto _name = metadata->GetStringFromIndex(fieldDefinition->nameIndex);
                if (CheckObj(_name) != NULL && name == _name) {
                    FieldInfo *ret = metadata->GetFieldInfoFromIndexAndClass(index,
                                                                             metadata->GetTypeInfoFromTypeDefinitionIndex(
                                                                                     ClassIndex));
                    return ret;
                }
            }
        } else {
            DO_API(FieldInfo *, il2cpp_class_get_field_from_name,
                   (Il2CppClass * klass, const char *name));
            return il2cpp_class_get_field_from_name(klass, name.c_str());
        }
        return nullptr;
    }

    template<typename T>
    Field<T> GetFieldByName(string name, void *instance) {
        auto fieldInfo = GetFieldInfoByName(name);
        if (!fieldInfo) return Field<T>();
        return Field<T>(fieldInfo, instance);
    }

    DWORD GetFieldOffset(string name) {
        if (!klass) return 0;
        return GetOffset(GetFieldInfoByName(name));
    }

    static DWORD GetOffset(FieldInfo *filed) {
        return filed->offset;
    }

    static DWORD GetOffset(MethodInfo *methodInfo) {
        return (DWORD) methodInfo->methodPointer;
    }

    const MethodInfo *GetMethodInfoByName(string name, int paramcount) {
        if (!klass) return nullptr;
        if (_new) {
            Metadata *metadata = MetaDataUtils::CrateMetadata();
            MethodIndex start = klass->typeDefinition->methodStart;
            MethodIndex end = start + klass->method_count;
            for (MethodIndex index = start; index < end; ++index) {
                const Il2CppMethodDefinition *methodDefinition = metadata->GetMethodDefinitionFromIndex(
                        index);
                auto _name = metadata->GetStringFromIndex(methodDefinition->nameIndex);
                if (CheckObj(_name) != NULL && name == _name &&
                    methodDefinition->parameterCount == paramcount) {
                    return metadata->GetMethodInfoFromIndex(index);
                }
            }
        } else {
            DO_API(const MethodInfo*, il2cpp_class_get_method_from_name,
                   (Il2CppClass * klass, const char *name, int argsCount));
            if (klass)
                return il2cpp_class_get_method_from_name(klass, name.c_str(), paramcount);
        }
        return nullptr;
    }

    DWORD GetMethodOffsetByName(string name, int paramcount) {
        if (!klass) return 0;
        const MethodInfo *res = nullptr;
        int try_count = 0;
        do {
            res = GetMethodInfoByName(name, paramcount);
            if (try_count == 3 && !res) {
                LOGIBNM(OBFUSCATE_BNM("Method: %s.%s::%s, %d - not founded"), CheckObj(klass->namespaze), CheckObj(klass->name), CheckObj(name.c_str()), paramcount);
                return 0;
            } else if (!res || abs((DWORD) res->methodPointer) <
                               abs((DWORD) GetLibInfo(OBFUSCATE_BNM("libil2cpp.so")).startAddr)) {
                sleep(1);
                try_count++;
            } else if (abs((DWORD) res->methodPointer) >
                       abs((DWORD) GetLibInfo(OBFUSCATE_BNM("libil2cpp.so")).startAddr)) {
                break;
            }
        } while (true);
        return (DWORD) res->methodPointer;
    }

    const MethodInfo *GetMethodInfoByName(string name, std::vector<string>params_names) {
        if (!klass) return nullptr;
        int paramcount = params_names.size();
        if (_new) {
            Metadata *metadata = MetaDataUtils::CrateMetadata();
            auto typeDefinition = metadata->getTypeDefinitionByIndex(ClassIndex);
            MethodIndex start = typeDefinition->methodStart;
            MethodIndex end = start + klass->method_count;
            for (MethodIndex index = start; index < end; ++index) {
                const Il2CppMethodDefinition *methodDefinition = metadata->GetMethodDefinitionFromIndex(
                        index);
                auto _name = metadata->GetStringFromIndex(methodDefinition->nameIndex);
                if (CheckObj(_name) != NULL && name == _name &&
                    methodDefinition->parameterCount == paramcount) {
                    ParameterIndex parameterStart = methodDefinition->parameterStart;
                    ParameterIndex parameterEnd = parameterStart + methodDefinition->parameterCount;
                    bool ok = true;
                    int paramIndex = 0;
                    for (ParameterIndex i = parameterStart; i < parameterEnd; i++) {
                        auto param = metadata->GetParameterDefinitionFromIndex(i);
                        if (metadata->GetStringFromIndex(param->nameIndex) != params_names[paramIndex]) {
                            ok = false;
                            break;
                        }
                        paramIndex++;
                    }
                    if (ok) return metadata->GetMethodInfoFromIndex(index);
                }
            }
        } else {
            for (int i = 0; i < klass->method_count; i++) {
                const MethodInfo *method = klass->methods[i];
                if (name == method->name && method->parameters_count == paramcount) {
                    bool ok = true;
                    for (int i = 0; i < paramcount; i++) {
                        auto param = method->parameters + i;
                        if (param->name != params_names[i]) {
                            ok = false;
                            break;
                        }
                    }
                    if (ok) {
                        return method;
                    }
                }
            }
        }
        return nullptr;
    }

    DWORD GetMethodOffsetByName(string name, std::vector<string> params_names) {
        if (!klass) return 0;
        int paramcount = params_names.size();
        const MethodInfo *res = nullptr;
        int try_count = 0;
        do {
            res = GetMethodInfoByName(name, params_names);
            if (try_count == 3 && !res) {
                LOGIBNM(OBFUSCATE_BNM("Method: %s.%s::%s, %d - not founded"), CheckObj(klass->namespaze), CheckObj(klass->name), CheckObj(name.c_str()), paramcount);
                return 0;
            } else if (!res || abs((DWORD) res->methodPointer) <
                               abs((DWORD) GetLibInfo(OBFUSCATE_BNM("libil2cpp.so")).startAddr)) {
                sleep(1);
                try_count++;
            } else if (abs((DWORD) res->methodPointer) >
                       abs((DWORD) GetLibInfo(OBFUSCATE_BNM("libil2cpp.so")).startAddr)) {
                break;
            }
        } while (true);
        return (DWORD) res->methodPointer;
    }

    static LoadClass
    GetLC_ByClassAndMethodName(string _namespace, string _name, string methodName) {
        LoadClass out;
        Metadata *metadata = MetaDataUtils::CrateMetadata();
        uint32_t imageCount = metadata->header->imagesCount / sizeof(Il2CppImageDefinition);
        for (int imageIndex = 0; imageIndex < imageCount; ++imageIndex) {
            const Il2CppImageDefinition *imageDefinition = metadata->getImageDefinitionByIndex(
                    imageIndex);
            uint32_t typeEnd = imageDefinition->typeStart + imageDefinition->typeCount;
            for (int32_t i = imageDefinition->typeStart; i < typeEnd; i++) {
                const Il2CppTypeDefinition *typeDefinition = metadata->getTypeDefinitionByIndex(
                        i);
                auto name = metadata->GetStringFromIndex(typeDefinition->nameIndex);
                auto namespace_ = metadata->GetStringFromIndex(
                        typeDefinition->namespaceIndex);
                if (CheckObj(name) != NULL && CheckObj(namespace_) != NULL) {
                    if (_name == name && _namespace == namespace_) {
                        MethodIndex start = typeDefinition->methodStart;
                        MethodIndex end = start + typeDefinition->method_count;
                        for (MethodIndex index = start; index < end; ++index) {
                            const Il2CppMethodDefinition *methodDefinition = metadata->GetMethodDefinitionFromIndex(
                                    index);
                            auto _name = metadata->GetStringFromIndex(methodDefinition->nameIndex);
                            if (methodName == _name) {
                                out = LoadClass(metadata->GetTypeInfoFromTypeDefinitionIndex(i),
                                                true);
                            }
                        }
                    }
                }
            }
        }
        return out;
    }
    template<typename T>
    monoArray<T> *NewArray(il2cpp_array_size_t length = 65535){
        if (!klass) return nullptr;
        DO_API(Il2CppArray*, il2cpp_array_new, (Il2CppClass * elementTypeInfo, il2cpp_array_size_t length));
        return *(monoArray<T> **) il2cpp_array_new(klass, length);
    }

    template<typename T>
    monoList<T> *NewList(){
        if (!klass) return nullptr;
        DO_API(Il2CppObject*, il2cpp_object_new, (const Il2CppClass * klass));
        monoArray<T> *array = NewArray<T>();
        const Il2CppClass *ArrClass = array->klass;
        monoList<T> *lst = il2cpp_object_new(ArrClass);
        lst->Items = array;
        return lst;
    }

    template<typename T>
    T BoxObject(T obj) {
        if (!klass) return nullptr;
        DO_API(Il2CppObject*, il2cpp_value_box, (Il2CppClass * klass, void* data));
        return *(T*)il2cpp_value_box(klass, *(void**)obj);
    }
	
	template<typename T>
    static T UnBoxObject(T obj) {
        void* val = (void*)(((char*)obj) + sizeof(Il2CppObject));
        return *(T*) val;
    }

    const Il2CppType *GetIl2CppType() {
        if (!klass) return nullptr;
#if IL2CPP_VERSION > 240
        return &klass->byval_arg;
#else
        return klass->byval_arg;
#endif
    }
    MonoType *GetMonoType() {
        if (!klass) return nullptr;
        DO_API(Il2CppObject*, il2cpp_type_get_object, (const Il2CppType * type));
#if IL2CPP_VERSION > 240
        return (MonoType *) il2cpp_type_get_object(&klass->byval_arg);
#else
        return (Il2CppReflectionType *) il2cpp_type_get_object(klass->byval_arg);
#endif
    }

    void *CreateNewClass(void **args, int args_count, std::vector<std::string> arg_names = {}) {
        if (!klass) return nullptr;
        DO_API(Il2CppObject*, il2cpp_runtime_invoke,
               (const MethodInfo * method, void *obj, void **params, Il2CppException **exc));
        const MethodInfo* method;
        method = arg_names.empty() ? GetMethodInfoByName(OBFUSCATES_BNM(".ctor"), args_count)
                                   : GetMethodInfoByName(OBFUSCATES_BNM(".ctor"), arg_names);
        Il2CppObject *instance = (Il2CppObject *) CreateNewInstance();
        il2cpp_runtime_invoke(method, instance, args, NULL);
        return (void *) instance;
    }
    void *CreateNewInstance() {
        if (!klass) return nullptr;
        DO_API(Il2CppObject*, il2cpp_object_new, (const Il2CppClass * klass));
        return (void *) il2cpp_object_new(klass);
    }
};

void *get_Extern_Method(string str) {
    DO_API(void*, il2cpp_resolve_icall, (const char *str));
    return il2cpp_resolve_icall(str.c_str());
}
monoString *(*mono_CreateString)(void *, const char *);
monoString *
CreateMonoString(string str, bool il2cpp = true) {
    if (!il2cpp) {
        if (!mono_CreateString)
            InitFunc(mono_CreateString, LoadClass(OBFUSCATE("System"), OBFUSCATE("String")).GetMethodOffsetByName(OBFUSCATE("CreateString"), 1));
        return mono_CreateString(NULL, str.c_str());
    } else {
        DO_API(monoString*, il2cpp_string_new_wrapper, (const char *str));
        return il2cpp_string_new_wrapper(str.c_str());
    }
}
