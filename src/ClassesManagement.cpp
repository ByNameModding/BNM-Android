#include <BNM/ClassesManagement.hpp>

#ifdef BNM_CLASSES_MANAGEMENT

#include "Internals.hpp"

using namespace BNM;

#define BNM_I2C_NEW(type) (IL2CPP::type *) malloc(sizeof(IL2CPP::type))

IL2CPP::Il2CppImage *MakeImage(std::string_view imageName) {
    auto newImg = BNM_I2C_NEW(Il2CppImage);

    if (imageName.ends_with(OBFUSCATE_BNM(".dll"))) imageName.remove_suffix(4);

    auto nameLen = imageName.size();
#if UNITY_VER >= 171
    newImg->nameNoExt = (char *) malloc(nameLen + 1);
    memcpy((void *)newImg->nameNoExt, (void *)imageName.data(), nameLen);
    ((char *)newImg->nameNoExt)[nameLen] = 0;
#endif
    newImg->name = (char *) malloc(nameLen + 5);
    memcpy((void *)newImg->name, (void *)imageName.data(), nameLen);
    auto nameEnd = ((char *)(newImg->name + nameLen));
    nameEnd[0] = '.'; nameEnd[1] = 'd'; nameEnd[2] = 'l'; nameEnd[3] = 'l'; nameEnd[4] = 0;

#if UNITY_VER > 182
    newImg->assembly = nullptr;
    newImg->customAttributeCount = 0;
#if UNITY_VER < 201
    newImg->customAttributeStart = -1;
#endif
#endif

#if UNITY_VER > 201
    // Создать пустой Il2CppImageDefinition
    auto handle = (IL2CPP::Il2CppImageDefinition *) malloc(sizeof(IL2CPP::Il2CppImageDefinition));
    memset(handle, 0, sizeof(IL2CPP::Il2CppImageDefinition));
    handle->typeStart = -1;
    handle->exportedTypeStart = -1;
    handle->typeCount = 0;
    handle->exportedTypeCount = 0;
    handle->nameIndex = -1;
    handle->assemblyIndex = -1;
    handle->customAttributeCount = 0;
    handle->customAttributeStart = -1;
    newImg->metadataHandle = (decltype(newImg->metadataHandle))handle;
#else
    // -1 для отключения анализа от il2cpp
    newImg->typeStart = -1;
#if UNITY_VER >= 171
    newImg->exportedTypeStart = -1;
#endif
#endif
    // Инициализировать переменные
    newImg->typeCount = 0;
#if UNITY_VER >= 171
    newImg->exportedTypeCount = 0;
#endif
    newImg->token = 1;

    // Создать новую сборку для образа
    auto newAsm = BNM_I2C_NEW(Il2CppAssembly);
    auto &aname = newAsm->aname;
#if UNITY_VER > 174
    // Установить образ и сборку
    newAsm->image = newImg;
    newAsm->image->assembly = newAsm;
    aname.name = newImg->name;
    aname.culture = nullptr;
    aname.public_key = nullptr;
#else
    // Отрицательное значение для отключения анализа от il2cpp
    static int newAsmCount = 1;
    newImg->assemblyIndex = newAsm->imageIndex = -newAsmCount;
    newAsmCount++;
    aname.publicKeyIndex = aname.hashValueIndex = aname.nameIndex = 0;
    aname.cultureIndex = -1;
#endif
    // Инициализировать эти переменные на всякий случай
    aname.revision = aname.build = aname.minor = aname.major = 0;
#if UNITY_VER > 174
    aname.public_key_token[0] = aname.hash_len = 0;
#else
    aname.publicKeyToken[0] = aname.hash_len = 0;
#endif

    aname.hash_alg = aname.flags = 0;
    newAsm->referencedAssemblyStart = -1;
    newAsm->referencedAssemblyCount = 0;

    // Используя это, BNM может проверить, создана ли им эта сборка
    newImg->nameToClassHashTable = (decltype(newImg->nameToClassHashTable)) -0x424e4d;

    // Добавить сборку в список
    Internal::Assembly$$GetAllAssemblies()->push_back(newAsm);

    BNM_LOG_INFO(DBG_BNM_MSG_ClassesManagement_MakeImage_Added_Image, imageName);

    return newImg;
}

