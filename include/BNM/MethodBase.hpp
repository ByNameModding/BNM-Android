#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "Class.hpp"

// NOLINTBEGIN
namespace BNM {

#pragma pack(push, 1)

    template <typename Ret> struct Method;

    /**
        @brief Class base for working with il2cpp methods.

        This class provides fields and some basic functions to allow to call methods.
    */
    struct MethodBase {
        /**
            @brief Create empty method base.
        */
        inline constexpr MethodBase() = default;

        /**
            @brief Copy method.
            @param other Other method
        */
        inline MethodBase(const MethodBase &other) = default;

        /**
            @brief Create method from il2cpp method.
            @param info Il2cpp method
        */
        MethodBase(const IL2CPP::MethodInfo *info);

        /**
            @brief Create method from il2cpp reflection method.
            @param reflectionMethod Il2cpp reflection method
        */
        MethodBase(const IL2CPP::Il2CppReflectionMethod *reflectionMethod);

        /**
            @brief Set method instance if it's non-static.
            @param instance Instance
            @return Reference to current MethodBase
        */
        MethodBase &SetInstance(IL2CPP::Il2CppObject *val);

        /**
            @brief Get MethodInfo.
            @return MethodInfo
        */
        inline IL2CPP::MethodInfo *GetInfo() const { return _data; }

        /**
            @brief Get method offset.
            @return Method offset if method is valid, otherwise zero.
        */
        inline BNM_PTR GetOffset() const { return _data ? (BNM_PTR) _data->methodPointer : 0; }

        /**
            @brief Get typed version of current generic method.

            @return Typed method if method is valid and generic, otherwise empty method.
        */
        MethodBase GetGeneric(const std::initializer_list<CompileTimeClass> &templateTypes) const;

        /**
            @brief Get overridden method of current virtual method.
            @return Overridden method if method is valid and virtual, otherwise empty method.
        */
        MethodBase GetOverride() const;

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current MethodBase
        */
        inline MethodBase &operator[](void *instance) { SetInstance((IL2CPP::Il2CppObject *) instance); return *this;}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current MethodBase
        */
        inline MethodBase &operator[](IL2CPP::Il2CppObject *instance) { SetInstance(instance); return *this;}

        /**
            @brief Operator for setting instance.
            @param instance Instance
            @return Reference to current MethodBase
        */
        inline MethodBase &operator[](UnityEngine::Object *instance) { SetInstance((IL2CPP::Il2CppObject *) instance); return *this;}

        /**
            @brief Check if method is valid.
            @return State of method
        */
        [[nodiscard]] inline bool IsValid() const noexcept { return _data; }

#ifdef BNM_ALLOW_STR_METHODS

        /**
            @brief Get full method name.

            Returns string with full method name: ClassFullName.(method name){arguments count: ...} (static)/nothing

            @return Method's full name or "Dead method".
        */
        inline std::string str() const {
#if UNITY_VER > 174
#define kls klass
#else
#define kls declaring_type
#endif
            if (!_data) return BNM_OBFUSCATE(DBG_BNM_MSG_MethodBase_str_nullptr);
            return Class(_data->return_type).str() + BNM_OBFUSCATE(" ") +
                Class(_data->kls).str() + BNM_OBFUSCATE(".(") +
                    _data->name + BNM_OBFUSCATE("){" DBG_BNM_MSG_MethodBase_str_args_count ": ") +
                std::to_string(_data->parameters_count) + BNM_OBFUSCATE("}") +
                (_isStatic ? BNM_OBFUSCATE("(" DBG_BNM_MSG_MethodBase_str_static ")") : BNM_OBFUSCATE(""));
#undef kls
        }
#endif

        /**
           @brief Cast method to be able to call it.
        */
        template<typename NewType> inline Method<NewType> &cast() const { return (Method<NewType> &)*this; }

        IL2CPP::MethodInfo *_data{};
        IL2CPP::Il2CppObject *_instance{};
        uint8_t _isStatic : 1 = false, _isVirtual : 1 = false;
    };

#pragma pack(pop)

    /**
        @brief Hook method by changing MethodInfo

        This way of hooking will work only if method called using il2cpp API.
        There are not many such methods in Unity: All unity messages (like Start, Update etc.), some of system actions.
        Primary usage is Unity messages, because this way of hooking don't add any overhead, that basic hooks do.

        @param info Target method info
        @param newMet Pointer to replacement method
        @param oldMet Pointer where original method pointer will be stored. Can be null

        @return True if method valid
    */
    bool InvokeHookImpl(IL2CPP::MethodInfo *info, void *newMet, void **oldMet);

    /**
        @brief Hook method by changing MethodInfo

        Alias for BNM::InvokeHookImpl

        @param targetMethod Target method
        @param newMet Replacement method
        @param oldMet Pointer where original method pointer will be stored. Can be null

        @return True if method valid
    */
    template<typename T_NEW, typename T_OLD>
    bool InvokeHook(const BNM::MethodBase &targetMethod, T_NEW newMet, T_OLD &oldMet) {
        if (targetMethod.IsValid()) return InvokeHookImpl(targetMethod._data, (void *)newMet, (void **)&oldMet);
        return false;
    }

    /**
        @brief Hook method by changing MethodInfo

        Alias for BNM::InvokeHookImpl

        @param targetMethod Target method
        @param newMet Replacement method
        @param oldMet Pointer where original method pointer will be stored. Can be null

        @return True if method valid
    */
    template<typename T_NEW, typename T_OLD>
    bool InvokeHook(const BNM::MethodBase &targetMethod, T_NEW newMet, T_OLD &&oldMet) {
        if (targetMethod.IsValid()) return InvokeHookImpl(targetMethod._data, (void *)newMet, (void **)&oldMet);
        return false;
    }

