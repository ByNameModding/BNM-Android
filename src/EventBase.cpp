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

EventBase::EventBase(const BNM::MethodBase &newAdd, const BNM::MethodBase &newRemove, const BNM::MethodBase &newRaise) {
    _hasAdd = _hasRemove = _hasRaise = false;
    if (newAdd) {
        _hasAdd = true;
        _add = newAdd;
    }
    if (newRemove) {
        _hasRemove = true;
        _remove = newRemove;
    }
    if (newRaise) {
        _hasRaise = true;
        _raise = newRaise;
    }
}

EventBase &EventBase::SetInstance(IL2CPP::Il2CppObject *val) {
    if (_hasAdd) _add.SetInstance(val);
    if (_hasRemove) _remove.SetInstance(val);
    if (_hasRaise) _raise.SetInstance(val);
    return *this;
}