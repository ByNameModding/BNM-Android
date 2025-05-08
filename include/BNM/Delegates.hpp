#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "Method.hpp"
#include "BasicMonoStructures.hpp"
#include "Utils.hpp"

// NOLINTBEGIN
namespace BNM {

    template<typename Ret>
    struct Delegate;
    template<typename Ret>
    struct MulticastDelegate;

    /**
        @brief Wrapper of Il2CppDelegate
    */
    struct DelegateBase : IL2CPP::Il2CppDelegate {
        inline constexpr DelegateBase() = default;

        /**
            @brief Get instance of delegate.
            @return Instance if delegate isn't null, otherwise null.
        */
        inline IL2CPP::Il2CppObject* GetInstance() const {
            return CheckForNull(this) ? target : nullptr;
        }

        /**
            @brief Get method of delegate.
            @return MethodBase if delegate isn't null, otherwise empty MethodBase.
        */
        inline MethodBase GetMethod() const {
            if (!CheckForNull(this)) return {};
            auto method = MethodBase(this->method);
            auto instance = GetInstance();
            if (instance) method.SetInstance(instance);
            return method;
        }

        /**
            @brief Check if delegate is valid.
        */
        [[nodiscard]] inline bool IsValid() const noexcept { return CheckForNull(this); }

        /**
            @brief Check if delegate is valid.
        */
        inline operator bool() const noexcept { return IsValid(); }

        /**
            @brief Cast delegate to be able to invoke it.
        */
        template<typename NewRet>
        inline Delegate<NewRet> &cast() const { return (Delegate<NewRet> &) *this; }
    };

    /**
        @brief Wrapper of Il2CppMulticastDelegate
    */
    struct MulticastDelegateBase : IL2CPP::Il2CppMulticastDelegate {
        inline constexpr MulticastDelegateBase() = default;

        /**
            @brief Get methods of delegate.
            @return Vector of MethodBase if delegate isn't null, otherwise empty vector.
        */
        inline std::vector<MethodBase> GetMethods() const {
            if (!CheckForNull(this)) return {};

            auto delegates = (Structures::Mono::Array<DelegateBase *> *) this->delegates;
            if (!delegates) return {((DelegateBase *)this)->GetMethod()};

            std::vector<MethodBase> ret{};
            ret.reserve(delegates->capacity);
            for (IL2CPP::il2cpp_array_size_t i = 0; i < delegates->capacity; ++i) ret.push_back(delegates->At(i)->GetMethod());
            return std::move(ret);
        }

        /**
            @brief Add delegate.
        */
        void Add(DelegateBase *delegate);

        /**
            @brief Remove delegate.
        */
        void Remove(DelegateBase *delegate);

        inline void operator +=(DelegateBase *base) { return Add(base); }
        inline void operator -=(DelegateBase *base) { return Remove(base); }

        /**
            @brief Cast delegate to be able to invoke it.
        */
        template<typename NewRet>
        inline MulticastDelegate<NewRet> &cast() const { return (MulticastDelegate<NewRet> &) *this; }
    };

    /**
        @brief Typed wrapper of Il2CppDelegate
        @tparam Ret Return type
    */
    template<typename Ret>
    struct Delegate : public DelegateBase {

        /**
            @brief Invoke delegate.
            @tparam Ret Return type
            @tparam Parameters Delegate parameter types
            @param parameters Delegate parameters
        */
        template<typename ...Parameters>
        inline Ret Invoke(Parameters ...parameters) {
            return GetMethod().template cast<Ret>().Call(parameters...);
        }

        template<typename ...Parameters>
        inline Ret operator()(Parameters ...parameters) { return Invoke(parameters...); }
    };

    /**
        @brief Typed wrapper of Il2CppMulticastDelegate
        @tparam Ret Return type
    */
    template<typename Ret>
    struct MulticastDelegate : public MulticastDelegateBase {

        /**
            @brief Invoke delegate.
            @tparam Ret Return type
            @tparam Parameters Delegate parameter types
            @param parameters Delegate parameters
        */
        template<typename ...Parameters>
        inline Ret Invoke(Parameters ...parameters) {
            if (!CheckForNull(this)) return BNM::PRIVATE_INTERNAL::ReturnEmpty<Ret>();

            auto delegates = (Structures::Mono::Array<DelegateBase *> *) this->delegates;
            if (!delegates) return ((Delegate<Ret>*)this)->Invoke(parameters...);

            for (IL2CPP::il2cpp_array_size_t i = 0; i < delegates->capacity - 1; ++i) delegates->At(i)->GetMethod().template cast<Ret>().Call(parameters...);
            return delegates->At(delegates->capacity - 1)->GetMethod().template cast<Ret>().Call(parameters...);
        }

        template<typename ...Parameters>
        inline Ret operator()(Parameters ...parameters) { return Invoke(parameters...); }
    };

    namespace Structures::Mono {
        /**
           @brief System.Action type implementation
        */
        template <typename ...Parameters>
        struct Action : public MulticastDelegate<void> {};
    }
}
// NOLINTEND