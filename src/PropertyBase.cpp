#include <BNM/PropertyBase.hpp>
#include <BNM/DebugMessages.hpp>

using namespace BNM;


PropertyBase::PropertyBase(const IL2CPP::PropertyInfo *info) {
    if (!info) return;
    _hasGetter = _hasSetter = false;
    if (info->get && info->get->methodPointer) {
        _hasGetter = true;
        _getter = info->get;
    }
    if (info->set && info->set->methodPointer) {
        _hasSetter = true;
        _setter = info->set;
    }
}

PropertyBase::PropertyBase(const MethodBase &newGetter, const MethodBase &newSetter)  {
    _hasGetter = _hasSetter = false;
    if (newGetter) {
        _hasGetter = true;
        _getter = newGetter;
    }
    if (newSetter) {
        _hasSetter = true;
        _setter = newSetter;
    }
}

PropertyBase &PropertyBase::SetInstance(IL2CPP::Il2CppObject *val) {
    if (_hasGetter) _getter.SetInstance(val);
    if (_hasSetter) _setter.SetInstance(val);
    return *this;
}