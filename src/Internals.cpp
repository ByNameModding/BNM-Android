
#include "Internals.hpp"

namespace BNM::Internal {
    States states{};
    void *il2cppLibraryHandle{};

    void *BasicFinder(const char *name, void *userData) { return BNM_dlsym(*(void **)userData, name); }

    Loading::MethodFinder currentFinderMethod = BasicFinder;
    void *currentFinderData = &il2cppLibraryHandle;

    // A list with variables from the il2cpp VM
    _VMData vmData{};

    // il2cpp methods to avoid searching for them every BNM query
    _IL2CppMethods il2cppMethods{};

    std::list<void(*)()> onIl2CppLoaded{};

    std::string_view constructorName = OBFUSCATE_BNM(".ctor");
    BNM::Class customListTemplateClass{};
    std::map<uint32_t, BNM::Class> customListsMap{};
    int32_t finalizerSlot = -1;

    std::vector<BNM::IL2CPP::Il2CppAssembly *> *(*Assembly$$GetAllAssemblies)(){};

    void (*orig_Image$$GetTypes)(const IL2CPP::Il2CppImage *image, bool exportedOnly, std::vector<BNM::IL2CPP::Il2CppClass *> *target){};

    void (*Class$$Init)(IL2CPP::Il2CppClass *klass){};

    void *BNM_il2cpp_init_origin{};
    void (*old_BNM_il2cpp_init)(const char *){};

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
        struct _BNMClassesMap BNMClassesMap{};
    }
#endif
}