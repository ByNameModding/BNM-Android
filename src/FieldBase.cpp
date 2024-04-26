#include <BNM/FieldBase.hpp>
#include <BNM/DebugMessages.hpp>
#include "Internals.hpp"

using namespace BNM;

// Для статических полей потоков
namespace BNM::PRIVATE_FieldUtils {
    void GetStaticValue(IL2CPP::FieldInfo *info, void *value) { return Internal::il2cppMethods.il2cpp_field_static_get_value(info, value); }

    void SetStaticValue(IL2CPP::FieldInfo *info, void *value) { return Internal::il2cppMethods.il2cpp_field_static_set_value(info, value); }
}

bool CheckIsFieldStatic(IL2CPP::FieldInfo *field) {
    if (!field || !field->type) return false;
    return (field->type->attrs & 0x0010) != 0 && field->offset != -1 && (field->type->attrs & 0x0040) == 0;
}

FieldBase::FieldBase(IL2CPP::FieldInfo *info) {
    _init = info != nullptr;
    if (_init) {
        _isStatic = CheckIsFieldStatic(info);
        _data = info;
        _isThreadStatic = _data->offset == -1;
        _isInStruct = Class(info->parent).GetIl2CppType()->type == IL2CPP::IL2CPP_TYPE_VALUETYPE;
    }
}

FieldBase &FieldBase::SetInstance(IL2CPP::Il2CppObject *val)  {
    if (_init && _isStatic) {
        BNM_LOG_WARN(DBG_BNM_MSG_FieldBase_SetInstance_Warn, str().c_str());
        return *this;
    }
    _init = val && _data != nullptr;
    _instance = val;
    return *this;
}

void *FieldBase::GetFieldPointer() const {
    if (!_init) return nullptr;
    if (!_isStatic && !CheckObj(_instance)) {
        BNM_LOG_ERR(DBG_BNM_MSG_FieldBase_GetFieldPointer_Error_instance_dead_instance, str().c_str());
        return nullptr;
    } else if (_isStatic && !CheckObj(_data->parent)) {
        BNM_LOG_ERR(DBG_BNM_MSG_FieldBase_GetFieldPointer_Error_static_dead_parent, str().c_str());
        return nullptr;
    } else if (_isThreadStatic) {
        BNM_LOG_ERR(DBG_BNM_MSG_FieldBase_GetFieldPointer_Error_thread_static_unsupported, str().c_str());
        return nullptr;
    }
    if (_isStatic) return (void *) ((BNM_PTR) _data->parent->static_fields + _data->offset);
    return (void *) ((BNM_PTR) _instance + _data->offset - (_isInStruct ? sizeof(IL2CPP::Il2CppObject) : 0x0));
}