    /**
        @brief Hook method by changing MethodInfo

        Alias for BNM::InvokeHookImpl

        @param info Target method info
        @param newMet Replacement method
        @param oldMet Pointer where original method pointer will be stored. Can be null

        @return True if method valid
    */
    template<typename T_NEW, typename T_OLD>
    bool InvokeHook(IL2CPP::MethodInfo *info, T_NEW newMet, T_OLD &oldMet) { return InvokeHookImpl(info, (void *)newMet, (void **)&oldMet); }

    /**
        @brief Hook method by changing MethodInfo

        Alias for BNM::InvokeHookImpl

        @param info Target method info
        @param newMet Replacement method
        @param oldMet Pointer where original method pointer will be stored. Can be null

        @return True if method valid
    */
    template<typename T_NEW, typename T_OLD>
    bool InvokeHook(IL2CPP::MethodInfo *info, T_NEW newMet, T_OLD &&oldMet) { return InvokeHookImpl(info, (void *)newMet, (void **)&oldMet); }

    /**
        @brief Hook method by changing class virtual table

        This way of hooking will work only if virtual method called from some method.
        This hook won't be triggered by overridden methods, because that methods call overridden method directly, without class virtual table.

        @param targetClass Target class
        @param info Virtual method info
        @param newMet Pointer to replacement method
        @param oldMet Pointer where original method pointer will be stored. Can be null

        @return True if class and method are valid and method got hooked
    */
    bool VirtualHookImpl(BNM::Class targetClass, IL2CPP::MethodInfo *info, void *newMet, void **oldMet);

    /**
        @brief Hook method by changing class virtual table

        Alias for BNM::VirtualHookImpl

        @param targetClass Target class
        @param targetMethod Virtual method
        @param newMet Replacement method
        @param oldMet Pointer where original method pointer will be stored. Can be null

        @return True if method valid
    */
    template<typename T_NEW, typename T_OLD>
    bool VirtualHook(BNM::Class targetClass, const BNM::MethodBase &targetMethod, T_NEW newMet, T_OLD &oldMet) {
        if (targetClass && targetMethod.IsValid()) return VirtualHookImpl(targetClass, targetMethod._data, (void *)newMet, (void **)&oldMet);
        return false;
    }

    /**
        @brief Hook method by changing class virtual table

        Alias for BNM::VirtualHookImpl

        @param targetClass Target class
        @param targetMethod Virtual method
        @param newMet Replacement method
        @param oldMet Pointer where original method pointer will be stored. Can be null

        @return True if method valid
    */
    template<typename T_NEW, typename T_OLD>
    bool VirtualHook(BNM::Class targetClass, const BNM::MethodBase &targetMethod, T_NEW newMet, T_OLD &&oldMet) {
        if (targetClass && targetMethod.IsValid()) return VirtualHookImpl(targetClass, targetMethod._data, (void *)newMet, (void **)&oldMet);
        return false;
    }

    /**
        @brief Hook method by changing class virtual table

        Alias for BNM::VirtualHookImpl

        @param targetClass Target class
        @param info Virtual method info
        @param newMet Replacement method
        @param oldMet Pointer where original method pointer will be stored. Can be null

        @return True if method valid
    */
    template<typename T_NEW, typename T_OLD>
    bool VirtualHook(BNM::Class targetClass, IL2CPP::MethodInfo *info, T_NEW newMet, T_OLD &oldMet) {
        return VirtualHookImpl(targetClass, info, (void *)newMet, (void **)&oldMet);
    }

    /**
        @brief Hook method by changing class virtual table

        Alias for BNM::VirtualHookImpl

        @param targetClass Target class
        @param info Virtual method info
        @param newMet Replacement method
        @param oldMet Pointer where original method pointer will be stored. Can be null

        @return True if method valid
    */
    template<typename T_NEW, typename T_OLD>
    bool VirtualHook(BNM::Class targetClass, IL2CPP::MethodInfo *info, T_NEW newMet, T_OLD &&oldMet) {
        return VirtualHookImpl(targetClass, info, (void *)newMet, (void **)&oldMet);
    }

    /**
        @brief Hook method using hooking software

        Alias for BasicHook method

        @param targetMethod Virtual method info
        @param newMet Replacement method
        @param oldMet Pointer where original method pointer will be stored. Can be null

        @return True if method valid
    */
    template<typename NEW_T, typename T_OLD>
    void BasicHook(const BNM::MethodBase &targetMethod, NEW_T newMethod, T_OLD &oldBytes) {
        if (targetMethod.IsValid()) ::BasicHook((void *) targetMethod._data->methodPointer, newMethod, oldBytes);
    }

    /**
        @brief Hook method using hooking software

        Alias for BasicHook method

        @param targetMethod Virtual method info
        @param newMet Replacement method
        @param oldMet Pointer where original method pointer will be stored. Can be null

        @return True if method valid
    */
    template<typename NEW_T, typename T_OLD>
    void BasicHook(const BNM::MethodBase &targetMethod, NEW_T newMethod, T_OLD &&oldBytes) {
        if (targetMethod.IsValid()) ::BasicHook((void *) targetMethod._data->methodPointer, newMethod, oldBytes);
    }
}
// NOLINTEND
