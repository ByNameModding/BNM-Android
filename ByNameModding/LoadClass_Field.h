#pragma once


#include <Unity/ByNameModding/Il2CppTypeDefs/il2cpp_mono_types.h>

template<typename T>
struct Field {
    static bool CheckIfStatic(FieldInfo *fieldInfo) {
        if (!fieldInfo || !fieldInfo->type)
            return false;

        if (fieldInfo->type->attrs & 0x0010)
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
                init = _instance != nullptr;
                instance = _instance;
            }
            thiz = thiz_;
            thread_static = thiz->offset == -1;
        }
    }

    DWORD GetOffset() {
        return thiz->offset;
    }

    T* getPointer() {
        if (!init || thread_static || (statik && (!thiz->parent || !thiz->parent->static_fields)))
            return 0;

        if (statik) {
            return (T *) ((uint64_t) thiz->parent->static_fields + thiz->offset);
        }
        return (T *) ((uint64_t) instance + thiz->offset);
    }

    T get() {
        if (!init || thread_static || (statik && (!thiz->parent || !thiz->parent->static_fields)))
            return T();
        return *getPointer();
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


    Field<T> setInstance(void *val) {
        init = val && thiz != 0;
        instance = val;
        return *this;
    }

    Field<T> setInstanceDanger(void *val) {
        init = true;
        instance = val;
        return *this;
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
        DO_API(const Il2CppImage*, il2cpp_assembly_get_image, (const Il2CppAssembly * assembly));
        for (auto asmb : *Assembly$$GetAllAssemblies()) {
            TypeVector clases;
            Image$$GetTypes(il2cpp_assembly_get_image(asmb), false, &clases);
            for (auto cls : clases){
                if (!cls) continue;
                Class$$Init(cls);
                if (cls->name == _name && cls->namespaze == _namespace){
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
    LoadClass(Il2CppType *type) {
        DO_API(Il2CppClass*, il2cpp_class_from_il2cpp_type, (const Il2CppType * type));
        klass = il2cpp_class_from_il2cpp_type(type);
    }
    LoadClass(MonoType *type) {
        DO_API(Il2CppClass*, il2cpp_class_from_il2cpp_type, (const Il2CppType * type));
        klass = il2cpp_class_from_il2cpp_type(type->type);
    }
    LoadClass(Il2CppObject *obj) {
        klass = obj->klass;
    }
    LoadClass(string namespaze, string clazz) {
        klass = GetClassFromName(namespaze, clazz);
        if (!klass) {
            LOGIBNM(OBFUSCATE_BNM("Class: [%s].[%s] - not founded"), namespaze.c_str(), clazz.c_str());
        }
    }

    LoadClass(string _namespace, string _name, string dllname) {
        DO_API(const Il2CppImage*, il2cpp_assembly_get_image, (const Il2CppAssembly * assembly));
        const Il2CppImage *dll;
        for (auto asemb : *Assembly$$GetAllAssemblies()){
            if (dllname == il2cpp_assembly_get_image(asemb)->name)
                dll = il2cpp_assembly_get_image(asemb);
        }
        if (!dll){
            LOGIBNM(OBFUSCATE_BNM("Dll: [%s] - not founded"), dllname.c_str());
            klass = 0;
            return;
        }
        TypeVector clases;
        Image$$GetTypes(dll, false, &clases);
        for (auto cls : clases){
            if (!cls) continue;
            Class$$Init(cls);
            if (cls->name == _name && cls->namespaze == _namespace){
                klass = cls;
                break;
            }
        }
        if (!klass) {
            LOGIBNM(OBFUSCATE_BNM("Class: [%s].[%s] - not founded"), _namespace.c_str(), _name.c_str());
        }
    }

    FieldInfo *GetFieldInfoByName(string name) {
        if (!klass) return nullptr;
        Class$$Init(klass);
        DO_API(FieldInfo *, il2cpp_class_get_field_from_name,
               (Il2CppClass * klass, const char *name));
        auto ret = il2cpp_class_get_field_from_name(klass, str2char(name));
        return ret;
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

    static DWORD GetOffset(FieldInfo *field) {
        return field->offset;
    }

    static DWORD GetOffset(MethodInfo *methodInfo) {
        return (DWORD) methodInfo->methodPointer;
    }

    MethodInfo *GetMethodInfoByName(string name, int paramcount) {
        if (!klass) return nullptr;
        Class$$Init(klass);
        DO_API(MethodInfo*, il2cpp_class_get_method_from_name,
               (Il2CppClass * klass, const char *name, int argsCount));
        auto ret = il2cpp_class_get_method_from_name(klass, name.c_str(), paramcount);
        return ret;
    }

    DWORD GetMethodOffsetByName(string name, int paramcount = -1) {
        if (!klass) return 0;
        MethodInfo *res = GetMethodInfoByName(name, paramcount);
        if (!res) {
            LOGIBNM(OBFUSCATE_BNM("Method: [%s].[%s]::[%s], %d - not founded"), klass->namespaze, klass->name, name.c_str(), paramcount);
            return 0;
        }
        return GetOffset(res);
    }

    MethodInfo *GetMethodInfoByName(string name, std::vector<string> params_names) {
        if (!klass) return nullptr;
        MethodInfo *ret = 0;
        Class$$Init(klass);
        int paramcount = params_names.size();
        for (int i = 0; i < klass->method_count; i++) {
            MethodInfo *method = (MethodInfo *)klass->methods[i];
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
                    ret = method;
                    break;
                }
            }
        }
        return ret;
    }
    DWORD GetMethodOffsetByName(string name, std::vector<string> params_names) {
        if (!klass) return 0;
        int paramcount = params_names.size();
        MethodInfo *res = GetMethodInfoByName(name, params_names);
        if (!res) {
            LOGIBNM(OBFUSCATE_BNM("Method: [%s].[%s]::[%s], %d - not founded"), klass->namespaze, klass->name, name.c_str(), paramcount);
            return 0;
        }
        return GetOffset(res);
    }

    MethodInfo *GetMethodInfoByName(string name, std::vector<string> params_names, std::vector<const Il2CppType *> params_types) {
        if (!klass) return nullptr;
        MethodInfo *ret = 0;
        Class$$Init(klass);
        DO_API(Il2CppClass*, il2cpp_class_from_il2cpp_type, (const Il2CppType * type));
        DO_API(bool, il2cpp_type_equals, (const Il2CppType * type, const Il2CppType * otherType));
        int paramcount = params_names.size();
        if (paramcount != params_types.size()) return nullptr;
        for (int i = 0; i < klass->method_count; i++) {
            MethodInfo *method = (MethodInfo *)klass->methods[i];
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
                    ret = method;
                    break;
                }
            }
        }
        return ret;
    }

    DWORD GetMethodOffsetByName(string name, std::vector<string> params_names, std::vector<const Il2CppType *> params_types) {
        if (!klass) return 0;
        int paramcount = params_names.size();
        if (paramcount != params_types.size()) return 0;
        MethodInfo *res = GetMethodInfoByName(name, params_names, params_types);
        if (!res) {
            LOGIBNM(OBFUSCATE_BNM("Method: [%s].[%s]::[%s], %d - not founded"), klass->namespaze, klass->name, name.c_str(), paramcount);
            return 0;
        }
        return GetOffset(res);
    }

    MethodInfo *GetMethodInfoByName(string name, std::vector<const Il2CppType *> params_types) {
        if (!klass) return nullptr;
        MethodInfo *ret = 0;
        Class$$Init(klass);
        DO_API(Il2CppClass*, il2cpp_class_from_il2cpp_type, (const Il2CppType * type));
        DO_API(bool, il2cpp_type_equals, (const Il2CppType * type, const Il2CppType * otherType));
        int paramcount = params_types.size();
        for (int i = 0; i < klass->method_count; i++) {
            MethodInfo *method = (MethodInfo *)klass->methods[i];
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
                    ret = method;
                    break;
                }
            }
        }
        return ret;
    }

    DWORD GetMethodOffsetByName(string name, std::vector<const Il2CppType *> params_types) {
        if (!klass) return 0;
        int paramcount = params_types.size();
        MethodInfo *res = GetMethodInfoByName(name, params_types);
        if (!res) {
            LOGIBNM(OBFUSCATE_BNM("Method: [%s].[%s]::[%s], %d - not founded"), klass->namespaze, klass->name, name.c_str(), paramcount);
            return 0;
        }
        return GetOffset(res);
    }

    static LoadClass GetLC_ByClassAndMethodName(string _namespace, string _name, string methodName) {
        LoadClass out;
        DO_API(Il2CppImage*, il2cpp_assembly_get_image, (const Il2CppAssembly * assembly));
        for (auto asmb : *Assembly$$GetAllAssemblies()) {
            TypeVector clases;
            if (!CheckObj(asmb)) continue;
            Image$$GetTypes(il2cpp_assembly_get_image(asmb), false, &clases);
            bool found = false;
            for (auto cls : clases){
                if (!CheckObj(cls)) continue;
                Class$$Init(cls);
                if (CheckObj(cls->name) == _name && CheckObj(cls->namespaze) == _namespace){
                    for (int i = 0; i < cls->method_count; i++) {
                        MethodInfo *method = (MethodInfo *)cls->methods[i];
                        if (method && methodName == method->name) {
                            out = LoadClass((Il2CppClass*)cls);
                            found = true;
                            break;
                        }
                    }
                    if (found)
                        break;
                }
            }
            if (found)
                break;
        }
        return out;
    }

    template<typename T>
    monoArray<T> *NewArray(il2cpp_array_size_t length = 65535) {
        if (!klass) return nullptr;
        Class$$Init(klass);
        DO_API(Il2CppArray*, il2cpp_array_new, (Il2CppClass * elementTypeInfo, il2cpp_array_size_t length));
        return (monoArray<T> *) il2cpp_array_new(klass, length);
    }

    template<typename T>
    monoList<T> *NewList() {
        if (!klass) return nullptr;
        Class$$Init(klass);
        DO_API(Il2CppObject*, il2cpp_object_new, (const Il2CppClass * klass));
        monoArray<T> *array = NewArray<T>();
        const Il2CppClass *ArrClass = array->klass;
        monoList<T> *lst = il2cpp_object_new(ArrClass);
        lst->Items = array;
        return lst;
    }

    template<typename T>
    Il2CppObject* BoxObject(T obj) {
        if (!klass) return nullptr;
        Class$$Init(klass);
        DO_API(Il2CppObject*, il2cpp_value_box, (Il2CppClass * klass, void * data));
        return il2cpp_value_box(klass, (void *) obj);
    }

    Il2CppType *GetIl2CppType() {
        if (!klass) return nullptr;
        Class$$Init(klass);
#if UNITY_VER > 174
        return (Il2CppType *)&klass->byval_arg;
#else
        return (Il2CppType *)klass->byval_arg;
#endif
    }

    MonoType *GetMonoType() {
        if (!klass) return nullptr;
        Class$$Init(klass);
        DO_API(Il2CppObject*, il2cpp_type_get_object, (const Il2CppType * type));
#if UNITY_VER > 174
        return (MonoType *) il2cpp_type_get_object(&klass->byval_arg);
#else
        return (MonoType *) il2cpp_type_get_object(klass->byval_arg);
#endif
    }

    template<typename ...Args>
    void *CreateNewObjectCtor(int args_count, std::vector<std::string> arg_names, Args ... args) {
        if (!klass) return nullptr;
        Class$$Init(klass);
        MethodInfo *method = arg_names.empty() ? GetMethodInfoByName(OBFUSCATES_BNM(".ctor"), args_count)
                                                     : GetMethodInfoByName(OBFUSCATES_BNM(".ctor"), arg_names);
        Il2CppObject *instance = (Il2CppObject *) CreateNewInstance();
        void (*ctor)(...);
        InitFunc(ctor, method->methodPointer);
        ctor(instance, args...);
        return (void *) instance;
    }
    template<typename ...Args>
    void *CreateNewObject(Args ... args) {
        return CreateNewObjectCtor(sizeof...(Args), {}, args...);
    }

    void *CreateNewInstance() {
        if (!klass) return nullptr;
        Class$$Init(klass);
        DO_API(Il2CppObject*, il2cpp_object_new, (const Il2CppClass * klass));
        return (void *) il2cpp_object_new(klass);
    }

    Il2CppClass *GetIl2CppClass() {
        Class$$Init(klass);
        return klass;
    }
    std::string GetClassName(){
        if (klass){
            Class$$Init(klass);
            return OBFUSCATES_BNM("[") + klass->namespaze + OBFUSCATES_BNM("]::[") + klass->name + OBFUSCATES_BNM("]");
        }
        return OBFUSCATES_BNM("Class not loaded!");
    }
};

