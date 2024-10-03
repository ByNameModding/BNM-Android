#include <BNM/Exceptions.hpp>
#include <Internals.hpp>

BNM::Exception BNM::TryInvoke(const std::function<void()> &func) {
    BNM::IL2CPP::Il2CppType type;
    type.type = BNM::IL2CPP::IL2CPP_TYPE_VOID;
    BNM::IL2CPP::Il2CppClass klass;
#if UNITY_VER <= 204
    klass.valuetype = 0;
#endif
    BNM::IL2CPP::MethodInfo info;
    for (size_t i = 0; i < sizeof(info); ++i) *(char *)&info = 0;
    info.return_type = &type;
#if UNITY_VER <= 174
    info.declaring_type = &klass;
#else
    info.klass = &klass;
#endif

    info.methodPointer = (decltype(info.methodPointer)) &func;
#if UNITY_VER < 171
    info.invoker_method = (BNM::IL2CPP::InvokerMethod) +[](BNM::IL2CPP::MethodInfo *info) -> void { ((*(std::function<void()>*)info->methodPointer))(); };
#else
    info.invoker_method = (BNM::IL2CPP::InvokerMethod) +[](std::function<void()> *func) -> void { (*func)(); };
#endif
    BNM::IL2CPP::Il2CppException *exception = nullptr;
    Internal::il2cppMethods.il2cpp_runtime_invoke(&info, nullptr, nullptr, &exception);
    return exception;
}