IL2CPP::MethodInfo *CreateMethod(MANAGEMENT_STRUCTURES::CustomMethod *method);
IL2CPP::MethodInfo *ProcessCustomMethod(MANAGEMENT_STRUCTURES::CustomMethod *method, Class target, bool *hooked = nullptr) {
    if (!target) return CreateMethod(method);

    auto parameters = (uint8_t) method->_parameterTypes.size();

    auto originalMethod = Internal::IterateMethods(target, [method, parameters](IL2CPP::MethodInfo *klassMethod) {
        if (method->_name != klassMethod->name || klassMethod->parameters_count != parameters) return false;
        for (uint8_t i = 0; i < parameters; ++i) {
#if UNITY_VER < 212
            auto param = (klassMethod->parameters + i)->parameter_type;
#else
            auto param = klassMethod->parameters[i];
#endif

            if (Class(param).GetClass() != method->_parameterTypes[i].ToIl2CppClass()) return false;
        }
        return true;
    });

    if (!originalMethod) return CreateMethod(method);

#ifndef BNM_AUTO_HOOK_DISABLE_VIRTUAL_HOOK
    IL2CPP::VirtualInvokeData *vTable{};
    for (uint16_t i = 0; i < target._data->vtable_count; ++i) {
        auto &it = target._data->vtable[i];

        if (it.method != originalMethod) continue;

        vTable = &it;
        break;
    }

    if (vTable) {
        method->_originalAddress = (void *) vTable->methodPtr;
        vTable->methodPtr = (void(*)()) method->_address;
    } else
#endif
        ::HOOK((void *)originalMethod->methodPointer, method->_address, method->_originalAddress);

    if (hooked) *hooked = true;

    return originalMethod;
}


IL2CPP::MethodInfo *CreateMethod(MANAGEMENT_STRUCTURES::CustomMethod *method) {
    auto *myInfo = BNM_I2C_NEW(MethodInfo);
    myInfo->methodPointer = (decltype(myInfo->methodPointer)) method->_address;
    myInfo->invoker_method = (decltype(myInfo->invoker_method)) method->_invoker;
    myInfo->parameters_count = method->_parameterTypes.size();

    auto name = (char *) malloc(method->_name.size() + 1);
    memcpy((void *)name, method->_name.data(), method->_name.size());
    name[method->_name.size()] = 0;
    myInfo->name = name;

    // Установить флаги метода
    myInfo->flags = 0x0006 | 0x0080; // PUBLIC | HIDE_BY_SIG
    if (method->_isStatic) myInfo->flags |= 0x0010; // |= STATIC
    if (method->_name == Internal::constructorName) myInfo->flags |= 0x0800 | 0x1000; // |= SPECIAL_NAME | RT_SPECIAL_NAME

    // BNM не поддерживает создание generic-методов
    myInfo->is_generic = false;

    // Установить возвращаемый тип
    auto methodType = method->_returnType.ToClass();
    if (!methodType) methodType = Internal::vmData.Object;
    myInfo->return_type = methodType.GetIl2CppType();

    // Создать аргументы
    auto argsCount = myInfo->parameters_count;
    if (argsCount) {
        auto &types = method->_parameterTypes;
#if UNITY_VER < 212
        myInfo->parameters = (IL2CPP::ParameterInfo *) malloc(argsCount * sizeof(IL2CPP::ParameterInfo));

        auto parameter = (IL2CPP::ParameterInfo *)myInfo->parameters;
        for (uint8_t p = 0; p < argsCount; ++p) {
            parameter->name = nullptr; // Имя параметра не интересует движок вообще никак
            parameter->position = p;

            // Установить тип в любом случае
            auto type = p < types.size() ? types[p].ToClass() : Internal::vmData.Object;
            if (!type) type = Internal::vmData.Object;
            parameter->parameter_type = type.GetIl2CppType();
            parameter->token = parameter->parameter_type->attrs | p;

            ++parameter;
        }
#else

        auto parameters = (IL2CPP::Il2CppType **) malloc(argsCount * sizeof(IL2CPP::Il2CppType *));

        myInfo->parameters = (const IL2CPP::Il2CppType **) parameters;
        for (uint8_t p = 0; p < argsCount; ++p) {
            auto parameter = BNM_I2C_NEW(Il2CppType);

            // Установить тип в любом случае
            auto type = p < types.size() ? types[p].ToLC() : vmData.Object;
            if (!type) type = vmData.Object;
            memcpy(parameter, type.GetIl2CppType(), sizeof(IL2CPP::Il2CppType));

            parameters[p] = parameter;
        }
#endif
    }
#if UNITY_VER >= 212
    myInfo->methodMetadataHandle = nullptr;
#endif
    return myInfo;
}

