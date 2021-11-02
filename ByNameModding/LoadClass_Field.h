#pragma once

monoString *CreateMonoString(string str, monoString::CreateMethod way = monoString::C_LIKE);

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
            if (!statik) {
                init = IsNativeObjectAlive(_instance);
                instance = _instance;
            }
            thiz = thiz_;
            thread_static = thiz->offset == -1;
        }
    }

    DWORD GetOffset() {
        return thiz->offset;
    }

    T get() {
        if (!init || thread_static || (statik && (!thiz->parent || !thiz->parent->static_fields)))
            return T();
        if (statik) {
            return *(T *) ((uint64_t) thiz->parent->static_fields + thiz->offset);
        }
        return *(T *) ((uint64_t) instance + thiz->offset);
    }

    void set(T val) {
        if (!init || thread_static || (statik && (!thiz->parent || !thiz->parent->static_fields)))
            return;
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
        init = IsNativeObjectAlive(val) && thiz != 0;
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
        DO_API(Il2CppDomain*, il2cpp_domain_get, ());
        DO_API(const Il2CppAssembly**, il2cpp_domain_get_assemblies, (const Il2CppDomain * domain, size_t * size));
        DO_API(const Il2CppImage*, il2cpp_assembly_get_image, (const Il2CppAssembly * assembly));
        auto dom = il2cpp_domain_get();
        if (!dom) {
            return nullptr;
        }
        size_t assemb_count;
        auto assemblies = il2cpp_domain_get_assemblies(dom, &assemb_count);
        for (int i = 0; i < assemb_count; i++) {
            TypeVector clases;
            Image$$GetTypes(il2cpp_assembly_get_image(assemblies[i]), false, &clases);
            for (auto cls : clases){
                if (!cls) continue;
                if (CheckObj(cls->name) == _name && CheckObj(cls->namespaze) == _namespace){
                    return (Il2CppClass *)cls;
                }
            }
        }
        return nullptr;
    }

