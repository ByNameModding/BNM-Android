#include <BNM/ClassesManagement.hpp>

#ifdef BNM_CLASSES_MANAGEMENT

#include <Internals.hpp>

using namespace BNM;

void BNM::MANAGEMENT_STRUCTURES::AddClass(CustomClass *_class) {
    if (!BNM::Internal::ClassesManagement::classesManagementVector) {
        Internal::ClassesManagement::classesManagementVector = (std::vector<MANAGEMENT_STRUCTURES::CustomClass *> *) BNM_malloc(sizeof(std::vector<MANAGEMENT_STRUCTURES::CustomClass *>));
        memset((void *) Internal::ClassesManagement::classesManagementVector, 0, sizeof(std::vector<MANAGEMENT_STRUCTURES::CustomClass *>));
    }

    Internal::ClassesManagement::classesManagementVector->push_back(_class);
}


#define BNM_I2C_NEW(type) (IL2CPP::type *) BNM_malloc(sizeof(IL2CPP::type))

struct CustomClassInfo {
    const char *_namespace{}, *_name{}, *_imageName{};
    BNM::Class _class{};
};

// The code for changing the data of an existing class
static void ModifyClass(BNM::MANAGEMENT_STRUCTURES::CustomClass *customClass, Class target);
// Code for creating new classes
static void CreateClass(BNM::MANAGEMENT_STRUCTURES::CustomClass *customClass, const CustomClassInfo &classInfo);

// Find the desired class or information about it
static CustomClassInfo GetClassInfo(const BNM::CompileTimeClass &compileTimeClass);

// The code for creating a new image and assembling it
static IL2CPP::Il2CppImage *MakeImage(std::string_view imageName);
// The code for processing a new method for its subsequent creation/modification
static IL2CPP::MethodInfo *ProcessCustomMethod(MANAGEMENT_STRUCTURES::CustomMethod *method, Class target, bool *hooked = nullptr);
// The code for setting data in fields
static void SetupField(IL2CPP::FieldInfo *newField, MANAGEMENT_STRUCTURES::CustomField *field);


static void ProcessInterface(IL2CPP::Il2CppClass *parent, IL2CPP::Il2CppClass *interface, std::vector<IL2CPP::Il2CppClass *> &interfaces);
// The code for changing the parent of the class and the owner of the nested class
static void SetupClassOwnerAndParent(IL2CPP::Il2CppClass *target, IL2CPP::Il2CppClass *owner, IL2CPP::Il2CppClass *parent);
// Code for checking whether a class and its parents have an interface
static bool HasInterface(IL2CPP::Il2CppClass *parent, IL2CPP::Il2CppClass *interface);
// Class type setup code
static void SetupTypes(IL2CPP::Il2CppClass *target);

void Internal::ClassesManagement::ProcessCustomClasses() {
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
            Utils::LogCompileTimeClass(type);
#endif
        }
        type.Free();
    }

    classesManagementVector->clear(); classesManagementVector->shrink_to_fit();
    BNM_free((void *) classesManagementVector);
    classesManagementVector = nullptr;
}