bool HasInterface(IL2CPP::Il2CppClass *parent, IL2CPP::Il2CppClass *interface) {
    if (!parent || !interface) return false;
    for (uint16_t i = 0; i < parent->interfaces_count; ++i) if (parent->implementedInterfaces[i] == interface) return true;
    if (parent->parent) return HasInterface(parent->parent, interface);
    return false;
}

namespace CompileTimeClassProcessors {
    typedef void (*ProcessorType)(CompileTimeClass &target, CompileTimeClass::_BaseInfo *info);
    extern ProcessorType processors[(uint8_t) CompileTimeClass::_BaseType::MaxCount];
}

struct CustomClassInfo {
    const char *_namespace{}, *_name{}, *_imageName{};
    BNM::Class _class{};
};

#ifdef BNM_DEBUG

const char *CompileTimeClassModifiers[] = {
        "None",
        "Array",
        "Pointer",
        "Reference"
};
void LogCompileTimeClassInfo(BNM::CompileTimeClass::_BaseInfo *info, const BNM::CompileTimeClass &tmp) {
    switch (info->_baseType) {
        case BNM::CompileTimeClass::_BaseType::None:
            BNM_LOG_ERR("\tNone");
            break;
        case BNM::CompileTimeClass::_BaseType::Class: {
            auto classInfo = (CompileTimeClass::_ClassInfo *) info;
            BNM_LOG_ERR("\tClass( imageName: \"%s\", namespace: \"%s\", name: \"%s\") - %s", classInfo->_imageName, classInfo->_namespace, classInfo->_name, tmp._loadedClass.str().data());
        } break;
        case BNM::CompileTimeClass::_BaseType::Modifier: {
            BNM_LOG_ERR("\tModifier(\"%s\") - %s", CompileTimeClassModifiers[(uint8_t) ((CompileTimeClass::_ModifierInfo *) info)->_modifierType], tmp._loadedClass.str().data());
        } break;
        case BNM::CompileTimeClass::_BaseType::Generic: {
            auto genericInfo = (CompileTimeClass::_GenericInfo *) info;
            BNM_LOG_ERR("\tGeneric: ");
            for (auto info : genericInfo->_types) BNM_LOG_ERR("\t\t%s", info.ToClass().str().data());
            BNM_LOG_ERR("\t%s", tmp._loadedClass.str().data());
        } break;
    }
}
void LogCompileTimeClass(const BNM::CompileTimeClass &compileTimeClass) {
    if (compileTimeClass._stack.empty()) return BNM_LOG_ERR("\t" DBG_BNM_MSG_ClassesManagement_LogCompileTimeClass_None);

    CompileTimeClass tmp{};

    auto &stack = compileTimeClass._stack;
    for (auto info : stack) {

        auto index = (uint8_t) info->_baseType;
        if (index >= (uint8_t) CompileTimeClass::_BaseType::MaxCount) {
            BNM_LOG_ERR("\t" DBG_BNM_MSG_CompileTimeClass_ToClass_OoB_Warn, (size_t)index);
            continue;
        }
        CompileTimeClassProcessors::processors[index](tmp, (BNM::CompileTimeClass::_BaseInfo *) info);

        LogCompileTimeClassInfo(info, tmp);
    }

}
#endif

