#include <BNM/Image.hpp>
#include <BNM/Class.hpp>
#include "Internals.hpp"


BNM::Image::Image(const std::string_view &name) {
    auto &assemblies = *Internal::Assembly$$GetAllAssemblies();

    for (auto assembly : assemblies) {
        auto currentImage = Internal::il2cppMethods.il2cpp_assembly_get_image(assembly);
        if (!Internal::CompareImageName(currentImage, name)) continue;
        _data = currentImage;
        return;
    }

    BNM_LOG_WARN(DBG_BNM_MSG_Image_Constructor_NotFound, name.data());
    _data = nullptr;
}

BNM::Image::Image(const BNM::IL2CPP::Il2CppAssembly *assembly) {
    _data = Internal::il2cppMethods.il2cpp_assembly_get_image(assembly);
}

std::vector<BNM::Class> BNM::Image::GetClasses(bool includeInner) const {
    std::vector<IL2CPP::Il2CppClass *> classes{};

    if (_data->nameToClassHashTable == (decltype(_data->nameToClassHashTable)) -0x424e4d) goto NEW_CLASSES;


    if (Internal::il2cppMethods.il2cpp_image_get_class) {
        size_t typeCount = _data->typeCount;

        for (size_t i = 0; i < typeCount; ++i) {
            auto cls = Internal::il2cppMethods.il2cpp_image_get_class(_data, i);
            if (strcmp(OBFUSCATE_BNM("<Module>"), cls->name) == 0 || !includeInner && cls->declaringType) continue;
            classes.push_back(cls);
        }

    } else {
        Internal::Image$$GetTypes(_data, false, &classes);

        if (includeInner) goto SKIP_INNER_REMOVING;

        for (auto it = classes.begin(); it != classes.end();) {
            if ((*it)->declaringType) {
                classes.erase(it);
                continue;
            }
            ++it;
        }

        SKIP_INNER_REMOVING:
        [[maybe_unused]] uint8_t thisGotoRequiresCpp23Min;
    }


    NEW_CLASSES:

#ifdef BNM_CLASSES_MANAGEMENT
    Internal::ClassesManagement::BNMClassesMap.forEachByImage(_data, [&classes, includeInner](IL2CPP::Il2CppClass *BNMClass) -> bool {
        if (!includeInner && BNMClass->declaringType) return false;

        classes.push_back(BNMClass);
        return false;
    });
#endif

    // Хитрый способ сделать копию с нужным типом без for, т.к. BNM::Class - это BNM::IL2CPP::Il2CppClass *.
    return *(std::vector<BNM::Class> *) &classes;
}

std::vector<BNM::Image> BNM::Image::GetImages() {
    auto &assemblies = *Internal::Assembly$$GetAllAssemblies();

    std::vector<BNM::Image> ret{assemblies.size()};

    for (auto assembly : assemblies) ret.emplace_back(Internal::il2cppMethods.il2cpp_assembly_get_image(assembly));

    return std::move(ret);
}