static void ModifyClass(BNM::MANAGEMENT_STRUCTURES::CustomClass *customClass, Class target) {
    BNM_LOG_DEBUG(DBG_BNM_MSG_ClassesManagement_ModifyClasses_Target, target.str().data());

    auto klass = target._data;

    auto baseType = customClass->_baseType.ToClass();
    if (baseType) klass->parent = baseType._data;

    IL2CPP::Il2CppClass *owner = customClass->_owner;

    SetupClassOwnerAndParent(klass, owner, baseType._data);

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
            BNM_LOG_DEBUG_IF(isHooked, DBG_BNM_MSG_ClassesManagement_ModifyClasses_Hooked_Method, (method->_isStatic == 1) ? DBG_BNM_MSG_ClassesManagement_Method_Static : "", method->_name.data(), method->_parameterTypes.size());
            BNM_LOG_DEBUG_IF(!isHooked, DBG_BNM_MSG_ClassesManagement_ModifyClasses_Added_Method, (method->_isStatic == 1) ? DBG_BNM_MSG_ClassesManagement_Method_Static : "", method->_name.data(), method->_parameterTypes.size());
            BNM_LOG_DEBUG_IF(method->_origin && method->_origin != method->myInfo, DBG_BNM_MSG_ClassesManagement_ModifyClasses_Overridden_Method, BNM::MethodBase(method->_origin).str().c_str());
        }

        if (!methodsToAdd.empty()) {
            auto newMethods = (IL2CPP::MethodInfo **) BNM_malloc((oldCount + methodsToAdd.size()) * sizeof(IL2CPP::MethodInfo *));

            auto oldSize = oldCount * sizeof(IL2CPP::MethodInfo *);

            if (oldCount) memcpy(newMethods, oldMethods, oldSize);

            memcpy(newMethods + oldCount, methodsToAdd.data(), methodsToAdd.size() * sizeof(IL2CPP::MethodInfo *));

            klass->methods = (const IL2CPP::MethodInfo **)newMethods;
            klass->method_count += methodsToAdd.size();
        }
    }

    if (newFieldsCount) {
        auto oldCount = klass->field_count;

        auto newFields = (IL2CPP::FieldInfo *) BNM_malloc((oldCount + newFieldsCount) * sizeof(IL2CPP::FieldInfo));

        if (oldCount) memcpy(newFields, klass->fields, oldCount * sizeof(IL2CPP::FieldInfo));

        auto &currentAddress = klass->instance_size;

        IL2CPP::FieldInfo *newField = newFields + oldCount;

        for (auto field : customClass->_fields) {
            SetupField(newField, field);
            newField->parent = klass;

            field->offset = currentAddress;
            newField->offset = (int32_t) currentAddress;
            field->myInfo = newField;

            currentAddress += field->_size;

            newField++;
            BNM_LOG_DEBUG(DBG_BNM_MSG_ClassesManagement_ModifyClasses_Added_Field, field->_name.data());
        }

        klass->actualSize = currentAddress;
    }

    customClass->myClass = klass;
    customClass->type = Class(klass);
}

