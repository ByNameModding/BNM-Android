#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "Method.hpp"
#include "BasicMonoStructures.hpp"
#include "Utils.hpp"

namespace BNM {

    template<typename Ret>
    struct Delegate;
    template<typename Ret>
    struct MulticastDelegate;

    struct DelegateBase : IL2CPP::Il2CppDelegate {
        inline constexpr DelegateBase() = default;

        inline IL2CPP::Il2CppObject* GetInstance() {
            return CheckForNull(this) ? target : nullptr;
        }
        inline MethodBase GetMethod() {
            if (!CheckForNull(this)) return {};
            auto method = MethodBase(this->method);
            auto instance = GetInstance();
            if (instance) method.SetInstance(instance);
            return method;
        }

        [[nodiscard]] inline bool Initialized() const noexcept { return CheckForNull(this); }
        inline operator bool() noexcept { return Initialized(); }
        inline operator bool() const noexcept { return Initialized(); }
        template<typename NewRet>
        inline Delegate<NewRet> &cast() { return (Delegate<NewRet> &) *this; }
    };

    struct MulticastDelegateBase : IL2CPP::Il2CppMulticastDelegate {
        inline constexpr MulticastDelegateBase() = default;

        inline std::vector<MethodBase> GetMethods() {
            if (!CheckForNull(this)) return {};

            auto delegates = (Structures::Mono::Array<DelegateBase *> *) this->delegates;
            if (!delegates) return {((DelegateBase *)this)->GetMethod()};

            std::vector<MethodBase> ret{};
            ret.reserve(delegates->capacity);
            for (IL2CPP::il2cpp_array_size_t i = 0; i < delegates->capacity; ++i) ret.push_back(delegates->At(i)->GetMethod());
            return std::move(ret);
        }

        void Add(DelegateBase *delegate);
        void Remove(DelegateBase *delegate);

        inline void operator +=(DelegateBase *base) { return Add(base); }
        inline void operator -=(DelegateBase *base) { return Remove(base); }

        template<typename NewRet>
        inline MulticastDelegate<NewRet> &cast() { return (MulticastDelegate<NewRet> &) *this; }
    };

    template<typename Ret>
    struct Delegate : public DelegateBase {
        template<typename ...Params>
        inline Ret Invoke(Params ...params) {
            return GetMethod().template cast<Ret>().Call(params...);
        }
    };

    template<typename Ret>
    struct MulticastDelegate : public MulticastDelegateBase {
        template<typename ...Params>
        inline Ret Invoke(Params ...params) {
            if (!CheckForNull(this)) return BNM::PRIVATE_INTERNAL::ReturnEmpty<Ret>();

            auto delegates = (Structures::Mono::Array<DelegateBase *> *) this->delegates;
            if (!delegates) return ((Delegate<Ret>*)this)->Invoke(params...);

            for (IL2CPP::il2cpp_array_size_t i = 0; i < delegates->capacity - 1; ++i) delegates->At(i)->GetMethod().template cast<Ret>().Call(params...);
            return delegates->At(delegates->capacity - 1)->GetMethod().template cast<Ret>().Call(params...);

        }
    };
}