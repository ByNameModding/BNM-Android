#pragma once

#include "il2cpptypes.h"
#include "ByNameModdingUtils.h"

template<typename T>
class Field : FieldInfo {
    static bool CheckStaticFieldInfo(FieldInfo *fieldInfo) {
        if ((fieldInfo->type->attrs & 0x10) == 0)
            return false;

        if ((fieldInfo->type->attrs & 0x40) != 0)
            return false;

        if (fieldInfo->offset == -1)
            LOGIBNM("thread static fields is not supported!");
        return true;
    }

public:
    bool init;
    bool thread_static;
    bool is_instance;
    void *clazz;

    Field() {};

    Field(FieldInfo *thiz, void *_instance = NULL) {
        bool static_;
        if (!(static_ = CheckStaticFieldInfo(thiz)))
            clazz = _instance;
        init = (thiz != NULL);
        if (init) {
            parent = thiz->parent;
            offset = thiz->offset;
            name = thiz->name;
            token = thiz->token;
            type = thiz->type;
            statik = static_;
            is_instance = (type->attrs & 0x10) == 0;
        }
    }

    DWORD get_offset() {
        return offset;
    }

    T get() {
        if (!init || thread_static) return T();
        if (statik) {
            return *(T *) ((uint64_t) parent->static_fields + offset);
        }
        return *(T *) ((uint64_t) clazz + offset);
    }

    Field set(T val) {
        if (!init || thread_static) return *this;
        if (statik) {
            *(T *) ((uint64_t) parent->static_fields + offset) = val;
            return *this;
        }
        *(T *) ((uint64_t) clazz + offset) = val;
        return *this;
    }

    operator T() {
        return get();
    }

    Field operator=(T val) {
        return set(val);
    }

    bool operator==(Field other) {
        if (!init) return false;
        return (type == other.type && parent == other.parent &&
                offset == other.offset && name == other.name && token == other.token);
    }

    T operator()() {
        return get();
    }

    bool statik;
};

class LoadClass {
    Il2CppClass *GetClassFromName(const char *_namespace, const char *_name) {
        auto class_from_name = (class_from_name_t) dlsym(get_il2cpp(),
                                                         "il2cpp_class_from_name");
        auto domain_get = (domain_get_t) dlsym(get_il2cpp(), "il2cpp_domain_get");
        auto dom = domain_get();
        if (!dom) {
            return nullptr;
        }
        size_t assemb_count;
        auto domain_get_assemblies = (domain_get_assemblies_t) dlsym(get_il2cpp(),
                                                                     "il2cpp_domain_get_assemblies");
        const Il2CppAssembly **allAssemb = domain_get_assemblies(dom, &assemb_count);
        auto assembly_get_image = (assembly_get_image_t) dlsym(get_il2cpp(),
                                                               "il2cpp_assembly_get_image");
        if (!_new) {
            for (int i = 0; i < assemb_count; i++) {
                auto assemb = allAssemb[i];
                auto img = assembly_get_image(assemb);
                if (!img) {
                    continue;
                }
                auto klass = class_from_name(img, _namespace, _name);
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
                        if ((std::string(name).find(std::string(_name)) !=
                             std::string::npos ||
                             std::string(name) == std::string(_name)) &&
                            ((std::string(namespace_).find(std::string(_namespace)) !=
                              std::string::npos ||
                              std::string(namespace_) == std::string(_namespace)) ||
                             namespace_ == "")) {
                            ClassIndex = i;
                            return metadata->GetTypeInfoFromTypeDefinitionIndex(i);
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
            auto domain_assembly_open = (domain_assembly_open_t) dlsym(get_il2cpp(),
                                                                       "il2cpp_domain_assembly_open");
            auto assembly_get_image = (assembly_get_image_t) dlsym(get_il2cpp(),
                                                                   "il2cpp_assembly_get_image");
            auto domain_get = (domain_get_t) dlsym(get_il2cpp(), "il2cpp_domain_get");
            auto class_from_name = (class_from_name_t) dlsym(get_il2cpp(),
                                                             "il2cpp_class_from_name");
            auto *dll = assembly_get_image(domain_assembly_open(domain_get(), dllname));
            int try_count = 0;
            do {
                thisclass = class_from_name(dll, _namespace, _name);
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
            auto class_get_field_from_name = (class_get_field_from_name_t) dlsym(get_il2cpp(),

                                                                                 "il2cpp_class_get_field_from_name");
            return class_get_field_from_name(klass, name);
        }
        return nullptr;
    }

    template<typename T>
    Field<T> GetFieldByName(const char *name, void *inst = NULL) {
        return Field<T>(GetFieldInfoByName(name), inst != NULL ? inst : (void *) klass);
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

    MethodInfo *GetMethodInfoByName(const char *name, int paramcount) {
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
            auto class_get_method_from_name = (class_get_method_from_name_t) dlsym(get_il2cpp(),

                                                                                   "il2cpp_class_get_method_from_name");
            if (klass)
                return class_get_method_from_name(klass, name, paramcount);
        }
        return nullptr;
    }

    DWORD GetMethodOffsetByName(const char *name, int paramcount) {
        MethodInfo *res = nullptr;
        int try_count = 0;
        do {
            res = GetMethodInfoByName(name, paramcount);
            if (try_count == 5 && !res) {
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

    template<typename T>
    static T Object_Unbox(T obj) {
        void *val = (void *) (((char *) obj) + sizeof(Il2CppObject));
        return val;
    }
	
	const Il2CppType *GetIl2CppType(){
        auto class_get_type = (class_get_type_t) dlsym(get_il2cpp(),
                                                                OBFUSCATE(
                                                                        "il2cpp_class_get_type"));
        return class_get_type(klass);
    }
	
    void *CreateNewClass(void **args, int args_count) {
        auto object_new = (object_new_t) dlsym(get_il2cpp(), "il2cpp_object_new");
        auto runtime_invoke = (runtime_invoke_t) dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        auto method = GetMethodInfoByName(".ctor", args_count);
        Il2CppObject* instance = object_new(klass);
        runtime_invoke(method, instance, args, NULL);
        return (void *) instance;
    }
};

void *get_Method(const char *str) {
    void *(*il2cpp_resolve_icall_0)(const char *str) = nullptr;
    do {
        il2cpp_resolve_icall_0 = (void *(*)(const char *)) dlsym(get_il2cpp(),
                                                                 "il2cpp_resolve_icall");
    } while (!il2cpp_resolve_icall_0);
    return il2cpp_resolve_icall_0(str);
}
