#include <Internals.hpp>

namespace BNM::Internal {
    States states{};
    void *il2cppLibraryHandle{};

    void *BasicFinder(const char *name, void *userData) { return BNM_dlsym(*(void **)userData, name); }

    Loading::MethodFinder currentFinderMethod = BasicFinder;
    void *currentFinderData = &il2cppLibraryHandle;

    // A list with variables from the il2cpp VM
    VMData vmData{};

    // il2cpp methods to avoid searching for them every BNM query
    Il2CppMethods il2cppMethods{};

    std::list<void(*)()> onIl2CppLoaded{};

    std::string_view constructorName = BNM_OBFUSCATE(".ctor");
    BNM::Class customListTemplateClass{};
    std::map<uint32_t, BNM::Class> customListsMap{};
    int32_t finalizerSlot = -1;

    std::vector<BNM::IL2CPP::Il2CppAssembly *> *(*Assembly$$GetAllAssemblies)(){};

    void (*orig_Image$$GetTypes)(const IL2CPP::Il2CppImage *image, bool exportedOnly, std::vector<BNM::IL2CPP::Il2CppClass *> *target){};

    void (*Class$$Init)(IL2CPP::Il2CppClass *klass){};

    void *BNM_il2cpp_init_origin{};
    int (*old_BNM_il2cpp_init)(const char *){};

    void *BNM_il2cpp_class_from_system_type_origin{};
    IL2CPP::Il2CppClass *(*old_BNM_il2cpp_class_from_system_type)(IL2CPP::Il2CppReflectionType*){};

#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
    std::shared_mutex loadingMutex{};
#endif

#ifdef BNM_CLASSES_MANAGEMENT
    namespace ClassesManagement {
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
        std::shared_mutex classesFindAccessMutex{};
#endif
        // A list with all the classes that BNM should create/modify
        std::vector<MANAGEMENT_STRUCTURES::CustomClass *> *classesManagementVector = nullptr;

        IL2CPP::Il2CppClass *(*old_Class$$FromIl2CppType)(IL2CPP::Il2CppType *type){};

        IL2CPP::Il2CppClass *(*old_Type$$GetClassOrElementClass)(IL2CPP::Il2CppType *type){};

        IL2CPP::Il2CppClass *(*old_Class$$FromName)(IL2CPP::Il2CppImage *image, const char *ns, const char *name){};

#if UNITY_VER <= 174
        IL2CPP::Il2CppImage *(*old_GetImageFromIndex)(IL2CPP::ImageIndex index){};
#endif
        BNMClassesMap bnmClassesMap{};
    }
#endif
}

using namespace BNM;

IL2CPP::Il2CppImage *Internal::TryGetImage(const std::string_view &_name) {
    auto &assemblies = *Internal::Assembly$$GetAllAssemblies();

    for (auto assembly : assemblies) {
        auto currentImage = Internal::il2cppMethods.il2cpp_assembly_get_image(assembly);
        if (!Internal::CompareImageName(currentImage, _name)) continue;
        return currentImage;
    }

    return {};
}

IL2CPP::Il2CppClass *Internal::TryGetClassInImage(const IL2CPP::Il2CppImage *image, const std::string_view &_namespace, const std::string_view &_name) {
    if (!image) return nullptr;

#ifdef BNM_CLASSES_MANAGEMENT
    // Get BNM classes
    if (image->nameToClassHashTable == (decltype(image->nameToClassHashTable))-0x424e4d) goto NEW_CLASSES;
#endif

    if (Internal::il2cppMethods.il2cpp_image_get_class) {
        size_t typeCount = image->typeCount;

        for (size_t i = 0; i < typeCount; ++i) {
            auto cls = il2cppMethods.il2cpp_image_get_class(image, i);
            if (strcmp(BNM_OBFUSCATE("<Module>"), cls->name) == 0 || cls->declaringType) continue;
            if (_namespace == cls->namespaze && _name == cls->name) return cls;
        }
    } else {
        std::vector<IL2CPP::Il2CppClass *> classes{};
        Internal::Image$$GetTypes(image, false, &classes);

        for (auto cls : classes) {
            if (!cls) continue;
            Internal::Class$$Init(cls);
            if (cls->declaringType) continue;
            if (cls->namespaze == _namespace && cls->name == _name) return cls;
        }
    }

#ifdef BNM_CLASSES_MANAGEMENT
    NEW_CLASSES:
    IL2CPP::Il2CppClass *result = nullptr;
    ClassesManagement::bnmClassesMap.ForEachByImage(image, [&_namespace, &_name, &result](IL2CPP::Il2CppClass *BNM_class) -> bool {
        if (_namespace != BNM_class->namespaze || _name != BNM_class->name) return false;

        result = BNM_class;
        return true;
    });
    return result;
#endif

    return nullptr;
}
Class Internal::TryMakeGenericClass(Class genericType, const std::vector<CompileTimeClass> &templateTypes) {
    if (!vmData.RuntimeType$$MakeGenericType.Initialized()) return {};
    auto monoType = genericType.GetMonoType();
    auto monoGenericsList = Structures::Mono::Array<MonoType *>::Create(templateTypes.size(), true);
    for (IL2CPP::il2cpp_array_size_t i = 0; i < (IL2CPP::il2cpp_array_size_t) templateTypes.size(); ++i)
        (*monoGenericsList)[i] = templateTypes[i].ToClass().GetMonoType();

    Class typedGenericType = vmData.RuntimeType$$MakeGenericType(monoType, monoGenericsList);

    monoGenericsList->Destroy();

    return typedGenericType;
}

MethodBase Internal::TryMakeGenericMethod(const MethodBase &genericMethod, const std::vector<CompileTimeClass> &templateTypes) {
    if (!vmData.RuntimeMethodInfo$$MakeGenericMethod_impl.Initialized() || !genericMethod.GetInfo()->is_generic) return {};
    IL2CPP::Il2CppReflectionMethod reflectionMethod;
    reflectionMethod.method = genericMethod.GetInfo();
    auto monoGenericsList = Structures::Mono::Array<MonoType *>::Create(templateTypes.size(), true);
    for (IL2CPP::il2cpp_array_size_t i = 0; i < (IL2CPP::il2cpp_array_size_t) templateTypes.size(); ++i) (*monoGenericsList)[i] = templateTypes[i].ToClass().GetMonoType();

    MethodBase typedGenericMethod = vmData.RuntimeMethodInfo$$MakeGenericMethod_impl[(void *)&reflectionMethod](monoGenericsList)->method;

    monoGenericsList->Destroy();

    return typedGenericMethod;
}

Class Internal::GetPointer(Class target) {
    if (!vmData.RuntimeType$$MakePointerType.Initialized()) return {};
    return vmData.RuntimeType$$MakePointerType(target.GetMonoType());
}

Class Internal::GetReference(Class target) {
    if (!vmData.RuntimeType$$make_byref_type.Initialized()) return {};
    return vmData.RuntimeType$$make_byref_type[(void *)target.GetMonoType()]();
}