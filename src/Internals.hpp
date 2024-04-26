#pragma once

#include <list>
#include <vector>
#include <map>

#include <BNM/UserSettings/GlobalSettings.hpp>
#include <BNM/Utils.hpp>
#include <BNM/BasicMonoStructures.hpp>
#include <BNM/Method.hpp>
#include <BNM/Class.hpp>
#include <BNM/ClassesManagement.hpp>
#include <BNM/Loading.hpp>


namespace BNM::Internal {

    extern bool state;
    extern bool lateInitAllowed;
    extern void *il2cppLibraryHandle;
    extern Loading::MethodFinder usersFinderMethod;
    extern void *usersFinderMethodData;

#pragma pack(push, 1)
    // Список с переменными из виртуальной машины il2cpp
    struct _VMData {
        BNM::Class Object{}, UnityEngine$$Object{};
        BNM::Method<IL2CPP::Il2CppReflectionType *> Type$$GetType{};
        BNM::Method<void *> Interlocked$$CompareExchange{};
        BNM::Method<BNM::MonoType *> RuntimeType$$MakeGenericType{};
        BNM::Method<BNM::MonoType *> RuntimeType$$MakePointerType{};
        BNM::Method<BNM::MonoType *> RuntimeType$$make_byref_type{};
        BNM::Method<BNM::IL2CPP::Il2CppReflectionMethod *> RuntimeMethodInfo$$MakeGenericMethod_impl{};
        BNM::Structures::Mono::String **String$$Empty{};
    } extern vmData;

    // il2cpp методы, чтобы не искать их каждый запрос BNM
    struct _IL2CppMethods {
        BNM::IL2CPP::Il2CppImage *(*il2cpp_get_corlib)(){};
        BNM::IL2CPP::Il2CppClass *(*il2cpp_class_from_name)(const BNM::IL2CPP::Il2CppImage *, const char *, const char *){};
        BNM::IL2CPP::Il2CppImage *(*il2cpp_assembly_get_image)(const BNM::IL2CPP::Il2CppAssembly *){};
        BNM::IL2CPP::Il2CppClass *(*il2cpp_image_get_class)(const BNM::IL2CPP::Il2CppImage *, unsigned int){};
        const char *(*il2cpp_method_get_param_name)(const BNM::IL2CPP::MethodInfo *, uint32_t){};
        BNM::IL2CPP::Il2CppClass *(*il2cpp_class_from_il2cpp_type)(const BNM::IL2CPP::Il2CppType *){};
        BNM::IL2CPP::Il2CppClass *(*il2cpp_array_class_get)(const BNM::IL2CPP::Il2CppClass *, uint32_t){};
        BNM::IL2CPP::Il2CppObject *(*il2cpp_type_get_object)(const BNM::IL2CPP::Il2CppType *){};
        BNM::IL2CPP::Il2CppObject *(*il2cpp_object_new)(const BNM::IL2CPP::Il2CppClass *){};
        BNM::IL2CPP::Il2CppObject *(*il2cpp_value_box)(const BNM::IL2CPP::Il2CppClass *, void *){};
        BNM::IL2CPP::Il2CppArray *(*il2cpp_array_new)(const BNM::IL2CPP::Il2CppClass *, BNM::IL2CPP::il2cpp_array_size_t){};
        void (*il2cpp_field_static_get_value)(const BNM::IL2CPP::FieldInfo *, void *){};
        void (*il2cpp_field_static_set_value)(const BNM::IL2CPP::FieldInfo *, void *){};
        BNM::Structures::Mono::String *(*il2cpp_string_new)(const char *){};
        void *(*il2cpp_resolve_icall)(const char *){};
    } extern il2cppMethods;

#pragma pack(pop)

    extern std::list<void(*)()> onIl2CppLoaded;

    extern std::string_view constructorName;
    extern BNM::Class customListTemplateClass;
    extern std::map<uint32_t, BNM::Class> customListsMap;
    extern int32_t finalizerSlot;

    extern std::vector<BNM::IL2CPP::Il2CppAssembly *> *(*Assembly$$GetAllAssemblies)();

    extern void (*orig_Image$$GetTypes)(const IL2CPP::Il2CppImage *image, bool exportedOnly, std::vector<BNM::IL2CPP::Il2CppClass *> *target);
    void Image$$GetTypes(const IL2CPP::Il2CppImage *image, bool exportedOnly, std::vector<BNM::IL2CPP::Il2CppClass *> *target);

    extern void (*Class$$Init)(IL2CPP::Il2CppClass *klass);

    void Load();
    void LateInit(void *il2cpp_class_from_il2cpp_type_addr);

    void *GetIl2CppMethod(const char *methodName);

    extern void (*old_BNM_il2cpp_init)(const char *);
    void BNM_il2cpp_init(const char *domain_name);

