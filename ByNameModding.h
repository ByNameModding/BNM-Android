#pragma once

#include "il2cpptypes.h"
#include <dlfcn.h>
#include <Includes/Utils.h>
#include <Includes/Logger.h>
#include <Includes/obfuscate.h>
#include <sstream>
#include <vector>

typedef Il2CppClass *(*class_from_name_t)(const Il2CppImage *assembly, const char *name_space,
                                          const char *name);

typedef MethodInfo *(*class_get_method_from_name_t)(Il2CppClass *klass, const char *name,
                                                    int paramcount);

typedef Il2CppDomain *(*domain_get_t)();

typedef const Il2CppAssembly **(*domain_get_assemblies_t)(const Il2CppDomain *domain, size_t *size);

typedef const Il2CppImage *(*assembly_get_image_t)(const Il2CppAssembly *assembly);

typedef const Il2CppAssembly *(*domain_assembly_open_t)(Il2CppDomain *domain, const char *name);

typedef FieldInfo *(*class_get_field_from_name_t)(Il2CppClass *klass, const char *name);

typedef void (*field_static_get_value_t)(FieldInfo *field, void *value);

typedef void (*field_static_set_value_t)(FieldInfo *field, void *value);

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

template<typename T>
class Field : FieldInfo {
    bool CheckStatic() {
        if (!type->attrs)
            return false;
        if ((type->attrs & 0x10) == 0)
            return false;

        if ((type->attrs & 0x40) != 0)
            return false;

        if (thread_static = offset == -1)
            LOGI(OBFUSCATE("thread static fields is not supported!"));

        return true;
    }

    bool CheckStaticFieldInfo(FieldInfo *fieldInfo) {
        if ((fieldInfo->type->attrs & 0x10) == 0)
            return false;

        if ((fieldInfo->type->attrs & 0x40) != 0)
            return false;

        if (fieldInfo->offset == -1)
            LOGI(OBFUSCATE("thread static fields is not supported!"));

        return true;
    }

public:
    bool init;
    bool thread_static;
    bool is_instance;
    void *instance;

