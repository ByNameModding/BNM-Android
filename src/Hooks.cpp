#include "Internals.hpp"

using namespace BNM;


void Internal::BNM_il2cpp_init(const char *domain_name) {
    old_BNM_il2cpp_init(domain_name);

    Load();
}

IL2CPP::Il2CppClass *Internal::BNM_il2cpp_class_from_system_type(IL2CPP::Il2CppReflectionType *type) {
    auto klass = old_BNM_il2cpp_class_from_system_type(type);

    Load();

    UNHOOK(BNM_il2cpp_class_from_system_type_origin);
    return klass;
}

void Internal::Image$$GetTypes(const IL2CPP::Il2CppImage *image, bool, std::vector<BNM::IL2CPP::Il2CppClass *> *target) {
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
    std::shared_lock<std::shared_mutex> lock(findClassesMutex);
#endif
    // Получить не BNM-классы
    if (image->nameToClassHashTable != (decltype(image->nameToClassHashTable))-0x424e4d)
        orig_Image$$GetTypes(image, false, target);

#ifdef BNM_CLASSES_MANAGEMENT
    // Получить BNM-классы
    ClassesManagement::BNMClassesMap.forEachByImage(image, [&target](IL2CPP::Il2CppClass *BNM_class) -> bool {
        target->push_back(BNM_class);
        return false;
    });
#endif
}

#ifdef BNM_CLASSES_MANAGEMENT

// Подмена `FromIl2CppType`, чтобы предотвратить вылет il2cpp при попытке получить класс из типа, созданного BNM
IL2CPP::Il2CppClass *Internal::ClassesManagement::Class$$FromIl2CppType(IL2CPP::Il2CppType *type) {
    if (!type) return nullptr;

    // Проверить, создан ли тип BNM
    if (type->num_mods == 31) return (IL2CPP::Il2CppClass *)type->data.dummy;

    return old_Class$$FromIl2CppType(type);
}

// Подмена `GetClassOrElementClass`, чтобы предотвратить вылет il2cpp при попытке unity загрузить пакет с полем класс которого, создан BNM
IL2CPP::Il2CppClass *Internal::ClassesManagement::Type$$GetClassOrElementClass(IL2CPP::Il2CppType *type) {
    if (!type) return nullptr;

    // Проверить, создан ли тип BNM
    if (type->num_mods == 31) return (IL2CPP::Il2CppClass *)type->data.dummy;

    return old_Type$$GetClassOrElementClass(type);
}

// Подмена `FromName`, чтобы предотвратить вылет il2cpp при попытке найти класс, созданный BNM
IL2CPP::Il2CppClass *Internal::ClassesManagement::Class$$FromName(IL2CPP::Il2CppImage *image, const char *namespaze, const char *name) {
    if (!image) return nullptr;

    IL2CPP::Il2CppClass *ret = nullptr;

    // Проверить, создан ли образ BNM
    if (image->nameToClassHashTable != (decltype(image->nameToClassHashTable)) -0x424e4d)
        ret = old_Class$$FromName(image, namespaze, name);

    // Если через BNM, ищем класс
    if (!ret) BNMClassesMap.forEachByImage(image, [namespaze, name, &ret](IL2CPP::Il2CppClass *BNM_class) -> bool {
            if (!strcmp(namespaze, BNM_class->namespaze) && !strcmp(name, BNM_class->name)) {
                ret = BNM_class;
                // Найдено, останавливаем for
                return true;
            }
            return false;
        });

    return ret;
}

// Требуются, потому что в Unity 2017 и ниже в образах и сборках они хранятся по номерам
#if UNITY_VER <= 174
IL2CPP::Il2CppImage *Internal::ClassesManagement::new_GetImageFromIndex(IL2CPP::ImageIndex index) {
    // Номер меньше 0, значит, это сборка BNM
    if (index < 0) {
        IL2CPP::Il2CppImage *ret = nullptr;

        // Перебрать все образы и проверить, совпадает ли номер
        BNMClassesMap.forEach([index, &ret](IL2CPP::Il2CppImage *img, const std::vector<IL2CPP::Il2CppClass *> &classes) -> bool {
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

// Все запросы перенаправляются в BNM и обрабатываются им
// В Unity 2017 и ниже имена хранятся, как номер в метаданных, поэтому мы не можем их использовать
// Но мы можем проверить название по образам
IL2CPP::Il2CppAssembly *Internal::ClassesManagement::Assembly$$Load(const char *name) {
    auto &assemblies = *Assembly$$GetAllAssemblies();

    for (auto assembly : assemblies) {

        // Получить образ для сборки
        auto image = new_GetImageFromIndex(assembly->imageIndex);

        // Проверить, совпадают ли имена
        if (CompareImageName(image, name)) return assembly;
    }

    return nullptr;
}
#endif

#endif
