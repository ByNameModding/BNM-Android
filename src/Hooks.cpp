#include <Internals.hpp>
#include <BNM/UserSettings/Il2CppMethodNames.hpp>

using namespace BNM;

int Internal::BNM_il2cpp_init(const char *domain_name) {
    if (states.lateInitAllowed) Unhook(BNM_Class$$FromIl2CppType_origin);

    auto ret = old_BNM_il2cpp_init(domain_name);

    Unhook(BNM_il2cpp_init_origin);

    Load();

    return ret;
}

IL2CPP::Il2CppClass *Internal::BNM_Class$$FromIl2CppType(IL2CPP::Il2CppReflectionType *type) {
    auto klass = old_BNM_Class$$FromIl2CppType(type);

    if (states.state) return klass;

    if (!Internal::il2cppMethods.il2cpp_domain_get) {
        Internal::il2cppMethods.il2cpp_domain_get = (decltype(Internal::il2cppMethods.il2cpp_domain_get)) GetIl2CppMethod(BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_domain_get));
        Internal::il2cppMethods.il2cpp_thread_current = (decltype(Internal::il2cppMethods.il2cpp_thread_current)) GetIl2CppMethod(BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_thread_current));
    }

    auto domain = Internal::il2cppMethods.il2cpp_domain_get();
    auto thread = Internal::il2cppMethods.il2cpp_thread_current(domain);

    // Will be true after il2cpp_init
    if (!thread || !thread->internal_thread || (void *) domain->default_context != (void *) thread->internal_thread->current_appcontext) return klass;

    Unhook(BNM_il2cpp_init_origin);
    Unhook(BNM_Class$$FromIl2CppType_origin);

    Load();

    return klass;
}

void Internal::Image$$GetTypes(const IL2CPP::Il2CppImage *image, bool, std::vector<BNM::IL2CPP::Il2CppClass *> *target) {
    // Get non-BNM classes
    if (image->nameToClassHashTable != (decltype(image->nameToClassHashTable))-0x424e4d)
        il2cppMethods.orig_Image$$GetTypes(image, false, target);

#ifdef BNM_CLASSES_MANAGEMENT
    // Get BNM classes
    ClassesManagement::bnmClassesMap.ForEachByImage(image, [&target](IL2CPP::Il2CppClass *BNM_class) -> bool {
        target->push_back(BNM_class);
        return false;
    });
#endif
}

#ifdef BNM_CLASSES_MANAGEMENT

// Hook `FromIl2CppType' to prevent il2cpp from crashing when trying to get a class from a type created by BNM
IL2CPP::Il2CppClass *Internal::ClassesManagement::Class$$FromIl2CppType(IL2CPP::Il2CppType *type) {
    if (!type) return nullptr;

    // Check if type is BNM created
    if (type->num_mods == 31) return (IL2CPP::Il2CppClass *)type->data.dummy;

    return old_Class$$FromIl2CppType(type);
}

// Hook `GetClassOrElementClass' to prevent il2cpp from crashing when unity tries to load a package with a field whose class was created by BNM
IL2CPP::Il2CppClass *Internal::ClassesManagement::Type$$GetClassOrElementClass(IL2CPP::Il2CppType *type) {
    if (!type) return nullptr;

    // Check if type is BNM created
    if (type->num_mods == 31) return (IL2CPP::Il2CppClass *)type->data.dummy;

    return old_Type$$GetClassOrElementClass(type);
}

// Hook `FromName` to prevent il2cpp from crashing when trying to find a class created by BNM
IL2CPP::Il2CppClass *Internal::ClassesManagement::Class$$FromName(IL2CPP::Il2CppImage *image, const char *namespace_, const char *name) {
    if (!image) return nullptr;

    IL2CPP::Il2CppClass *ret = nullptr;

    // Check if image is BNM created
    if (image->nameToClassHashTable != (decltype(image->nameToClassHashTable)) -0x424e4d)
        ret = old_Class$$FromName(image, namespace_, name);

    // If through BNM, we are looking for a class
    if (!ret) bnmClassesMap.ForEachByImage(image, [namespace_, name, &ret](IL2CPP::Il2CppClass *BNM_class) -> bool {
            if (!strcmp(namespace_, BNM_class->namespaze) && !strcmp(name, BNM_class->name)) {
                ret = BNM_class;
                // Found, stop for
                return true;
            }
            return false;
        });

    return ret;
}

// Required because in Unity 2017 and below, they are stored by numbers in images and assemblies
#if UNITY_VER <= 174
IL2CPP::Il2CppImage *Internal::ClassesManagement::new_GetImageFromIndex(IL2CPP::ImageIndex index) {
    // The number is less than 0, which means it is BNM assembly
    if (index < 0) {
        IL2CPP::Il2CppImage *ret = nullptr;

        // Go through all the images and check if the number matches
        bnmClassesMap.ForEach([index, &ret](IL2CPP::Il2CppImage *img, const std::vector<IL2CPP::Il2CppClass *> &classes) -> bool {
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

// All requests are redirected to BNM and processed by it
// In Unity 2017 and below, the names are stored as a number in the metadata, so we can't use them
// But we can check the name by images
IL2CPP::Il2CppAssembly *Internal::ClassesManagement::Assembly$$Load(const char *name) {
    auto &assemblies = *il2cppMethods.Assembly$$GetAllAssemblies();

    for (auto assembly : assemblies) {

        // Get image for assembly
        auto image = new_GetImageFromIndex(assembly->imageIndex);

        // Check if the names match
        if (CompareImageName(image, name)) return assembly;
    }

    return nullptr;
}
#endif

#endif
