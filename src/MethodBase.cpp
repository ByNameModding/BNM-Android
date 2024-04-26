#include <BNM/MethodBase.hpp>
#include <BNM/DebugMessages.hpp>
#include "Internals.hpp"

using namespace BNM;


MethodBase::MethodBase(const IL2CPP::MethodInfo *info)  {
    _init = (BNM::CheckObj(info) != nullptr);
    if (_init) {
        _isStatic = (info->flags & 0x0010) == 0x0010;
        _isVirtual = info->slot != 65535;
        _data = (decltype(_data)) info;
    }
}

MethodBase::MethodBase(const IL2CPP::Il2CppReflectionMethod *reflectionMethod) {
    _init = (BNM::CheckObj(reflectionMethod) != nullptr) && (BNM::CheckObj(reflectionMethod->method) != nullptr);
    auto info = reflectionMethod->method;
    if (_init) {
        _isStatic = (info->flags & 0x0010) == 0x0010;
        _isVirtual = info->slot != 65535;
        _data = (decltype(_data)) info;
    }
}

MethodBase &MethodBase::SetInstance(IL2CPP::Il2CppObject *val)  {
    if (!_init) return *this;
    if (_isStatic) {
        BNM_LOG_WARN(DBG_BNM_MSG_MethodBase_SetInstance_Warn, str().c_str());
        return *this;
    }
    _instance = val;
    return *this;
}

MethodBase MethodBase::GetGeneric(const std::initializer_list<CompileTimeClass> &templateTypes) const {
    BNM_LOG_WARN_IF(!_data->is_generic, DBG_BNM_MSG_MethodBase_GetGeneric_Warn, str().c_str());
    if (!_data->is_generic) return {};
    return Internal::TryMakeGenericMethod(*this, templateTypes);
}

MethodBase MethodBase::Virtualize() const {
    if (!_init || _isStatic) return {};
    if (!BNM::CheckObj(_instance)) {
        BNM_LOG_WARN(DBG_BNM_MSG_MethodBase_Virtualize_Warn, str().c_str());
        return {};
    }

    auto klass = _instance->klass;
    uint16_t i = 0;
    NEXT:
    for (; i < klass->vtable_count; ++i) {
        auto &vTable = klass->vtable[i];
        auto count = vTable.method->parameters_count;

        if (strcmp(vTable.method->name, _data->name) || count != _data->parameters_count) continue;

        for (uint8_t p = 0; p < count; ++p) {
#if UNITY_VER < 212
            auto type = (vTable.method->parameters + p)->parameter_type;
            auto type2 = (_data->parameters + p)->parameter_type;
#else
            auto type = vTable.method->parameters[p];
            auto type2 = _data->parameters[p];
#endif

            if (Class(type).GetClass() != Class(type2).GetClass()) goto NEXT;
        }
        return MethodBase(vTable.method)[_instance];
    }
    return {};
}