// Найти нужный класс или информацию о нём
CustomClassInfo GetClassInfo(const BNM::CompileTimeClass &compileTimeClass) {
    CompileTimeClass tmp{};

    auto &stack = compileTimeClass._stack;
    int i = 0;
    for (auto it = stack.begin(); it != stack.end(); ++it, i++) {
        auto info = *it;

        auto index = (uint8_t) info->_baseType;

        // Защита от других типов. Нас не интересуют Generic, Modifier и другое, что не относится к иерархии классов.
        if (index != (uint8_t) BNM::CompileTimeClass::_BaseType::Class) continue;

        CompileTimeClassProcessors::processors[index](tmp, (BNM::CompileTimeClass::_BaseInfo *) info);

        if (tmp._loadedClass) continue;

        auto it2 = it;
        bool found = true;
        while (++it2 != stack.end()) {
            if ((*it2)->_baseType != BNM::CompileTimeClass::_BaseType::Class) continue;
            found = false;
            break;
        }

        auto classInfo = (CompileTimeClass::_ClassInfo *) info;

        if (found) return {classInfo->_namespace, classInfo->_name, classInfo->_imageName};
        return {};
    }
    return {._class = compileTimeClass.ToClass()};
}

void ChangeInnerClassOwner(IL2CPP::Il2CppClass *owner, IL2CPP::Il2CppClass *inner) {
    auto oldOwner = inner->declaringType;
    auto oldInnerList = owner->nestedTypes;

    inner->declaringType = owner;

    // Добавить класс в список нового владельца
    auto newInnerList = (IL2CPP::Il2CppClass **) malloc(sizeof(IL2CPP::Il2CppClass) * (owner->nested_type_count + 1));
    memcpy(newInnerList, owner->nestedTypes, sizeof(IL2CPP::Il2CppClass) * owner->nested_type_count);
    newInnerList[owner->nested_type_count++] = inner;
    owner->nestedTypes = newInnerList;

    // Отметить класс, чтобы использовать меньше памяти
    if ((owner->flags & 0x99000000) == 0x99000000) free(oldInnerList);
    owner->flags |= 0x99000000;

    // Удалить класс из списка старого владельца
    if (oldOwner) {
        oldInnerList = oldOwner->nestedTypes;
        newInnerList = (IL2CPP::Il2CppClass **) malloc(sizeof(IL2CPP::Il2CppClass) * (oldOwner->nested_type_count - 1));
        uint8_t skipped = 0;
        for (uint16_t i = 0; i < oldOwner->nested_type_count; ++i) {
            if (skipped == 0) if (skipped = (oldInnerList[i] == inner); skipped) continue;
            newInnerList[i - skipped] = oldInnerList[i];
        }
        oldOwner->nestedTypes = newInnerList;
        --oldOwner->nested_type_count;

        // Отметить класс, чтобы использовать меньше памяти
        if ((oldOwner->flags & 0x99000000) == 0x99000000) free(oldInnerList);
        oldOwner->flags |= 0x99000000;
    }
}

