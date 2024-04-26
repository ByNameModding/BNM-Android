#include <BNM/UserSettings/GlobalSettings.hpp>
#include <BNM/Utils.hpp>
#include <BNM/Method.hpp>
#include "Internals.hpp"

using namespace BNM;

Structures::Mono::String *BNM::CreateMonoString(const char *str) {
    return Internal::il2cppMethods.il2cpp_string_new(str);
}

Structures::Mono::String *BNM::CreateMonoString(const std::string_view &str) {
    return CreateMonoString(str.data());
}

void *BNM::GetExternMethod(const std::string_view &str) {
    auto c_str = str.data();
    auto ret = Internal::il2cppMethods.il2cpp_resolve_icall(c_str);
    BNM_LOG_WARN_IF(!ret, DBG_BNM_MSG_GetExternMethod_Warn, c_str);
    return ret;
}

bool BNM::IsLoaded() {
    return Internal::state;
}

void *BNM::GetIl2CppLibraryHandle() {
    return Internal::il2cppLibraryHandle;
}

bool BNM::InvokeHookImpl(IL2CPP::MethodInfo *m, void *newMet, void **oldMet) {
    if (!m) return false;
    if (oldMet) *oldMet = (void *) m->methodPointer;
    m->methodPointer = (IL2CPP::Il2CppMethodPointer) newMet;
    return true;
}

bool BNM::VirtualHookImpl(BNM::Class targetClass, IL2CPP::MethodInfo *m, void *newMet, void **oldMet) {
    if (!m || !targetClass) return false;
    uint16_t i = 0;
    NEXT:
    for (; i < targetClass._data->vtable_count; ++i) {
        auto &vTable = targetClass._data->vtable[i];
        auto count = vTable.method->parameters_count;

        if (strcmp(vTable.method->name, m->name) != 0 || count != m->parameters_count) continue;

        for (uint8_t p = 0; p < count; ++p) {
#if UNITY_VER < 212
            auto type = (vTable.method->parameters + p)->parameter_type;
            auto type2 = (m->parameters + p)->parameter_type;
#else
            auto type = vTable.method->parameters[p];
            auto type2 = m->parameters[p];
#endif
            if (Class(type).GetClass() != Class(type2).GetClass()) goto NEXT;

        }

        if (oldMet) *oldMet = (void *) vTable.methodPtr;
        vTable.methodPtr = (void(*)()) newMet;
        return true;

    }
    return false;
}

template<> bool BNM::IsA<IL2CPP::Il2CppObject *>(IL2CPP::Il2CppObject *object, IL2CPP::Il2CppClass *klass) {
    if (!object || !klass) return false;
    for (auto cls = object->klass; cls; cls = cls->parent) if (cls == klass) return true;
    return false;
}

IL2CPP::Il2CppClass *Internal::TryGetClassInImage(const IL2CPP::Il2CppImage *image, const std::string_view &_namespace, const std::string_view &_name) {
    if (!image) return nullptr;

#ifdef BNM_CLASSES_MANAGEMENT
    // Получить BNM-классы
    if (image->nameToClassHashTable == (decltype(image->nameToClassHashTable))-0x424e4d) goto NEW_CLASSES;
#endif

    if (Internal::il2cppMethods.il2cpp_image_get_class) {
        size_t typeCount = image->typeCount;

        for (size_t i = 0; i < typeCount; ++i) {
            auto cls = il2cppMethods.il2cpp_image_get_class(image, i);
            if (strcmp(OBFUSCATE_BNM("<Module>"), cls->name) == 0 || cls->declaringType) continue;
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
    ClassesManagement::BNMClassesMap.forEachByImage(image, [&_namespace, &_name, &result](IL2CPP::Il2CppClass *BNM_class) -> bool {
        if (_namespace != BNM_class->namespaze || _name != BNM_class->name) return false;

        result = BNM_class;
        return true;
    });
#endif

    return result;
}
Class Internal::TryMakeGenericClass(Class genericType, const std::vector<CompileTimeClass> &templateTypes) {
    if (!vmData.RuntimeType$$MakeGenericType) return {};
    auto monoType = genericType.GetMonoType();
    auto monoGenericsList = Structures::Mono::Array<MonoType *>::Create(templateTypes.size());
    for (IL2CPP::il2cpp_array_size_t i = 0; i < (IL2CPP::il2cpp_array_size_t) templateTypes.size(); ++i)
        (*monoGenericsList)[i] = templateTypes[i].ToClass().GetMonoType();
    Class typedGenericType = vmData.RuntimeType$$MakeGenericType(monoType, monoGenericsList);
    monoGenericsList->Destroy();
    return typedGenericType;
}

MethodBase Internal::TryMakeGenericMethod(const MethodBase &genericMethod, const std::vector<CompileTimeClass> &templateTypes) {
    if (!vmData.RuntimeMethodInfo$$MakeGenericMethod_impl || !genericMethod.GetInfo()->is_generic) return {};
    IL2CPP::Il2CppReflectionMethod reflectionMethod;
    reflectionMethod.method = genericMethod.GetInfo();
    auto monoGenericsList = Structures::Mono::Array<MonoType *>::Create(templateTypes.size());
    for (IL2CPP::il2cpp_array_size_t i = 0; i < (IL2CPP::il2cpp_array_size_t) templateTypes.size(); ++i) (*monoGenericsList)[i] = templateTypes[i].ToClass().GetMonoType();

    MethodBase typedGenericMethod = vmData.RuntimeMethodInfo$$MakeGenericMethod_impl[(void *)&reflectionMethod](monoGenericsList)->method;

    monoGenericsList->Destroy();

    return typedGenericMethod;
}

Class Internal::GetPointer(Class target) {
    if (!vmData.RuntimeType$$MakePointerType) return {};
    return vmData.RuntimeType$$MakePointerType(target.GetMonoType());
}

Class Internal::GetReference(Class target) {
    if (!vmData.RuntimeType$$make_byref_type) return {};
    return vmData.RuntimeType$$make_byref_type[(void *)target.GetMonoType()]();
}

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
            for (auto type : genericInfo->_types) BNM_LOG_ERR("\t\t%s", type.ToClass().str().data());
            BNM_LOG_ERR("\t%s", tmp._loadedClass.str().data());
        } break;
        case CompileTimeClass::_BaseType::MaxCount: break;
    }
}

namespace CompileTimeClassProcessors {
    typedef void (*ProcessorType)(CompileTimeClass &target, CompileTimeClass::_BaseInfo *info);
    extern ProcessorType processors[(uint8_t) CompileTimeClass::_BaseType::MaxCount];
}

void BNM::Utils::LogCompileTimeClass(const BNM::CompileTimeClass &compileTimeClass) {
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