public:
    Il2CppClass *klass;

    LoadClass() {};

    LoadClass(Il2CppClass *clazz) {
        klass = clazz;
    }

    LoadClass(string namespaze, string clazz) {
        Il2CppClass *thisclass;
        int try_count = 0;
        do {
            thisclass = GetClassFromName(namespaze, clazz);
            if (try_count == 10 && !thisclass) {
                LOGIBNM(OBFUSCATE_BNM("Class: [%s].[%s] - not founded"), namespaze.c_str(), clazz.c_str());
                break;
            } else if (!thisclass) {
                try_count++;
            }
        } while (!thisclass);
        this->klass = thisclass;
    }

    LoadClass(string _namespace, string _name, string dllname) {
        Il2CppClass *thisclass = nullptr;
        DO_API(const Il2CppAssembly*, il2cpp_domain_assembly_open, (Il2CppDomain * domain, const char *name));
        DO_API(Il2CppDomain*, il2cpp_domain_get, ());
        DO_API(const Il2CppImage*, il2cpp_assembly_get_image, (const Il2CppAssembly * assembly));
        auto *dll = il2cpp_assembly_get_image(il2cpp_domain_assembly_open(il2cpp_domain_get(), dllname.c_str()));
        int try_count = 0;
        do {
            auto dom = il2cpp_domain_get();
            if (dom) {
                TypeVector clases;
                Image$$GetTypes(dll, false, &clases);
                for (auto cls : clases){
                    if (!cls) continue;
                    if (CheckObj(cls->name) == _name && CheckObj(cls->namespaze) == _namespace){
                        thisclass = (Il2CppClass*)cls;
                    }
                }
            }

            if (try_count == 10 && !thisclass) {
                LOGIBNM(OBFUSCATE_BNM("Class: [%s].[%s] - not founded"), _namespace.c_str(),
                        _name.c_str());
                return;
            } else if (!thisclass) {
                try_count++;
            }
        } while (!thisclass);
        klass = thisclass;
        if (!klass)
            LOGIBNM(OBFUSCATE_BNM("Class: [%s].[%s] - not founded"), _namespace.c_str(),
                    _name.c_str());
    }

    FieldInfo *GetFieldInfoByName(string name) {
        if (!klass) return nullptr;
        DO_API(FieldInfo *, il2cpp_class_get_field_from_name,
               (Il2CppClass * klass, const char *name));
        return il2cpp_class_get_field_from_name(klass, name.c_str());
    }

    template<typename T>
    Field<T> GetFieldByName(string name, void *instance = nullptr) {
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

    static DWORD GetOffset(const MethodInfo *methodInfo) {
        return (DWORD) methodInfo->methodPointer;
    }

    const MethodInfo *GetMethodInfoByName(string name, int paramcount) {
        if (!klass) return nullptr;
        DO_API(const MethodInfo*, il2cpp_class_get_method_from_name,
               (Il2CppClass * klass, const char *name, int argsCount));
        if (klass)
            return il2cpp_class_get_method_from_name(klass, name.c_str(), paramcount);
        return nullptr;
    }

    DWORD GetMethodOffsetByName(string name, int paramcount = -1) {
        if (!klass) return 0;
        const MethodInfo *res = nullptr;
        int try_count = 0;
        do {
            res = GetMethodInfoByName(name, paramcount);
            if (try_count == 10 && !res) {
                LOGIBNM(OBFUSCATE_BNM("Method: [%s].[%s]::[%s], %d - not founded"),
                        CheckObj(klass->namespaze), CheckObj(klass->name), CheckObj(name.c_str()),
                        paramcount);
                return 0;
            } else if (!res || abs((DWORD) GetOffset(res)) <
                               abs((DWORD) GetLibInfo(OBFUSCATE_BNM("libil2cpp.so")).startAddr)) {
                try_count++;
            } else if (abs((DWORD) GetOffset(res)) >
                       abs((DWORD) GetLibInfo(OBFUSCATE_BNM("libil2cpp.so")).startAddr)) {
                break;
            }
        } while (true);
        return GetOffset(res);
    }

    const MethodInfo *GetMethodInfoByName(string name, std::vector<string> params_names) {
        if (!klass) return nullptr;
        int paramcount = params_names.size();
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

        return nullptr;
    }
    DWORD GetMethodOffsetByName(string name, std::vector<string> params_names) {
        if (!klass) return 0;
        int paramcount = params_names.size();
        const MethodInfo *res = nullptr;
        int try_count = 0;
        do {
            res = GetMethodInfoByName(name, params_names);
            if (try_count == 10 && !res) {
                LOGIBNM(OBFUSCATE_BNM("Method: [%s].[%s]::[%s], %d - not founded"), CheckObj(klass->namespaze), CheckObj(klass->name), CheckObj(name.c_str()), paramcount);
                return 0;
            } else if (!res || abs((DWORD) GetOffset(res)) < abs((DWORD) GetLibInfo(OBFUSCATE_BNM("libil2cpp.so")).startAddr)) {
                try_count++;
            } else if (abs((DWORD) GetOffset(res)) > abs((DWORD) GetLibInfo(OBFUSCATE_BNM("libil2cpp.so")).startAddr)) {
                break;
            }
        } while (true);
        return GetOffset(res);
    }

    const MethodInfo *GetMethodInfoByName(string name, std::vector<string> params_names, std::vector<const Il2CppType *> params_types) {
        if (!klass) return nullptr;
        DO_API(Il2CppClass*, il2cpp_class_from_il2cpp_type, (const Il2CppType * type));
        DO_API(bool, il2cpp_type_equals, (const Il2CppType * type, const Il2CppType * otherType));
        int paramcount = params_names.size();
        if (paramcount != params_types.size()) return nullptr;
        for (int i = 0; i < klass->method_count; i++) {
            const MethodInfo *method = klass->methods[i];
            if (name == method->name && method->parameters_count == paramcount) {
                bool ok = true;
                for (int i = 0; i < paramcount; i++) {
                    auto param = method->parameters + i;
                    auto cls = il2cpp_class_from_il2cpp_type(param->parameter_type);
                    auto param_cls = il2cpp_class_from_il2cpp_type(params_types[i]);
                    if (param->name != params_names[i] || !(cls->name == param_cls->name && cls->namespaze == param_cls->namespaze)) {
                        ok = false;
                        break;
                    }
                }
                if (ok) {
                    return method;
                }
            }
        }
        return nullptr;
    }

    DWORD GetMethodOffsetByName(string name, std::vector<string> params_names, std::vector<const Il2CppType *> params_types) {
        if (!klass) return 0;
        int paramcount = params_names.size();
        if (paramcount != params_types.size()) return 0;
        const MethodInfo *res = nullptr;
        int try_count = 0;
        do {
            res = GetMethodInfoByName(name, params_names, params_types);
            if (try_count == 10 && !res) {
                LOGIBNM(OBFUSCATE_BNM("Method: [%s].[%s]::[%s], %d - not founded"), CheckObj(klass->namespaze), CheckObj(klass->name), CheckObj(name.c_str()), paramcount);
                return 0;
            } else if (!res || abs((DWORD) GetOffset(res)) < abs((DWORD) GetLibInfo(OBFUSCATE_BNM("libil2cpp.so")).startAddr)) {
                try_count++;
            } else if (abs((DWORD) GetOffset(res)) > abs((DWORD) GetLibInfo(OBFUSCATE_BNM("libil2cpp.so")).startAddr)) {
                break;
            }
        } while (true);
        return GetOffset(res);
    }

    const MethodInfo *GetMethodInfoByName(string name, std::vector<const Il2CppType *> params_types) {
        if (!klass) return nullptr;
        DO_API(Il2CppClass*, il2cpp_class_from_il2cpp_type, (const Il2CppType * type));
        DO_API(bool, il2cpp_type_equals, (const Il2CppType * type, const Il2CppType * otherType));
        int paramcount = params_types.size();
        for (int i = 0; i < klass->method_count; i++) {
            const MethodInfo *method = klass->methods[i];
            if (name == method->name && method->parameters_count == paramcount) {
                bool ok = true;
                for (int i = 0; i < paramcount; i++) {
                    auto param = method->parameters + i;
                    auto cls = il2cpp_class_from_il2cpp_type(param->parameter_type);
                    auto param_cls = il2cpp_class_from_il2cpp_type(params_types[i]);
                    if (!(cls->name == param_cls->name && cls->namespaze == param_cls->namespaze)) {
                        ok = false;
                        break;
                    }
                }
                if (ok) {
                    return method;
                }
            }
        }
        return nullptr;
    }

    DWORD GetMethodOffsetByName(string name, std::vector<const Il2CppType *> params_types) {
        if (!klass) return 0;
        int paramcount = params_types.size();
        const MethodInfo *res = nullptr;
        int try_count = 0;
        do {
            res = GetMethodInfoByName(name, params_types);
            if (try_count == 10 && !res) {
                LOGIBNM(OBFUSCATE_BNM("Method: [%s].[%s]::[%s], %d - not founded"), CheckObj(klass->namespaze), CheckObj(klass->name), CheckObj(name.c_str()), paramcount);
                return 0;
            } else if (!res || abs((DWORD) GetOffset(res)) < abs((DWORD) GetLibInfo(OBFUSCATE_BNM("libil2cpp.so")).startAddr)) {
                try_count++;
            } else if (abs((DWORD) GetOffset(res)) > abs((DWORD) GetLibInfo(OBFUSCATE_BNM("libil2cpp.so")).startAddr)) {
                break;
            }
        } while (true);
        return GetOffset(res);
    }

    static LoadClass GetLC_ByClassAndMethodName(string _namespace, string _name, string methodName) {
        LoadClass out;
        DO_API(Il2CppDomain*, il2cpp_domain_get, ());
        DO_API(const Il2CppImage*, il2cpp_assembly_get_image, (const Il2CppAssembly * assembly));
        DO_API(const Il2CppAssembly**, il2cpp_domain_get_assemblies, (const Il2CppDomain * domain, size_t * size));
        auto dom = il2cpp_domain_get();
        if (!dom) {
            return out;
        }
        size_t assemb_count;
        auto assemblies = il2cpp_domain_get_assemblies(dom, &assemb_count);
        for (int i = 0; i < assemb_count; i++) {
            TypeVector clases;
            Image$$GetTypes(il2cpp_assembly_get_image(assemblies[i]), false, &clases);
            for (auto cls : clases){
                if (!cls) continue;
                if (CheckObj(cls->name) == _name && CheckObj(cls->namespaze) == _namespace){
                    for (int i = 0; i < cls->method_count; i++) {
                        const MethodInfo *method = cls->methods[i];
                        if (method && methodName == CheckObj(method->name)) {
                            out = LoadClass((Il2CppClass*)cls);
                            return out;
                        }
                    }
                }
            }
        }
        return out;
    }

    template<typename T>
    monoArray<T> *NewArray(il2cpp_array_size_t length = 65535) {
        if (!klass) return nullptr;
        DO_API(Il2CppArray*, il2cpp_array_new, (Il2CppClass * elementTypeInfo, il2cpp_array_size_t length));
        return *(monoArray<T> **) il2cpp_array_new(klass, length);
    }

    template<typename T>
    monoList<T> *NewList() {
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
        DO_API(Il2CppObject*, il2cpp_value_box, (Il2CppClass * klass, void * data));
        return *(T *) il2cpp_value_box(klass, *(void **) obj);
    }

    const Il2CppType *GetIl2CppType() {
        if (!klass) return nullptr;
#if UNITY_VER > 172
        return &klass->byval_arg;
#else
        return klass->byval_arg;
#endif
    }

    MonoType *GetMonoType() {
        if (!klass) return nullptr;
        DO_API(Il2CppObject*, il2cpp_type_get_object, (const Il2CppType * type));
#if UNITY_VER > 172
        return (MonoType *) il2cpp_type_get_object(&klass->byval_arg);
#else
        return (MonoType *) il2cpp_type_get_object(klass->byval_arg);
#endif
    }

    void *CreateNewClass(void **args, int args_count = -1, std::vector<std::string> arg_names = {}) {
        if (!klass) return nullptr;
        DO_API(Il2CppObject*, il2cpp_runtime_invoke, (const MethodInfo * method, void *obj, void **params, Il2CppException **exc));
        const MethodInfo *method;
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

    Il2CppClass *GetIl2CppClass() {
        return klass;
    }
};

void *get_Extern_Method(string str) {
    DO_API(void*, il2cpp_resolve_icall, (const char *str));
    return il2cpp_resolve_icall(str.c_str());
}
char* conv2monoLikeStr(const char*arr, int len){
    int size = (len+1)*2;
    char out[size];
    memset(out, 0, size);
    int g = 0;
    for (int i = 0; i < size; i++){
        if (i== 0 || i % 2 == 0){
            out[i] = arr[g];
            g++;
        }
    }
    return out;
}
monoString *CreateMonoString(string str, monoString::CreateMethod crateMethod) {
    if (crateMethod == monoString::MONO) {
        static monoString *(*mono_CreateString)(void *, const char *);
        if (!mono_CreateString)
            InitFunc(mono_CreateString,
                     LoadClass(OBFUSCATES("System"), OBFUSCATES("String")).GetMethodOffsetByName(
                             OBFUSCATES("CreateString"), {LoadClass(OBFUSCATES("System"), OBFUSCATES("SByte")).GetIl2CppType()}));
        return mono_CreateString(NULL, str.c_str());
    } else if (crateMethod == monoString::IL2CPP) {
        DO_API(monoString*, il2cpp_string_new, (const char *str));
        return il2cpp_string_new(str.c_str());
    } else {
        int len = str.length();
        int size = (len + 1) * 2;
        monoString *out = (monoString*) malloc(sizeof(monoString) + size);
        memcpy((void*)out->chars, (void *)conv2monoLikeStr(str.c_str(), len), size);
        out->length = len;
        out->monitor = nullptr;
        return out;
    }
}