void ModifyClass(BNM::MANAGEMENT_STRUCTURES::CustomClass *customClass, Class target) {
    BNM_LOG_DEBUG(DBG_BNM_MSG_ClassesManagement_ModifyClasses_Target, target.str().data());

    auto klass = target._data;

    auto baseType = customClass->_baseType.ToClass();
    if (baseType) klass->parent = baseType._data;

    IL2CPP::Il2CppClass *owner = customClass->_owner;

    if (owner) ChangeInnerClassOwner(owner, klass);

    auto newMethodsCount = customClass->_methods.size();
    auto newFieldsCount = customClass->_fields.size();

    if (newMethodsCount) {
        auto oldCount = klass->method_count;
        auto oldMethods = klass->methods;

        std::vector<BNM::IL2CPP::MethodInfo *> methodsToAdd{};

        for (size_t i = 0; i < newMethodsCount; ++i) {
            auto method = customClass->_methods[i];

            bool isHooked = false;
            method->myInfo = ProcessCustomMethod(method, target, &isHooked);
#if UNITY_VER > 174
#define kls klass
#else
#define kls declaring_type
#endif
            method->myInfo->kls = klass;
#undef kls

            if (!isHooked) methodsToAdd.push_back(method->myInfo);
            BNM_LOG_DEBUG_IF(isHooked, DBG_BNM_MSG_ClassesManagement_ModifyClasses_Hooked_Method, (method->_isStatic == 1) ? DBG_BNM_MSG_ClassesManagement_ModifyClasses_Method_Static : "", method->_name.data(), method->_parameterTypes.size());
            BNM_LOG_DEBUG_IF(!isHooked, DBG_BNM_MSG_ClassesManagement_ModifyClasses_Added_Method, (method->_isStatic == 1) ? DBG_BNM_MSG_ClassesManagement_ModifyClasses_Method_Static : "", method->_name.data(), method->_parameterTypes.size());
        }

        if (!methodsToAdd.empty()) {
            auto newMethods = (IL2CPP::MethodInfo **) malloc((oldCount + methodsToAdd.size()) * sizeof(IL2CPP::MethodInfo *));

            auto oldSize = oldCount * sizeof(IL2CPP::MethodInfo *);

            if (oldCount) memcpy(newMethods, oldMethods, oldSize);

            memcpy(newMethods + oldCount, methodsToAdd.data(), methodsToAdd.size() * sizeof(IL2CPP::MethodInfo *));

            klass->methods = (const IL2CPP::MethodInfo **)newMethods;
            klass->method_count += methodsToAdd.size();
        }
    }

    if (newFieldsCount) {
        auto oldCount = klass->field_count;

        auto newFields = (IL2CPP::FieldInfo *)malloc((oldCount + newFieldsCount) * sizeof(IL2CPP::FieldInfo));

        if (oldCount) memcpy(newFields, klass->fields, oldCount * sizeof(IL2CPP::FieldInfo));

        auto currentAddress = klass->instance_size;

        IL2CPP::FieldInfo *newField = newFields + oldCount;

        for (auto field : customClass->_fields) {
            auto name = field->_name;
            auto len = name.size();
            newField->name = (char *) malloc(len + 1);
            memcpy((void *)newField->name, name.data(), len);
            ((char *)newField->name)[len] = 0;

            newField->type = BNM_I2C_NEW(Il2CppType);
            auto fieldType = field->_type.ToClass();
            if (!fieldType) fieldType = Internal::vmData.Object;
            memcpy((void *)newField->type, (void *)fieldType.GetIl2CppType(), sizeof(IL2CPP::Il2CppType));

            ((IL2CPP::Il2CppType*)newField->type)->attrs |= 0x0006; // PUBLIC
            newField->token = newField->type->attrs;
            newField->parent = klass;

            field->offset = currentAddress;
            newField->offset = (int32_t) currentAddress;
            field->myInfo = newField;

            currentAddress += field->_size;

            newField++;
            BNM_LOG_DEBUG(DBG_BNM_MSG_ClassesManagement_ModifyClasses_Added_Field, name);
        }
    }
}

