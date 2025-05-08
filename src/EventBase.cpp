#include <BNM/EventBase.hpp>
#include <BNM/DebugMessages.hpp>

using namespace BNM;

EventBase::EventBase(const IL2CPP::EventInfo *info) {
    if (!info) return;
    _hasAdd = _hasRemove = _hasRaise = false;
    if (info->add && info->add->methodPointer) {
        _hasAdd = true;
        _add = info->add;
    }
    if (info->remove && info->remove->methodPointer) {
        _hasRemove = true;
        _remove = info->remove;
    }
    if (info->raise && info->raise->methodPointer) {
        _hasRaise = true;
        _raise = info->raise;
    }
}

EventBase &EventBase::SetInstance(IL2CPP::Il2CppObject *instance) {
    if (_hasAdd) _add.SetInstance(instance);
    if (_hasRemove) _remove.SetInstance(instance);
    if (_hasRaise) _raise.SetInstance(instance);
    return *this;
}