    Field(FieldInfo *thiz, void *_instance = NULL) {
        if (!CheckStaticFieldInfo(thiz))
            instance = _instance;
        init = (thiz != NULL);
        if (init) {
            parent = thiz->parent;
            offset = thiz->offset;
            name = thiz->name;
            token = thiz->token;
            type = thiz->type;
            statik = CheckStatic();
            if (statik) /*** Init static fields ***/
            {
                parent->static_fields = calloc(1, parent->static_fields_size);
            }
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
        return *(T *) ((uint64_t) instance + offset);
    }

    void set(T val) {
        if (!init || thread_static) return;
        if (statik) {
            *(T *) ((uint64_t) parent->static_fields + offset) = val;
            return;
        }
        *(T *) ((uint64_t) instance + offset) = val;
    }

    operator T() {
        return get();
    }

    void operator=(T val) {
        set(val);
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
    void *get_il2cpp() {
        void *mod = 0;
        while (!mod) {
            mod = dlopen(OBFUSCATE("libil2cpp.so"), RTLD_LAZY);
        }
        return mod;
    }

    Il2CppClass *GetClassFromName(const char *name_space, const char *type_name) {
        auto domain_get = (domain_get_t) dlsym(get_il2cpp(), OBFUSCATE("il2cpp_domain_get"));
        auto dom = domain_get();
        if (!dom) {
            return nullptr;
        }
        size_t assemb_count;
        auto domain_get_assemblies = (domain_get_assemblies_t) dlsym(get_il2cpp(), OBFUSCATE(
                "il2cpp_domain_get_assemblies"));
        const Il2CppAssembly **allAssemb = domain_get_assemblies(dom, &assemb_count);
        auto assembly_get_image = (assembly_get_image_t) dlsym(get_il2cpp(), OBFUSCATE(
                "il2cpp_assembly_get_image"));
        auto class_from_name = (class_from_name_t) dlsym(get_il2cpp(),
                                                         OBFUSCATE("il2cpp_class_from_name"));
        for (int i = 0; i < assemb_count; i++) {
            auto assemb = allAssemb[i];
            auto img = assembly_get_image(assemb);
            if (!img) {
                continue;
            }
            auto klass = class_from_name(img, name_space, type_name);
            if (klass) {
                return klass;
            }
        }
        return nullptr;
    }

public:
    Il2CppClass *_class;

    /*** Don't work
    LoadClass(const char *fullpath) {
        std::string dot(OBFUSCATE("."));
        std::vector<std::string> elems;
        std::stringstream ss;
        ss.str(fullpath);
        std::string item;
        while (std::getline(ss, item, '.')) {
            elems.push_back(item);
        }
        const char *namespaze = OBFUSCATE("");
        if (elems.size() == 1) {
            LoadClass(namespaze, elems.back().c_str());
        } else {
            for (int i = 0; i < elems.size() - 1; i++) {
                if (i == 0)
                    namespaze = (elems[i]).c_str();
                else
                    namespaze = (std::string(namespaze) + dot + (elems[i])).c_str();
            }
            LoadClass(namespaze, elems.back().c_str());
        }
    }
    ***/
    LoadClass(Il2CppClass *clazz) {
        _class = clazz;
    }

    LoadClass(const char *namespaze, const char *clazz) {
        Il2CppClass *thisclass;
        int try_count = 0;
        do {
            thisclass = GetClassFromName(namespaze, clazz);
            if (try_count == 2 && !thisclass) {
                LOGI(OBFUSCATE("Method: %s, %s - not founded"), namespaze,
                     clazz);
                return;
            } else if (!thisclass) {
                sleep(1);
                try_count++;
            }
        } while (!thisclass);
        _class = thisclass;
    }

    LoadClass(const char *namespaze, const char *clazz, const char *dllname) {
        auto domain_assembly_open = (domain_assembly_open_t) dlsym(get_il2cpp(), OBFUSCATE(
                "il2cpp_domain_assembly_open"));
        auto assembly_get_image = (assembly_get_image_t) dlsym(get_il2cpp(), OBFUSCATE(
                "il2cpp_assembly_get_image"));
        auto domain_get = (domain_get_t) dlsym(get_il2cpp(), OBFUSCATE("il2cpp_domain_get"));
        auto class_from_name = (class_from_name_t) dlsym(get_il2cpp(),
                                                         OBFUSCATE("il2cpp_class_from_name"));
        auto *dll = assembly_get_image(domain_assembly_open(domain_get(), dllname));
        Il2CppClass *thisclass = nullptr;
        int try_count = 0;
        do {
            thisclass = class_from_name(dll, namespaze, clazz);
            if (try_count == 2 && !thisclass) {
                LOGI(OBFUSCATE("Class: %s, %s - not founded"), namespaze,
                     clazz);
                return;
            } else if (!thisclass) {
                sleep(1);
                try_count++;
            }
        } while (!thisclass);
        _class = thisclass;
    }


    FieldInfo *GetFieldInfoByName(const char *name) {
        auto class_get_field_from_name = (class_get_field_from_name_t) dlsym(get_il2cpp(),
                                                                             OBFUSCATE(
                                                                                     "il2cpp_class_get_field_from_name"));
        return class_get_field_from_name(_class, name);
    }

    template<typename T>
    Field<T> GetFieldByName(const char *name) {
        return Field<T>(GetFieldInfoByName(name), this);
    }

    DWORD GetFieldOffset(const char *name) {
        return GetFieldInfoByName(name)->offset;
    }

    DWORD GetFieldOffset(FieldInfo *filed) {
        return filed->offset;
    }

    MethodInfo *GetMethodInfoByName(const char *name, int paramcount) {
        auto class_get_method_from_name = (class_get_method_from_name_t) dlsym(get_il2cpp(),
                                                                               OBFUSCATE(
                                                                                       "il2cpp_class_get_method_from_name"));
        if (_class)
            return class_get_method_from_name(_class, name, paramcount);
        else
            return nullptr;
    }

    DWORD GetMethodOffsetByName(const char *name, int paramcount) {
        MethodInfo *res = nullptr;
        int try_count = 0;
        do {
            res = GetMethodInfoByName(name, paramcount);
            if (try_count == 2 && !res) {
                if (name)
                    LOGI(OBFUSCATE("Method: %s, %s - not founded"), name,
                         to_string(paramcount).c_str());
                else
                    LOGI(OBFUSCATE("Method: , %s - not founded"), to_string(paramcount).c_str());
                return 0;
            } else if (!res) {
                sleep(1);
                try_count++;
            }
        } while (!res);
        return (DWORD) res->methodPointer;
    }

    static void *Object_Unbox(Il2CppObject *obj) {
        void *val = (void *) (((char *) obj) + sizeof(Il2CppObject));
        return val;
    }

    static Il2CppObject *Object_Box_My(void *obj) {
        Il2CppObject *val = (Il2CppObject *) (((char *) obj) - sizeof(Il2CppObject));
        return val;
    }
};

void *get_Method(const char *str) {
    void *(*il2cpp_resolve_icall_0)(const char *str) = nullptr;
    void *h = nullptr;
    while (!h) {
        h = dlopen(OBFUSCATE("libil2cpp.so"), 4);
    }
    do {
        il2cpp_resolve_icall_0 = (void *(*)(const char *)) dlsym(h,
                                                                 OBFUSCATE("il2cpp_resolve_icall"));
    } while (!il2cpp_resolve_icall_0);
    return il2cpp_resolve_icall_0(str);
}
/*** Don't work
DWORD get_Method_by_path(const char *path, int paramcount) {
    std::vector<std::string> elems = split(path, '.');
    const char *class_path;
    std::string dot(OBFUSCATE("."));
    for (int i = 0; i < elems.size() - 1; i++) {
        if (i == 0)
            class_path = (elems[i]).c_str();
        else
            class_path = (std::string(class_path) + dot + elems[i]).c_str();
    }
    LoadClass *tmp = new LoadClass(class_path);
    return tmp->GetMethodOffsetByName(elems.back().c_str(), paramcount);
}
***/
#define InitResolveFunc(x, y) *reinterpret_cast<void **>(&x) = get_Method(y)
#define InitFunc(x, y) if (y != 0) *reinterpret_cast<void **>(&x) = (void *)(y)
#define FieldBN(myfield, type, inst, nameSpacec, clazzz, fieldName, key) Field<type> myfield = LoadClass(OBFUSCATE_KEY(nameSpacec, key), OBFUSCATE_KEY(clazzz, key)).GetFieldByName<type>(OBFUSCATE_KEY(fieldName, key)); myfield.instance = inst
// #define FieldBN_Full(myfield, type, inst, fullpath, fieldName, key) Field<type> myfield = (new LoadClass(OBFUSCATE_KEY(fullpath, key)))->GetFieldByName<type>(OBFUSCATE_KEY(fieldName, key)); myfield.clazz = inst