monoString *monoString::Empty() {
    return LoadClass(OBFUSCATES("System"), OBFUSCATES("String")).GetFieldByName<monoString *>(OBFUSCATES("Empty"))();
}
monoString *CreateMonoString(const char *str) {
    DO_API(monoString*, il2cpp_string_new, (const char *str));
    return il2cpp_string_new(str);
}
monoString *CreateMonoString(std::string str) {
    return CreateMonoString(str2char(str));
}

void *getExternMethod(string str) {
    DO_API(void*, il2cpp_resolve_icall, (const char *str));
    return il2cpp_resolve_icall(str.c_str());
}

template<typename TKey, typename TValue>
void NET4x::monoDictionary<TKey, TValue>::Add(TKey key, TValue value) {
    return BetterCall<void>(LoadClass((Il2CppObject *)this).GetMethodInfoByName(OBFUSCATE_BNM("Add"), 2), this, key, value);
}

template<typename TKey, typename TValue>
bool NET4x::monoDictionary<TKey, TValue>::Remove(TKey key) {
    return BetterCall<bool>(LoadClass((Il2CppObject *)this).GetMethodInfoByName(OBFUSCATE_BNM("Remove"), 1), this, key);
}

template<typename TKey, typename TValue>
bool NET4x::monoDictionary<TKey, TValue>::TryGet(TKey key, TValue &value) {
    return BetterCall<bool>(LoadClass((Il2CppObject *)this).GetMethodInfoByName(OBFUSCATE_BNM("TryGetValue"), 2), this, key, value);
}

template<typename TKey, typename TValue>
bool NET4x::monoDictionary<TKey, TValue>::ContainsKey(TKey key) {
    return BetterCall<bool>(LoadClass((Il2CppObject *)this).GetMethodInfoByName(OBFUSCATE_BNM("ContainsKey"), 1), this, key);
}

template<typename TKey, typename TValue>
bool NET4x::monoDictionary<TKey, TValue>::ContainsValue(TValue value) {
    return BetterCall<bool>(LoadClass((Il2CppObject *)this).GetMethodInfoByName(OBFUSCATE_BNM("ContainsValue"), 1), this, value);
}

template<typename TKey, typename TValue>
void NET4x::monoDictionary<TKey, TValue>::Insert(TKey key, TValue value) {
    return BetterCall<void>(LoadClass((Il2CppObject *)this).GetMethodInfoByName(OBFUSCATE_BNM("set_Item"), 2), this, key, value);
}