char forEmptyString = '\0';
void CreateClass(BNM::MANAGEMENT_STRUCTURES::CustomClass *customClass, const CustomClassInfo &classInfo) {
    Image image{};
    if (classInfo._imageName) image = Image(classInfo._imageName);
    else image = Image(OBFUSCATE_BNM("Assembly-CSharp.dll"));
    if (!image) image = MakeImage(classInfo._imageName);

    IL2CPP::Il2CppType classType;
    memset(&classType, 0, sizeof(IL2CPP::Il2CppType));
    classType.type = IL2CPP::Il2CppTypeEnum::IL2CPP_TYPE_CLASS;
    classType.attrs = 0x1; // Public
    classType.pinned = 0;
    classType.byref = 0;
    classType.num_mods = 31;


    IL2CPP::Il2CppClass *parent = customClass->_baseType;
    if (!parent) parent = Internal::vmData.Object;

    IL2CPP::Il2CppClass *owner = customClass->_owner;


    std::vector<IL2CPP::Il2CppRuntimeInterfaceOffsetPair> newInterOffsets{};
    if (parent->interfaceOffsets)
        for (uint16_t i = 0; i < parent->interface_offsets_count; ++i)
            newInterOffsets.push_back(parent->interfaceOffsets[i]);


    auto newVtableSize = parent->vtable_count;

    auto allInterfaces = customClass->_interfaces;
    std::vector<IL2CPP::Il2CppClass *> interfaces{};
    for (auto &interface : allInterfaces)
        if (auto cls = interface.ToIl2CppClass(); !HasInterface(parent, cls))
            interfaces.push_back(cls);

    // Требуется для переопределения виртуальных методов
    std::vector<IL2CPP::VirtualInvokeData> newVTable(newVtableSize);
    for (uint16_t i = 0; i < parent->vtable_count; ++i) newVTable[i] = parent->vtable[i];
    for (auto interface : interfaces) {
        newInterOffsets.push_back({interface, newVtableSize});
        for (uint16_t i = 0; i < interface->method_count; ++i) {
            auto v = interface->methods[i];
            ++newVtableSize;
            newVTable.push_back({nullptr, v});
        }
    }

    // Создать все новые методы
    uint8_t hasFinalize = 0;
    auto methods = (const IL2CPP::MethodInfo **) malloc(customClass->_methods.size() * sizeof(IL2CPP::MethodInfo *));

    for (size_t i = 0; i < customClass->_methods.size(); ++i) {
        auto method = customClass->_methods[i];

        method->myInfo = ProcessCustomMethod(method, {});

        // Замена методов в таблице виртуальных методов
        uint16_t v = 0;
        NEXT:
        for (; v < newVtableSize; ++v) {
            auto &vTable = newVTable[v];
            auto count = vTable.method->parameters_count;

            if (!strcmp(vTable.method->name, method->myInfo->name) && count == method->myInfo->parameters_count && method->_parameterTypes.size() == count) {
                for (uint8_t p = 0; p < count; ++p) {
#if UNITY_VER < 212
                    auto type = (vTable.method->parameters + p)->parameter_type;
#else
                    auto type = vTable.method->parameters[p];
#endif
                    if (Class(type).GetClass() != method->_parameterTypes[p].ToIl2CppClass()) goto NEXT;
                }
                if (!hasFinalize) hasFinalize = v == Internal::finalizerSlot;
                vTable.method = method->myInfo;
                vTable.methodPtr = method->myInfo->methodPointer;
                break;

            }
        }

        methods[i] = method->myInfo;
    }


    auto klass = customClass->myClass = (IL2CPP::Il2CppClass *)malloc(sizeof(IL2CPP::Il2CppClass) + newVTable.size() * sizeof(IL2CPP::VirtualInvokeData));
    memset(klass, 0, sizeof(IL2CPP::Il2CppClass) + newVTable.size() * sizeof(IL2CPP::VirtualInvokeData));
    klass->parent = parent;

    if (owner) ChangeInnerClassOwner(owner, klass);


#if UNITY_VER > 174
#define kls klass
#else
#define kls declaring_type
#endif
    // Завершение создания методов
    for (auto method : customClass->_methods) method->myInfo->kls = klass;
    klass->method_count = customClass->_methods.size();
    klass->methods = methods;
#undef kls

    // Завершение создания полей

    klass->field_count = customClass->_fields.size();
    if (klass->field_count > 0) {
        // Создать список полей
        auto fields = (IL2CPP::FieldInfo *)malloc(klass->field_count * sizeof(IL2CPP::FieldInfo));

        // Получить первое поле
        IL2CPP::FieldInfo *newField = fields;
        for (auto field : customClass->_fields) {
            auto name = field->_name;
            auto len = name.size();
            newField->name = (char *) malloc(len + 1);
            memcpy((void *)newField->name, name.data(), len);
            ((char *)newField->name)[len] = 0;

            // Копировать тип
            newField->type = BNM_I2C_NEW(Il2CppType);
            auto fieldType = field->_type.ToClass();
            if (!fieldType) fieldType = Internal::vmData.Object;
            memcpy((void *)newField->type, (void *)fieldType.GetIl2CppType(), sizeof(IL2CPP::Il2CppType));

            ((IL2CPP::Il2CppType*)newField->type)->attrs |= 0x0006; // PUBLIC
            newField->token = newField->type->attrs;
            newField->parent = klass;
            newField->offset = (int32_t) field->offset;
            field->myInfo = newField;

            newField++;
        }
        klass->fields = fields;
    } else klass->fields = nullptr;
    klass->static_fields_size = 0;
    klass->static_fields = nullptr;

    klass->typeHierarchyDepth = parent->typeHierarchyDepth + 1;
    klass->typeHierarchy = (IL2CPP::Il2CppClass **)malloc(klass->typeHierarchyDepth * sizeof(IL2CPP::Il2CppClass *));
    memcpy(klass->typeHierarchy, parent->typeHierarchy, parent->typeHierarchyDepth * sizeof(IL2CPP::Il2CppClass *));
    klass->typeHierarchy[parent->typeHierarchyDepth] = klass;

    klass->image = image;

    auto len = strlen(classInfo._name);
    klass->name = (char *) malloc(len + 1);
    memcpy((void *)klass->name, classInfo._name, len);
    ((char *)klass->name)[len] = 0;


    if (!owner && classInfo._namespace) {
        len = strlen(classInfo._namespace);
        klass->namespaze = (char *) malloc(len + 1);
        memcpy((void *)klass->namespaze, classInfo._namespace, len);
        ((char *)klass->namespaze)[len] = 0;
    } else klass->namespaze = &forEmptyString;


    classType.data.dummy = klass;
#if UNITY_VER > 174
    klass->this_arg = klass->byval_arg = classType;
    klass->this_arg.byref = 1;
#else
    klass->byval_arg = BNM_I2C_NEW(Il2CppType);
    *((IL2CPP::Il2CppType *)klass->byval_arg) = classType;
    classType.byref = 1;
    klass->this_arg = BNM_I2C_NEW(Il2CppType);
    *((IL2CPP::Il2CppType *)klass->this_arg) = classType;
#endif

    // Копировать флаги родителя и убрать ABSTRACT флаг, если существует
    klass->flags = klass->parent->flags & ~0x00000080; // TYPE_ATTRIBUTE_ABSTRACT

    klass->element_class = klass;
    klass->castClass = klass;
#if UNITY_VER > 174
    klass->klass = klass;
#endif

    // Инициализировать размеры
    klass->native_size = -1;
    klass->element_size = 0;
    klass->instance_size = klass->actualSize = customClass->_size;

    // Установить таблицу виртуальных методов
    klass->vtable_count = newVTable.size();
    for (size_t i = 0; i < newVTable.size(); ++i) klass->vtable[i] = newVTable[i];

    // Установить адреса интерфейсов
    klass->interface_offsets_count = newInterOffsets.size();
    klass->interfaceOffsets = (IL2CPP::Il2CppRuntimeInterfaceOffsetPair *)malloc(newInterOffsets.size() * sizeof(IL2CPP::Il2CppRuntimeInterfaceOffsetPair));
    memcpy(klass->interfaceOffsets, newInterOffsets.data(), newInterOffsets.size() * sizeof(IL2CPP::Il2CppRuntimeInterfaceOffsetPair));

    // Добавить интерфейсы
    if (!interfaces.empty()) {
        klass->interfaces_count = interfaces.size();
        klass->implementedInterfaces = (IL2CPP::Il2CppClass **)malloc(interfaces.size() * sizeof(IL2CPP::Il2CppClass *));
        memcpy(klass->implementedInterfaces, interfaces.data(), interfaces.size() * sizeof(IL2CPP::Il2CppClass *));
    } else {
        klass->interfaces_count = 0;
        klass->implementedInterfaces = nullptr;
    }

    // Запретить il2cpp вызывать LivenessState::TraverseGCDescriptor для класса
    klass->gc_desc = nullptr;

    // BNM не поддерживает создание generic классов
    klass->generic_class = nullptr;
    klass->genericRecursionDepth = 1;
#if UNITY_VER < 202
    klass->genericContainerIndex = -1;
#else
    klass->genericContainerHandle = nullptr;
    klass->typeMetadataHandle = nullptr;
#endif


#if UNITY_VER < 211
    klass->valuetype = 0;
#endif
    // BNM не поддерживает создание конструктора статических полей (.cctor). Il2Cpp устроен так, что его не вызовет никогда, да и BNM не позволяет создавать статических полей
    klass->has_cctor = 0;

    // Остальные переменные, которые необходимо установить
    klass->interopData = nullptr;
    klass->nestedTypes = nullptr;
    klass->properties = nullptr;
    klass->rgctx_data = nullptr;
    klass->has_references = 0;
    klass->has_finalize = hasFinalize;
    klass->size_inited = klass->is_vtable_initialized = 1;
    klass->initialized = 1;
#if UNITY_VER > 182
    klass->initialized_and_no_error = 1;
#if UNITY_VER >= 231
    klass->initializationExceptionGCHandle = nullptr;
#else
    klass->initializationExceptionGCHandle = 0;
#endif
#if UNITY_VER < 212
    klass->has_initialization_error = 0;
#endif
#if UNITY_VER < 222
    klass->naturalAligment = 0;
#else
    klass->stack_slot_size = sizeof(void *);
#endif
#endif
    klass->init_pending = 0;
    klass->enumtype = 0;
    klass->minimumAlignment = sizeof(void *);
    klass->is_generic = 0;
    klass->rank = 0;
    klass->nested_type_count = 0;
    klass->thread_static_fields_offset = 0;
    klass->thread_static_fields_size = 0;
    klass->cctor_started = 0;
#if UNITY_VER >= 203 && (UNITY_VER != 211 || UNITY_PATCH_VER >= 24)
    klass->size_init_pending = 0;
#endif
#if UNITY_VER >= 212
    klass->cctor_finished_or_no_cctor = 1;
#else
    klass->cctor_finished = 1;
#endif

    klass->cctor_thread = 0;

    // Добавить класс к списку созданных классов
    Internal::ClassesManagement::BNMClassesMap.addClass(image.GetInfo(), klass);

    // Получить С#-тип
    customClass->type = Class(klass);

    BNM_LOG_INFO(DBG_BNM_MSG_ClassesManagement_CreateClass_Added, klass, klass->namespaze, klass->name, parent->namespaze, parent->name, klass->image->name);
}

void Internal::ClassesManagement::ProcessCustomClasses() {
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
    std::lock_guard<std::mutex> lock(customClassesMutex);
#endif

    if (classesManagementVector == nullptr) return;
    for (auto customClass : *classesManagementVector) {
        auto type = customClass->_targetType;
        type._autoFree = false;
        auto info = GetClassInfo(type);
        if (info._class) ModifyClass(customClass, info._class);
        else if (info._name) CreateClass(customClass, info);
        else {
#ifdef BNM_DEBUG
            BNM_LOG_ERR(DBG_BNM_MSG_ClassesManagement_ProcessCustomClasses_Error);
            LogCompileTimeClass(type);
#endif
        }
        type.Free();
    }
    classesManagementVector->clear(); classesManagementVector->shrink_to_fit();
    delete classesManagementVector;
    classesManagementVector = nullptr;

}
void BNM::MANAGEMENT_STRUCTURES::AddClass(CustomClass *klass) {
    if (!BNM::Internal::ClassesManagement::classesManagementVector) Internal::ClassesManagement::classesManagementVector = new std::vector<MANAGEMENT_STRUCTURES::CustomClass *>();
    Internal::ClassesManagement::classesManagementVector->push_back(klass);
}
#undef BNM_I2C_NEW
#endif
