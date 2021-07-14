#pragma once

template<typename T>
struct Field {
    static bool CheckIfStatic(FieldInfo *fieldInfo) {
        if ((fieldInfo->type->attrs & 0x10) == 0)
            return false;

        if ((fieldInfo->type->attrs & 0x40) != 0)
            return false;

        if (fieldInfo->offset == -1)
            LOGIBNM("thread static fields is not supported!");
        return true;
    }

    FieldInfo *thiz;
    bool init;
    bool thread_static;
    bool is_instance;
    void *instance;
    bool statik;

    Field() {};

    Field(FieldInfo *thiz_, void *_instance = NULL) {
        statik = CheckIfStatic(thiz_);
        if (!statik)
            instance = _instance;
        init = (thiz_ != NULL);
        if (init) {
            thiz = thiz_;
            if (statik && !thiz->parent->static_fields &&
                thiz->parent->static_fields_size) /*** Init static fields ***/
            {
                thiz->parent->static_fields = calloc(1, thiz->parent->static_fields_size);
            }
            is_instance = (thiz->type->attrs & 0x10) == 0;
        }
    }

    DWORD get_offset() {
        return thiz->offset;
    }

    T get() {
        if (!init || thread_static) return T();
        if (statik) {
            return *(T *) ((uint64_t) thiz->parent->static_fields + thiz->offset);
        }
        return *(T *) ((uint64_t) instance + thiz->offset);
    }