static char forEmptyString = '\0';
static const char *baseImageName = BNM_OBFUSCATE_TMP("Assembly-CSharp.dll");
static void CreateClass(BNM::MANAGEMENT_STRUCTURES::CustomClass *customClass, const CustomClassInfo &classInfo) {
    Image image{};
    if (classInfo._imageName) {
        auto &assemblies = *Internal::Assembly$$GetAllAssemblies();
        for (auto assembly: assemblies) {
            auto currentImage = Internal::il2cppMethods.il2cpp_assembly_get_image(assembly);
            if (!Internal::CompareImageName(currentImage, classInfo._imageName)) continue;
            image = currentImage;
            break;
        }
    } else image = Image(baseImageName);
    if (!image) image = MakeImage(classInfo._imageName);

    BNM_LOG_DEBUG(DBG_BNM_MSG_ClassesManagement_CreateClass_Target, classInfo._namespace, classInfo._name, image._data->name);
    
    IL2CPP::Il2CppClass *parent = customClass->_baseType;
    if (!parent) parent = Internal::vmData.Object;

    IL2CPP::Il2CppClass *owner = customClass->_owner;


    std::vector<IL2CPP::Il2CppRuntimeInterfaceOffsetPair> newInterOffsets{};
    if (parent->interfaceOffsets)
        for (uint16_t i = 0; i < parent->interface_offsets_count; ++i)
            newInterOffsets.push_back(parent->interfaceOffsets[i]);

    auto allInterfaces = customClass->_interfaces;
    std::vector<IL2CPP::Il2CppClass *> interfaces{};
    for (auto &interface : allInterfaces)
        if (auto cls = interface.ToClass(); cls)
            ProcessInterface(parent, cls, interfaces);

    // Required to override virtual methods
    auto newVtableSize = parent->vtable_count;
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

    // Create all new methods
    uint8_t hasFinalize = 0;
    auto methods = (const IL2CPP::MethodInfo **) BNM_malloc(customClass->_methods.size() * sizeof(IL2CPP::MethodInfo *));

    for (size_t i = 0; i < customClass->_methods.size(); ++i) {
        auto method = customClass->_methods[i];

        method->myInfo = ProcessCustomMethod(method, {});


        BNM_LOG_DEBUG(DBG_BNM_MSG_ClassesManagement_CreateClass_Added_Method, (method->_isStatic == 1) ? DBG_BNM_MSG_ClassesManagement_Method_Static : "", method->_name.data(), method->_parameterTypes.size());
        
        // Replacing non-static methods in the virtual methods table
        if (!method->_isStatic) for (uint16_t v = 0; v < newVtableSize; ++v) {
            auto &vTable = newVTable[v];
            auto count = vTable.method->parameters_count;

            if (!strcmp(vTable.method->name, method->myInfo->name) && count == method->myInfo->parameters_count && method->_parameterTypes.size() == count) {
                if (!method->_skipTypeMatch) for (uint8_t p = 0; p < count; ++p) {
#if UNITY_VER < 212
                    auto type = (vTable.method->parameters + p)->parameter_type;
#else
                    auto type = vTable.method->parameters[p];
#endif
                    if (Class(type).GetClass() != method->_parameterTypes[p].ToIl2CppClass()) goto NEXT;
                }

                if (!hasFinalize) hasFinalize = v == Internal::finalizerSlot;
                method->_origin = (BNM::IL2CPP::MethodInfo *) vTable.method;
                method->_originalAddress = (void *) (vTable.method ? vTable.method->methodPointer : nullptr);
                vTable.method = method->myInfo;
                vTable.methodPtr = method->myInfo->methodPointer;

                BNM_LOG_DEBUG(DBG_BNM_MSG_ClassesManagement_CreateClass_Overridden_Method, BNM::MethodBase(method->_origin).str().c_str());

                break;

            }
            NEXT:
            [[maybe_unused]] uint8_t thisGotoRequiresCpp23Min;
        }

        methods[i] = method->myInfo;

    }

    auto klass = customClass->myClass = (IL2CPP::Il2CppClass *) BNM_malloc(sizeof(IL2CPP::Il2CppClass) + newVTable.size() * sizeof(IL2CPP::VirtualInvokeData));
    memset(klass, 0, sizeof(IL2CPP::Il2CppClass) + newVTable.size() * sizeof(IL2CPP::VirtualInvokeData));

    klass->initialized = 1;
    klass->init_pending = 0;
#if UNITY_VER > 182

    klass->initialized_and_no_error = 1;

#if UNITY_VER < 212
    klass->has_initialization_error = 0;
#endif

#endif

    klass->image = image;

    // Prevent il2cpp from calling LivenessState::TraverseGCDescriptor for a class
    klass->gc_desc = nullptr;

    auto len = strlen(classInfo._name);
    klass->name = (char *) BNM_malloc(len + 1);
    memcpy((void *)klass->name, classInfo._name, len);
    ((char *)klass->name)[len] = 0;

    if (!owner && classInfo._namespace) {
        len = strlen(classInfo._namespace);
        klass->namespaze = (char *) BNM_malloc(len + 1);
        memcpy((void *)klass->namespaze, classInfo._namespace, len);
        ((char *)klass->namespaze)[len] = 0;
    } else klass->namespaze = &forEmptyString;

    SetupTypes(klass);

    klass->element_class = klass;
    klass->castClass = klass;

    SetupClassOwnerAndParent(klass, owner, parent);

    // BNM does not support the creation of generic classes
    klass->generic_class = nullptr;

#if UNITY_VER < 202
    klass->typeDefinition = nullptr;
#else
    klass->typeMetadataHandle = nullptr;
#endif


#if UNITY_VER > 174
    klass->klass = klass;
#endif

    klass->field_count = customClass->_fields.size();
    if (klass->field_count > 0) {
        // Create a list of fields
        auto fields = (IL2CPP::FieldInfo *) BNM_malloc(klass->field_count * sizeof(IL2CPP::FieldInfo));

        // Get the first field
        IL2CPP::FieldInfo *newField = fields;
        for (auto field : customClass->_fields) {
            SetupField(newField, field);
            newField->parent = klass;

            newField->offset = (int32_t) field->offset;
            field->myInfo = newField;

            newField++;
        }
        klass->fields = fields;
    } else klass->fields = nullptr;

    // Add Interfaces
    if (!interfaces.empty()) {
        klass->interfaces_count = interfaces.size();
        klass->implementedInterfaces = (IL2CPP::Il2CppClass **) BNM_malloc(interfaces.size() * sizeof(IL2CPP::Il2CppClass *));
        memcpy(klass->implementedInterfaces, interfaces.data(), interfaces.size() * sizeof(IL2CPP::Il2CppClass *));
    } else {
        klass->interfaces_count = 0;
        klass->implementedInterfaces = nullptr;
    }

#if UNITY_VER > 174
#define kls klass
#else
#define kls declaring_type
#endif

    // Completing the creation of methods
    for (auto method : customClass->_methods) method->myInfo->kls = klass;
    klass->method_count = customClass->_methods.size();
    klass->methods = methods;
    klass->has_finalize = hasFinalize;

#undef kls

    // Copy the parent flags and remove the ABSTRACT flag if it exists
    klass->flags = klass->parent->flags & ~(0x00000080 | 0x00000020); // TYPE_ATTRIBUTE_ABSTRACT

    // Initialize sizes
    klass->native_size = -1;
    klass->element_size = sizeof(void *);
    klass->instance_size = klass->actualSize = customClass->_size;

    // Install a table of virtual methods
    klass->vtable_count = newVTable.size();
    for (size_t i = 0; i < newVTable.size(); ++i) klass->vtable[i] = newVTable[i];

    // Set interface addresses
    klass->interface_offsets_count = newInterOffsets.size();
    klass->interfaceOffsets = (IL2CPP::Il2CppRuntimeInterfaceOffsetPair *) BNM_malloc(newInterOffsets.size() * sizeof(IL2CPP::Il2CppRuntimeInterfaceOffsetPair));
    memcpy(klass->interfaceOffsets, newInterOffsets.data(), newInterOffsets.size() * sizeof(IL2CPP::Il2CppRuntimeInterfaceOffsetPair));

    klass->interopData = nullptr;
    klass->events = nullptr; // Creation is not supported
    klass->properties = nullptr; // Creation is not supported
    klass->nestedTypes = nullptr;
    klass->rgctx_data = nullptr; // Required for generic

    klass->static_fields = nullptr; // Creation is not supported
    klass->static_fields_size = 0;

    klass->genericRecursionDepth = 0;

#if UNITY_VER < 202
    klass->genericContainerIndex = -1;
#else
    klass->genericContainerHandle = nullptr;
#endif


#if UNITY_VER < 211
    // BNM создаёт только классы, т.к. новые struct особо не нужны для модификаций
    // BNM creates only classes, because new structs are not particularly needed for modifications
    klass->valuetype = 0;
#endif

    // BNM does not support creating a static field constructor (.cctor). Il2Cpp is designed so that it will never be called, and BNM does not allow you to create static fields
    klass->has_cctor = 0;

    klass->has_references = 0;
    klass->size_inited = 1;
    klass->is_vtable_initialized = 1;

#if UNITY_VER > 182
    klass->initializationExceptionGCHandle = (decltype(klass->initializationExceptionGCHandle)) 0;

#   if UNITY_VER < 222
    klass->naturalAligment = 1;
#   else
    klass->stack_slot_size = sizeof(void *);
#   endif

#endif
    klass->enumtype = 0;
    klass->minimumAlignment = 1;
    klass->is_generic = 0;
    klass->rank = 0;
    klass->nested_type_count = 0;
    klass->thread_static_fields_offset = 0;
    klass->thread_static_fields_size = 0;
    klass->cctor_started = 0;
    klass->packingSize = 0;

#if UNITY_VER >= 203 && (UNITY_VER != 211 || UNITY_PATCH_VER >= 24)
    klass->size_init_pending = 0;
#endif

#if UNITY_VER >= 212
    klass->cctor_finished_or_no_cctor = 1;
#else
    klass->cctor_finished = 1;
#endif

    klass->cctor_thread = 0;

    // Add a class to the list of created classes
    Internal::ClassesManagement::bnmClassesMap.AddClass(image.GetInfo(), klass);

    // Get the C# type
    customClass->type = Class(klass);
}



