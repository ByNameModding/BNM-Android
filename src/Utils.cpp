#include <BNM/UserSettings/GlobalSettings.hpp>
#include <BNM/Utils.hpp>
#include <BNM/Method.hpp>
#include <Internals.hpp>

using namespace BNM;

Structures::Mono::String *BNM::CreateMonoString(const std::string_view &str) {
    return Internal::il2cppMethods.il2cpp_string_new(str.data());
}

void *BNM::GetExternMethod(const std::string_view &str) {
    auto c_str = str.data();
    auto ret = Internal::il2cppMethods.il2cpp_resolve_icall(c_str);
    BNM_LOG_WARN_IF(!ret, DBG_BNM_MSG_GetExternMethod_Warn, c_str);
    return ret;
}

bool BNM::IsLoaded() {
    return Internal::states.state;
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

template<> bool BNM::IsA<IL2CPP::Il2CppObject *>(IL2CPP::Il2CppObject *object, IL2CPP::Il2CppClass *_class) {
    if (!object || !_class) return false;
    for (auto cls = object->klass; cls; cls = cls->parent) if (cls == _class) return true;
    return false;
}

#ifdef BNM_DEBUG

static const char *CompileTimeClassModifiers[] = {
        "None",
        "Array",
        "Pointer",
        "Reference"
};

static void LogCompileTimeClassInfo(BNM::CompileTimeClass::_BaseInfo *info, const BNM::CompileTimeClass &tmp) {
    switch (info->_baseType) {
        case BNM::CompileTimeClass::_BaseType::None:
            BNM_LOG_ERR("\tNone");
            break;
        case BNM::CompileTimeClass::_BaseType::Class: {
            auto classInfo = (CompileTimeClass::_ClassInfo *) info;
            BNM_LOG_ERR("\tClass( imageName: \"%s\", namespace: \"%s\", name: \"%s\") - %s", classInfo->_imageName, classInfo->_namespace, classInfo->_name, tmp._loadedClass.str().data());
        } break;
        case CompileTimeClass::_BaseType::Inner: {
            auto innerInfo = (CompileTimeClass::_InnerInfo *) info;
            BNM_LOG_ERR("\tClass( name: \"%s\") - %s", innerInfo->_name, tmp._loadedClass.str().data());
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

bool BNM::AttachIl2Cpp() {
    if (CurrentIl2CppThread()) return false;
    Internal::il2cppMethods.il2cpp_thread_attach(Internal::il2cppMethods.il2cpp_domain_get());
    return true;
}

IL2CPP::Il2CppThread *BNM::CurrentIl2CppThread() {
    return Internal::il2cppMethods.il2cpp_thread_current(Internal::il2cppMethods.il2cpp_domain_get());
}

void BNM::DetachIl2Cpp() {
    auto thread = BNM::CurrentIl2CppThread();
    if (!thread) return;
    Internal::il2cppMethods.il2cpp_thread_detach(thread);
}