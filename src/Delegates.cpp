#include <BNM/Delegates.hpp>

void BNM::MulticastDelegateBase::Add(BNM::DelegateBase *delegate)  {
    auto delegates = (Structures::Mono::Array<DelegateBase *> *) this->delegates;
    auto arr = BNM::Class(delegates->klass->element_class).NewArray<DelegateBase *>(this->delegates->max_length + 1);
    arr->CopyFrom(delegates->m_Items, delegates->capacity);
    arr->m_Items[delegates->capacity] = delegate;
    this->delegates = (decltype(this->delegates)) arr;
}

void BNM::MulticastDelegateBase::Remove(BNM::DelegateBase *delegate)  {
    auto delegates = (Structures::Mono::Array<DelegateBase *> *) this->delegates;
    IL2CPP::il2cpp_array_size_t index = 0;
    bool found = false;
    for (IL2CPP::il2cpp_array_size_t i = 0; i < delegates->capacity; ++i) {
        if (delegates->m_Items[i] != delegate) continue;

        found = true;
        index = i;
        break;
    }
    if (!found) return;
    auto arr = BNM::Class(delegates->klass->element_class).NewArray<DelegateBase *>(this->delegates->max_length - 1);
    memmove(delegates->m_Items + index, delegates->m_Items + index + 1, (delegates->capacity - index - 1) * sizeof(void *));
    arr->CopyFrom(delegates->m_Items, delegates->capacity - 1);
    this->delegates = (decltype(this->delegates)) arr;
}