    extern void *BNM_il2cpp_class_from_system_type_origin;
    extern IL2CPP::Il2CppClass *(*old_BNM_il2cpp_class_from_system_type)(IL2CPP::Il2CppReflectionType*);
    IL2CPP::Il2CppClass *BNM_il2cpp_class_from_system_type(IL2CPP::Il2CppReflectionType *type);

    void SetupBNM();

#ifdef BNM_COROUTINE
    void SetupCoroutine();
    void LoadCoroutine();
#endif

    IL2CPP::Il2CppClass *TryGetClassInImage(const IL2CPP::Il2CppImage *image, const std::string_view &_namespace, const std::string_view &_name);

    Class TryMakeGenericClass(Class genericType, const std::vector<CompileTimeClass> &templateTypes);
    MethodBase TryMakeGenericMethod(const MethodBase &genericMethod, const std::vector<CompileTimeClass> &templateTypes);
    Class GetPointer(Class target);
    Class GetReference(Class target);
    template <class CompareMethod>
    IL2CPP::MethodInfo *IterateMethods(Class target, CompareMethod compare) {
        auto curClass = target._data;
        do {
            for (uint16_t i = 0; i < curClass->method_count; ++i) {
                auto method = curClass->methods[i];
                if (compare((IL2CPP::MethodInfo *)method)) return (IL2CPP::MethodInfo *) method;
            }
            curClass = curClass->parent;
        } while (curClass);
        return nullptr;
    }

#ifdef BNM_CLASSES_MANAGEMENT
    namespace ClassesManagement {
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
        extern std::shared_mutex classesFindAccessMutex, customClassesMutex;
#endif
        // Список со всеми классами, которые BNM должен создать/изменить
        extern std::vector<MANAGEMENT_STRUCTURES::CustomClass *> *classesManagementVector;

        extern IL2CPP::Il2CppClass *(*old_Class$$FromIl2CppType)(IL2CPP::Il2CppType *type);
        IL2CPP::Il2CppClass *Class$$FromIl2CppType(IL2CPP::Il2CppType *type);

        extern IL2CPP::Il2CppClass *(*old_Type$$GetClassOrElementClass)(IL2CPP::Il2CppType *type);
        IL2CPP::Il2CppClass *Type$$GetClassOrElementClass(IL2CPP::Il2CppType *type);

        extern IL2CPP::Il2CppClass *(*old_Class$$FromName)(IL2CPP::Il2CppImage *image, const char *ns, const char *name);
        IL2CPP::Il2CppClass *Class$$FromName(IL2CPP::Il2CppImage *image, const char *namespaze, const char *name);

#if UNITY_VER <= 174
        // Требуются, потому что в Unity 2017 и ниже в образах (image) и сборках (assembly) они хранятся по номерам

        extern IL2CPP::Il2CppImage *(*old_GetImageFromIndex)(IL2CPP::ImageIndex index);
        IL2CPP::Il2CppImage *new_GetImageFromIndex(IL2CPP::ImageIndex index);

        IL2CPP::Il2CppAssembly *Assembly$$Load(const char *name);
#endif

        void ProcessCustomClasses();

#pragma pack(push, 1)

        extern struct _BNMClassesMap { // Структура для быстрого поиска классов по их образам
            // Добавить класс к образу
            inline void addClass(const IL2CPP::Il2CppImage *image, IL2CPP::Il2CppClass *cls) {
                return addClass((BNM_PTR)image, cls);
            }

            inline void addClass(BNM_PTR image, IL2CPP::Il2CppClass *cls) {
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
                std::shared_lock lock(classesFindAccessMutex);
#endif
                map[image].emplace_back(cls);
            }

            // Перебрать все классы, добавленные к образу
            template <class IterateMethod>
            inline void forEachByImage(const IL2CPP::Il2CppImage *image, IterateMethod func) {
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
                std::shared_lock lock(classesFindAccessMutex);
#endif
                return forEachByImage((BNM_PTR)image, func);
            }

            template <class IterateMethod>
            inline void forEachByImage(BNM_PTR image, IterateMethod func) {
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
                std::shared_lock lock(classesFindAccessMutex);
#endif
                for (auto item : map[image]) if (func(item)) break;
            }

            // Перебрать все образы с их классами
            template <class IterateMethod>
            inline void forEach(IterateMethod func) {
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
                std::shared_lock lock(classesFindAccessMutex);
#endif
                for (auto &[img, classes] : map)
                    if (func((IL2CPP::Il2CppImage *)img, classes))
                        break;
            }
        private:
            std::map<BNM_PTR, std::vector<IL2CPP::Il2CppClass *>> map;
        } BNMClassesMap;
    }

#pragma pack(pop)

#endif

    inline bool CompareImageName(IL2CPP::Il2CppImage *image, const std::string_view &name) {
        bool value = image->name == name;
#if UNITY_VER >= 171
        value = value || image->nameNoExt == name;
#else
        if (!value) {
            auto nameWithoutDll = std::string_view(image->name);
            nameWithoutDll.remove_suffix(4);
            value = nameWithoutDll == name;
        }
#endif
        return value;
    }
}