    void set(T val) {
        if (!init || thread_static) return;
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
    Il2CppClass *GetClassFromName(const char *_namespace, const char *_name) {
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
                auto klass = il2cpp_class_from_name(img, _namespace, _name);
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
                        if (strcmp(name, _name) == 0 && strcmp(_namespace, namespace_) == 0) {
                            ClassIndex = i;
                            auto thisclass = metadata->GetTypeInfoFromTypeDefinitionIndex(i);
                            LOGIBNM("%s; %s ;%d; %d; %d", name, namespace_,
                                    thisclass->typeDefinition->method_count,
                                    thisclass->method_count, typeDefinition->method_count);
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

    LoadClass(const char *namespaze, const char *clazz, bool _new = false) {
        Il2CppClass *thisclass;
        this->_new = _new;
        int try_count = 0;
        do {
            thisclass = GetClassFromName(namespaze, clazz);
            if (try_count == 2 && !thisclass) {
                LOGIBNM("Class: %s, %s - not founded", namespaze,
                        clazz);
                break;
            } else if (!thisclass) {
                sleep(1);
                try_count++;
            }
        } while (!thisclass);
        klass = thisclass;
    }

    LoadClass(const char *_namespace, const char *_name, const char *dllname, bool _new = false) {
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
                    il2cpp_domain_assembly_open(il2cpp_domain_get(), dllname));
            int try_count = 0;
            do {
                thisclass = il2cpp_class_from_name(dll, _namespace, _name);
                if (try_count == 2 && !thisclass) {
                    LOGIBNM("Class: %s, %s - not founded", _namespace,
                            _name);
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
                                if ((std::string(name).find(std::string(_name)) !=
                                     std::string::npos ||
                                     std::string(name) == std::string(_name)) &&
                                    ((std::string(namespace_).find(std::string(_namespace)) !=
                                      std::string::npos ||
                                      std::string(namespace_) == std::string(_namespace)) ||
                                     namespace_ == "")) {
                                    thisclass = metadata->GetTypeInfoFromTypeDefinitionIndex(i);
                                    ClassIndex = i;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (try_count == 2 && !thisclass) {
                    LOGIBNM("Class: %s, %s - not founded", _namespace,
                            _name);
                    break;
                } else if (!thisclass) {
                    sleep(1);
                    try_count++;
                }
            } while (!thisclass);
        }
        klass = thisclass;
        if (!klass)
            LOGIBNM("Class: %s, %s - not founded", _namespace,
                    _name);
    }

    FieldInfo *GetFieldInfoByName(const char *name) {
        if (_new) {
            Metadata *metadata = MetaDataUtils::CrateMetadata();
            FieldIndex start = klass->typeDefinition->fieldStart;
            FieldIndex end = start + klass->typeDefinition->field_count;
            for (FieldIndex index = start; index < end; ++index) {
                const Il2CppFieldDefinition *fieldDefinition = metadata->getFieldDefinitionByIndex(
                        index);
                auto _name = metadata->GetStringFromIndex(fieldDefinition->nameIndex);
                if (CheckObj(_name) != NULL && ((std::string(_name).find(std::string(name)) !=
                                                 std::string::npos) || strcmp(name, _name) == 0)) {
                    FieldInfo *ret = metadata->GetFieldInfoFromIndexAndClass(index,
                                                                             metadata->GetTypeInfoFromTypeDefinitionIndex(
                                                                                     ClassIndex));
                    return ret;
                }
            }
        } else {
            DO_API(FieldInfo *, il2cpp_class_get_field_from_name,
                   (Il2CppClass * klass, const char *name));
            return il2cpp_class_get_field_from_name(klass, name);
        }
        return nullptr;
    }

    template<typename T>
    Field<T> GetFieldByName(const char *name, void *instance) {
        return Field<T>(GetFieldInfoByName(name), instance);
    }

    DWORD GetFieldOffset(const char *name) {
        return GetOffset(GetFieldInfoByName(name));
    }

    static DWORD GetOffset(FieldInfo *filed) {
        return filed->offset;
    }

    static DWORD GetOffset(MethodInfo *methodInfo) {
        return (DWORD) methodInfo->methodPointer;
    }

    const MethodInfo *GetMethodInfoByName(const char *name, int paramcount) {
        if (_new) {
            Metadata *metadata = MetaDataUtils::CrateMetadata();
            MethodIndex start = klass->typeDefinition->methodStart;
            MethodIndex end = start + klass->method_count;
            for (MethodIndex index = start; index < end; ++index) {
                const Il2CppMethodDefinition *methodDefinition = metadata->GetMethodDefinitionFromIndex(
                        index);
                auto _name = metadata->GetStringFromIndex(methodDefinition->nameIndex);
                if (CheckObj(_name) != NULL && ((std::string(_name).find(std::string(name)) !=
                                                 std::string::npos) || strcmp(name, _name) == 0) &&
                    methodDefinition->parameterCount == paramcount) {
                    return metadata->GetMethodInfoFromIndex(index);
                }
            }
        } else {
            DO_API(const MethodInfo*, il2cpp_class_get_method_from_name,
                   (Il2CppClass * klass, const char *name, int argsCount));
            if (klass)
                return il2cpp_class_get_method_from_name(klass, name, paramcount);
        }
        return nullptr;
    }

    DWORD GetMethodOffsetByName(const char *name, int paramcount) {
        const MethodInfo *res = nullptr;
        int try_count = 0;
        do {
            res = GetMethodInfoByName(name, paramcount);
            if (try_count == 2 && !res) {
                LOGIBNM("Method: %s, %d - not founded", CheckObj(name), paramcount);
                return 0;
            } else if (!res || abs((DWORD) res->methodPointer) <
                               abs((DWORD) GetLibInfo("libil2cpp.so").startAddr)) {
                sleep(1);
                try_count++;
            } else if (abs((DWORD) res->methodPointer) >
                       abs((DWORD) GetLibInfo("libil2cpp.so").startAddr)) {
                break;
            }
        } while (true);
        return (DWORD) res->methodPointer;
    }

    const MethodInfo *GetMethodInfoByName(const char *name, const char **params_names) {
        int paramcount = (sizeof(params_names) / sizeof(*params_names)) + 1;
        if (!klass) return nullptr;
        if (_new) {
            Metadata *metadata = MetaDataUtils::CrateMetadata();
            auto typeDefinition = metadata->getTypeDefinitionByIndex(ClassIndex);
            MethodIndex start = typeDefinition->methodStart;
            MethodIndex end = start + klass->method_count;
            for (MethodIndex index = start; index < end; ++index) {
                const Il2CppMethodDefinition *methodDefinition = metadata->GetMethodDefinitionFromIndex(
                        index);
                auto _name = metadata->GetStringFromIndex(methodDefinition->nameIndex);
                if (CheckObj(_name) != NULL && (strcmp(name, _name) == 0) &&
                    methodDefinition->parameterCount == paramcount) {
                    ParameterIndex parameterStart = methodDefinition->parameterStart;
                    ParameterIndex parameterEnd = parameterStart + methodDefinition->parameterCount;
                    bool ok = true;
                    int paramIndex = 0;
                    for (ParameterIndex i = parameterStart; i < parameterEnd; i++) {
                        auto param = metadata->GetParameterDefinitionFromIndex(i);
                        if (strcmp(metadata->GetStringFromIndex(param->nameIndex),
                                   params_names[paramIndex]) != 0) {
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
                if (strcmp(name, method->name) == 0 && method->parameters_count == paramcount) {
                    bool ok = true;
                    for (int i = 0; i < paramcount; i++) {
                        auto param = method->parameters + i;
                        if (strcmp(param->name, params_names[i]) != 0) {
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

    DWORD GetMethodOffsetByName(const char *name, const char **params_names) {
        int paramcount = (sizeof(params_names) / sizeof(*params_names)) + 1;
        const MethodInfo *res = nullptr;
        int try_count = 0;
        do {
            res = GetMethodInfoByName(name, params_names);
            if (try_count == 3 && !res) {
                LOGIBNM("Method: %s, %d - not founded", CheckObj(name), paramcount);
                return 0;
            } else if (!res || abs((DWORD) res->methodPointer) <
                               abs((DWORD) GetLibInfo("libil2cpp.so").startAddr)) {
                sleep(1);
                try_count++;
            } else if (abs((DWORD) res->methodPointer) >
                       abs((DWORD) GetLibInfo("libil2cpp.so").startAddr)) {
                break;
            }
        } while (true);
        return (DWORD) res->methodPointer;
    }

    static LoadClass
    GetLC_ByClassAndMethodName(const char *_namespace, const char *_name, const char *Metname) {
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
                    if (strstr(_name, name) && strstr(_namespace, namespace_)) {
                        MethodIndex start = typeDefinition->methodStart;
                        MethodIndex end = start + typeDefinition->method_count;
                        for (MethodIndex index = start; index < end; ++index) {
                            const Il2CppMethodDefinition *methodDefinition = metadata->GetMethodDefinitionFromIndex(
                                    index);
                            auto _name = metadata->GetStringFromIndex(methodDefinition->nameIndex);
                            if (!strcmp(Metname, _name)) {
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
    static T Object_Unbox(T obj) {
        void *val = (void *) (((char *) obj) + sizeof(Il2CppObject));
        return *(T *) val;
    }

    const Il2CppType *GetIl2CppType() {
#if IL2CPP_VERSION > 240
        return &klass->byval_arg;
#else
        return klass->byval_arg;
#endif
    }

    void *CreateNewClass(void **args, int args_count) {
        DO_API(Il2CppObject*, il2cpp_object_new, (const Il2CppClass * klass));
        DO_API(Il2CppObject*, il2cpp_runtime_invoke,
               (const MethodInfo * method, void *obj, void **params, Il2CppException **exc));
        auto method = GetMethodInfoByName(".ctor", args_count);
        Il2CppObject *instance = il2cpp_object_new(klass);
        il2cpp_runtime_invoke(method, instance, args, NULL);
        return (void *) instance;
    }
};

void *get_Method(const char *str) {
    DO_API(void*, il2cpp_resolve_icall, (const char *str));
    return il2cpp_resolve_icall(str);
}