namespace CompileTimeClassProcessors {
    typedef void (*ProcessorType)(CompileTimeClass &target, CompileTimeClass::_BaseInfo *info);
    extern ProcessorType processors[(uint8_t) CompileTimeClass::_BaseType::MaxCount];
}

static CustomClassInfo GetClassInfo(const BNM::CompileTimeClass &compileTimeClass) {
    CompileTimeClass tmp{};

    auto &stack = compileTimeClass._stack;
    int i = 0;
    for (auto it = stack.begin(); it != stack.end(); ++it, i++) {
        auto info = *it;

        auto index = (uint8_t) info->_baseType;

        // Protection from other types. We are not interested in Generic, Modifier, and other things that do not relate to the class hierarchy.
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


static const char *dotDllString = BNM_OBFUSCATE_TMP(".dll");
static IL2CPP::Il2CppImage *MakeImage(std::string_view imageName) {
    auto newImg = BNM_I2C_NEW(Il2CppImage);

    if (imageName.ends_with(dotDllString)) imageName.remove_suffix(4);

    auto nameLen = imageName.size();
#if UNITY_VER >= 171
    newImg->nameNoExt = (char *) BNM_malloc(nameLen + 1);
    memcpy((void *)newImg->nameNoExt, (void *)imageName.data(), nameLen);
    ((char *)newImg->nameNoExt)[nameLen] = 0;
#endif
    newImg->name = (char *) BNM_malloc(nameLen + 5);
    memcpy((void *)newImg->name, (void *)imageName.data(), nameLen);
    auto nameEnd = ((char *)(newImg->name + nameLen));
    nameEnd[0] = '.'; nameEnd[1] = 'd'; nameEnd[2] = 'l'; nameEnd[3] = 'l'; nameEnd[4] = 0;

#if UNITY_VER > 182
    newImg->assembly = nullptr;
    newImg->customAttributeCount = 0;

#   if UNITY_VER < 201
    newImg->customAttributeStart = -1;
#   endif

#endif

#if UNITY_VER > 201
    // Create an empty Il2CppImageDefinition
    auto handle = (IL2CPP::Il2CppImageDefinition *) BNM_malloc(sizeof(IL2CPP::Il2CppImageDefinition));
    memset(handle, 0, sizeof(IL2CPP::Il2CppImageDefinition));
    handle->typeStart = -1;
    handle->entryPointIndex = -1;
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

#   if UNITY_VER >= 171
    newImg->exportedTypeStart = -1;
#   endif
#endif
    // Initialize variables
    newImg->typeCount = 0;
#if UNITY_VER >= 171
    newImg->exportedTypeCount = 0;
#endif
    newImg->token = 1;

    // Create a new build for the image
    auto newAsm = BNM_I2C_NEW(Il2CppAssembly);
    auto &aName = newAsm->aname;
#if UNITY_VER > 174
    // Set image and assembly
    newAsm->image = newImg;
    newAsm->image->assembly = newAsm;
    aName.name = newImg->name;
    aName.culture = nullptr;
    aName.public_key = nullptr;
#else
    // Negative value to disable analysis from il2cpp
    static int newAsmCount = 1;
    newImg->assemblyIndex = newAsm->imageIndex = -newAsmCount;
    newAsmCount++;
    aName.publicKeyIndex = aName.hashValueIndex = aName.nameIndex = 0;
    aName.cultureIndex = -1;
#endif
    // Initialize these variables just in case
    aName.revision = aName.build = aName.minor = aName.major = 0;
#if UNITY_VER > 174
    aName.public_key_token[0] = aName.hash_len = 0;
#else
    aName.publicKeyToken[0] = aName.hash_len = 0;
#endif

    aName.hash_alg = aName.flags = 0;
    newAsm->referencedAssemblyStart = -1;
    newAsm->referencedAssemblyCount = 0;

    // Using this, BNM can check if it has created this assembly
    newImg->nameToClassHashTable = (decltype(newImg->nameToClassHashTable)) -0x424e4d;

    // Add an assembly to the list
    Internal::Assembly$$GetAllAssemblies()->push_back(newAsm);

    BNM_LOG_INFO(DBG_BNM_MSG_ClassesManagement_MakeImage_Added_Image, imageName.data());

    return newImg;
}

static IL2CPP::VirtualInvokeData *TryFindVirtualMethod(Class target, IL2CPP::MethodInfo *targetMethod) {
    for (uint16_t i = 0; i < target._data->vtable_count; ++i) {
        auto &it = target._data->vtable[i];
        if (it.method != targetMethod) continue;
        return &it;
    }
    return nullptr;
}

static IL2CPP::MethodInfo *CreateMethod(MANAGEMENT_STRUCTURES::CustomMethod *method);
static IL2CPP::MethodInfo *ProcessCustomMethod(MANAGEMENT_STRUCTURES::CustomMethod *method, Class target, bool *hooked) {
    if (!target) return CreateMethod(method);

    auto parameters = (uint8_t) method->_parameterTypes.size();

    auto originalMethod = Internal::IterateMethods(target, [method, parameters](IL2CPP::MethodInfo *klassMethod) {
        if (method->_name != klassMethod->name || klassMethod->parameters_count != parameters) return false;
        if (method->_skipTypeMatch) return true;

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

#if UNITY_VER > 174
#define kls klass
#else
#define kls declaring_type
#endif

    if (!originalMethod || originalMethod->kls != target._data) {
        bool isVirtual = originalMethod != nullptr && (originalMethod->flags & 0x0040) == 0x0040;
        auto parent = originalMethod;
        originalMethod = CreateMethod(method);
        if (!hooked || !isVirtual) return originalMethod;
        if (auto vTable = TryFindVirtualMethod(target, parent); vTable != nullptr) {
            method->_origin = parent;
            method->_originalAddress = (void *) parent->methodPointer;
            vTable->methodPtr = (void (*)()) method->_address;
            vTable->method = originalMethod;
            return originalMethod;
        }
    }
#undef kls


    if (method->_isInvokeHook) {
        method->_origin = originalMethod;
        method->_originalAddress = (void *) originalMethod->methodPointer;
        originalMethod->methodPointer = (void(*)()) method->_address;
        if (hooked) *hooked = true;
        return originalMethod;
    }

#ifdef BNM_AUTO_HOOK_DISABLE_VIRTUAL_HOOK
    goto SKIP_NON_VIRTUAL;
#endif

    if ((originalMethod->flags & 0x0040) == 0 || method->_isBasicHook) goto SKIP_NON_VIRTUAL;

    if (auto vTable = TryFindVirtualMethod(target, originalMethod); vTable != nullptr) {
        method->_origin = (BNM::IL2CPP::MethodInfo *) vTable->method;
        method->_originalAddress = (void *) method->_origin->methodPointer;
        vTable->methodPtr = (void(*)()) method->_address;
        if (hooked) *hooked = true;
        return originalMethod;
    }

    SKIP_NON_VIRTUAL:

    method->_origin = originalMethod;
    ::HOOK((void *)originalMethod->methodPointer, method->_address, method->_originalAddress);

    if (hooked) *hooked = true;

    return originalMethod;
}

static IL2CPP::MethodInfo *CreateMethod(MANAGEMENT_STRUCTURES::CustomMethod *method) {
    auto *myInfo = BNM_I2C_NEW(MethodInfo);
    myInfo->methodPointer = (decltype(myInfo->methodPointer)) method->_address;
    myInfo->invoker_method = (decltype(myInfo->invoker_method)) method->_invoker;
    myInfo->parameters_count = method->_parameterTypes.size();

#if UNITY_VER >= 212
    myInfo->virtualMethodPointer = (decltype(myInfo->virtualMethodPointer)) method->_address;
#endif

    auto name = (char *) BNM_malloc(method->_name.size() + 1);
    memcpy((void *)name, method->_name.data(), method->_name.size());
    name[method->_name.size()] = 0;
    myInfo->name = name;

    // Set method flags
    myInfo->flags = 0x0006 | 0x0080; // PUBLIC | HIDE_BY_SIG
    if (method->_isStatic) myInfo->flags |= 0x0010; // |= STATIC
    if (method->_name == Internal::constructorName) myInfo->flags |= 0x0800 | 0x1000; // |= SPECIAL_NAME | RT_SPECIAL_NAME

    // BNM does not support the creation of generic methods
    myInfo->is_generic = false;

    // Set the return type
    auto methodType = method->_returnType.ToClass();
    if (!methodType) methodType = Internal::vmData.Object;
    myInfo->return_type = methodType.GetIl2CppType();

    // Create arguments
    auto argsCount = myInfo->parameters_count;
    if (argsCount) {
        auto &types = method->_parameterTypes;
#if UNITY_VER < 212
        myInfo->parameters = (IL2CPP::ParameterInfo *) BNM_malloc(argsCount * sizeof(IL2CPP::ParameterInfo));

        auto parameter = (IL2CPP::ParameterInfo *)myInfo->parameters;
        for (uint8_t p = 0; p < argsCount; ++p) {
            // The name of parameter does not interest the engine in any way at all
            parameter->name = nullptr;
            parameter->position = p;

            // Set the type anyway
            auto type = p < types.size() ? types[p].ToClass() : Internal::vmData.Object;
            if (!type) type = Internal::vmData.Object;
            parameter->parameter_type = type.GetIl2CppType();
            parameter->token = parameter->parameter_type->attrs | p;

            ++parameter;
        }
#else

        auto parameters = (IL2CPP::Il2CppType **) BNM_malloc(argsCount * sizeof(IL2CPP::Il2CppType *));

        myInfo->parameters = (const IL2CPP::Il2CppType **) parameters;
        for (uint8_t p = 0; p < argsCount; ++p) {
            auto parameter = BNM_I2C_NEW(Il2CppType);

            // Set the type anyway
            auto type = p < types.size() ? types[p].ToClass() : Internal::vmData.Object;
            if (!type) type = Internal::vmData.Object;
            *parameter = *type.GetIl2CppType();

            parameters[p] = parameter;
        }
#endif
    }
    myInfo->rgctx_data = nullptr;
    myInfo->genericMethod = nullptr;
    return myInfo;
}

static void SetupField(IL2CPP::FieldInfo *newField, MANAGEMENT_STRUCTURES::CustomField *field) {
    auto name = field->_name;
    auto len = name.size();
    newField->name = (char *) BNM_malloc(len + 1);
    memcpy((void *)newField->name, name.data(), len);
    ((char *)newField->name)[len] = 0;

    // Copy type
    newField->type = BNM_I2C_NEW(Il2CppType);
    auto fieldType = field->_type.ToClass();
    if (!fieldType) fieldType = Internal::vmData.Object;
    *((IL2CPP::Il2CppType *)newField->type) = *fieldType.GetIl2CppType();

    ((IL2CPP::Il2CppType*)newField->type)->attrs |= 0x0006; // PUBLIC
    newField->token = newField->type->attrs;
}


static void SetupClassOwnerAndParent(IL2CPP::Il2CppClass *target, IL2CPP::Il2CppClass *owner, IL2CPP::Il2CppClass *parent) {
    if (!parent) goto SETUP_OWNER;

    // Set the parent

    if ((target->flags & 0x09000000) == 0x09000000) BNM_free(target->typeHierarchy);
    target->flags |= 0x09000000;

    target->typeHierarchyDepth = parent->typeHierarchyDepth + 1;
    target->typeHierarchy = (IL2CPP::Il2CppClass **) BNM_malloc(target->typeHierarchyDepth * sizeof(IL2CPP::Il2CppClass *));
    memcpy(target->typeHierarchy, parent->typeHierarchy, parent->typeHierarchyDepth * sizeof(IL2CPP::Il2CppClass *));
    target->typeHierarchy[parent->typeHierarchyDepth] = target;
    target->parent = parent;

    SETUP_OWNER:
    if (!owner) return;

    // Set the owner

    auto oldOwner = target->declaringType;
    auto oldInnerList = owner->nestedTypes;

    target->declaringType = owner;

    // Add a class to the new owner's list
    auto newInnerList = (IL2CPP::Il2CppClass **) BNM_malloc(sizeof(IL2CPP::Il2CppClass) * (owner->nested_type_count + 1));
    memcpy(newInnerList, owner->nestedTypes, sizeof(IL2CPP::Il2CppClass) * owner->nested_type_count);
    newInnerList[owner->nested_type_count++] = target;
    owner->nestedTypes = newInnerList;

    // Mark the class to use less memory
    if ((owner->flags & 0x90000000) == 0x90000000) BNM_free(oldInnerList);
    owner->flags |= 0x90000000;

    // Remove a class from the old owner's list
    if (oldOwner) {
        oldInnerList = oldOwner->nestedTypes;
        newInnerList = (IL2CPP::Il2CppClass **) BNM_malloc(sizeof(IL2CPP::Il2CppClass) * (oldOwner->nested_type_count - 1));
        uint8_t skipped = 0;
        for (uint16_t i = 0; i < oldOwner->nested_type_count; ++i) {
            if (skipped == 0) if (skipped = (oldInnerList[i] == target); skipped) continue;
            newInnerList[i - skipped] = oldInnerList[i];
        }
        oldOwner->nestedTypes = newInnerList;
        --oldOwner->nested_type_count;

        // Mark the class to use less memory
        if ((oldOwner->flags & 0x90000000) == 0x90000000) BNM_free(oldInnerList);
        oldOwner->flags |= 0x90000000;
    }
}

static void ProcessInterface(IL2CPP::Il2CppClass *parent, IL2CPP::Il2CppClass *interface, std::vector<IL2CPP::Il2CppClass *> &interfaces) {
    Internal::Class$$Init(interface);
    if (!HasInterface(parent, interface)) interfaces.push_back(interface);
    if (!interface->interfaces_count || interface->interfaces_count == (uint16_t) -1) return;

    for (uint16_t i = 0; i < interface->interfaces_count; ++i) ProcessInterface(parent, interface->implementedInterfaces[i], interfaces);
}

static bool HasInterface(IL2CPP::Il2CppClass *parent, IL2CPP::Il2CppClass *interface) {
    if (!parent || !interface) return false;
    for (uint16_t i = 0; i < parent->interfaces_count; ++i) if (parent->implementedInterfaces[i] == interface) return true;
    if (parent->parent) return HasInterface(parent->parent, interface);
    return false;
}

static void SetupTypes(IL2CPP::Il2CppClass *target) {
    IL2CPP::Il2CppType classType;
    memset(&classType, 0, sizeof(IL2CPP::Il2CppType));
    classType.type = IL2CPP::Il2CppTypeEnum::IL2CPP_TYPE_CLASS;
    classType.attrs = 0x1; // Public
    classType.pinned = 0;
    classType.byref = 0;
    classType.num_mods = 31;
    classType.data.dummy = target;

#if UNITY_VER > 174
    target->this_arg = target->byval_arg = classType;
    target->this_arg.byref = 1;
#else
    target->byval_arg = BNM_I2C_NEW(Il2CppType);
    *((IL2CPP::Il2CppType *)target->byval_arg) = classType;
    classType.byref = 1;
    target->this_arg = BNM_I2C_NEW(Il2CppType);
    *((IL2CPP::Il2CppType *)target->this_arg) = classType;
#endif
}

#undef BNM_I2C_NEW
#endif
