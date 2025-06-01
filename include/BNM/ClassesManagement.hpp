#pragma once

#include "UserSettings/GlobalSettings.hpp"

#ifdef BNM_CLASSES_MANAGEMENT

#include <type_traits>

#include "Il2CppHeaders.hpp"
#include "Class.hpp"
#include "Utils.hpp"

namespace BNM {
    /// @cond
    namespace MANAGEMENT_STRUCTURES {
        // Data about new fields
        struct CustomField {
            inline constexpr CustomField() = default;
            std::string_view _name{};
            size_t _size{};
            CompileTimeClass _type{};
            BNM_PTR offset{};
            BNM::IL2CPP::FieldInfo *myInfo{};
        };

        // Data about new methods
        struct CustomMethod {
            inline constexpr CustomMethod() = default;
            void *_address{};
            void *_invoker{};
            std::string_view _name{};
            CompileTimeClass _returnType{};
            std::vector<CompileTimeClass> _parameterTypes{}; // Will be freed after BNM initializes this method
            void *_originalAddress{};
            BNM::IL2CPP::MethodInfo *_origin{};
            BNM::IL2CPP::MethodInfo *myInfo{};
            uint8_t _isStatic : 1{0};
            uint8_t _isInvokeHook : 1{0};
            uint8_t _isBasicHook : 1{0};
            uint8_t _skipTypeMatch : 1{0};
            template<typename> struct OriginInvokeGetter {};
            template<typename RetT, typename T, typename ...ParametersT>
            struct OriginInvokeGetter<RetT(T:: *)(ParametersT...)> {
                BNM::IL2CPP::MethodInfo *_origin{};
                void *_originalAddress{};
                explicit OriginInvokeGetter(const CustomMethod &method) : _origin(method._origin), _originalAddress(method._originalAddress) {}
                inline RetT Invoke(T *instance, ParametersT ...parameters) {
                    if (!_origin) return BNM::PRIVATE_INTERNAL::ReturnEmpty<RetT>();
                    if constexpr (std::is_same_v<RetT, void>) ((RetT (*)(T *, ParametersT..., BNM::IL2CPP::MethodInfo *))_originalAddress)(instance, parameters..., _origin);
                    else return ((RetT (*)(T *, ParametersT..., BNM::IL2CPP::MethodInfo *))_originalAddress)(instance, parameters..., _origin);
                }
            };
            template<typename RetT, typename ...ParametersT>
            struct OriginInvokeGetter<RetT(*)(ParametersT...)> {
                BNM::IL2CPP::MethodInfo *_origin{};
                void *_originalAddress{};
                explicit OriginInvokeGetter(const CustomMethod &method) : _origin(method._origin), _originalAddress(method._originalAddress) {}
                inline RetT Invoke(ParametersT ...parameters) {
                    if (!_origin) return BNM::PRIVATE_INTERNAL::ReturnEmpty<RetT>();
#if UNITY_VER > 174
                    if constexpr (std::is_same_v<RetT, void>) ((RetT (*)(ParametersT..., BNM::IL2CPP::MethodInfo *))_originalAddress)(parameters..., _origin);
                    else return ((RetT (*)(ParametersT..., BNM::IL2CPP::MethodInfo *))_originalAddress)(parameters..., _origin);
#else
                    if constexpr (std::is_same_v<RetT, void>) ((RetT (*)(void *, ParametersT..., BNM::IL2CPP::MethodInfo *))_originalAddress)(nullptr, parameters..., _origin);
                    else return ((RetT (*)(void *, ParametersT..., BNM::IL2CPP::MethodInfo *))_originalAddress)(nullptr, parameters..., _origin);
#endif
                }
            };
        };

        // Data about new classes
        struct CustomClass {
            inline constexpr CustomClass() = default;
            size_t _size{};
            CompileTimeClass _targetType{};
            CompileTimeClass _baseType{};

            // All vectors will be freed after BNM initializes this class
            std::vector<CompileTimeClass> _interfaces{};
            std::vector<CustomField *> _fields{};
            std::vector<CustomMethod *> _methods{};

            CompileTimeClass _owner{};
            BNM::IL2CPP::Il2CppClass *myClass{};
            BNM::MonoType *type{};
        };

        // Add a new class to the list
        void AddClass(CustomClass *_class);

        namespace _InvokerHelper {
            // Unpack argument
            template<typename T, std::enable_if_t<std::is_reference_v<T>, bool> = true>
            inline constexpr T &UnpackArg(void *arg) { return *(typename std::remove_reference_t<T> *)arg; }
            template<typename T, std::enable_if_t<!std::is_reference_v<T>, bool> = true>
            inline constexpr T UnpackArg(void *arg) {
                if constexpr (std::is_pointer_v<T>) return (T) arg;
                else return *(T *)arg;
            }

#if UNITY_VER >= 211
#define ARG_4_PACK data
#define ARG_4_INVOKE returnValue
            template<typename T>
            inline void *PackReturnArg(T arg, void *returnValue) {
                *(T *)returnValue = arg;
                return nullptr;
            }
#else
#define ARG_4_PACK (const IL2CPP::Il2CppType *) data
#define ARG_4_INVOKE (void *) method->return_type
            template<typename T>
            inline void *PackReturnArg(T arg, const IL2CPP::Il2CppType *type) {
                if constexpr (std::is_pointer<T>::value) return arg;
                return BNM::Class(type).BoxObject(&arg);
            }
#endif
        }

        template<bool IsStatic, typename> struct GetMethodInvoker {};

        // Class for creating method invoker
        template<typename RetT, typename T, typename ...ParametersT>
        struct GetMethodInvoker<false, RetT(T:: *)(ParametersT...)> {
            template<std::size_t ...As>
            inline static void *InvokeMethod(RetT(*func)(T*, ParametersT...), T *instance, void **args, void *data, std::index_sequence<As...>) {
                if constexpr (std::is_same_v<RetT, void>) {
                    func(instance, _InvokerHelper::UnpackArg<ParametersT>(args[As])...);
                    return nullptr;
                } else return _InvokerHelper::PackReturnArg(func(instance, _InvokerHelper::UnpackArg<ParametersT>(args[As])...), ARG_4_PACK);
            }
#if UNITY_VER < 171
            static void *Invoke(IL2CPP::MethodInfo *method, void *obj, void **args) {
                IL2CPP::Il2CppMethodPointer ptr = method->methodPointer;
#else
            static void *Invoke(IL2CPP::Il2CppMethodPointer ptr, [[maybe_unused]] IL2CPP::MethodInfo *method, void *obj, void **args, void *returnValue) {
#endif
                auto func = (RetT(*)(T*, ParametersT...))(ptr);
                auto instance = (T *)(obj);
                auto seq = std::make_index_sequence<sizeof...(ParametersT)>();
                return InvokeMethod(func, instance, args, ARG_4_INVOKE, seq);
            }
        };

        template<typename RetT, typename T, typename ...ParametersT>
        struct GetMethodInvoker<false, RetT(T:: *)(ParametersT...) const> {
            template<std::size_t ...As>
            inline static void *InvokeMethod(RetT(*func)(T*, ParametersT...), T *instance, void **args, void *data, std::index_sequence<As...>) {
                if constexpr (std::is_same_v<RetT, void>) {
                    func(instance, _InvokerHelper::UnpackArg<ParametersT>(args[As])...);
                    return nullptr;
                } else return _InvokerHelper::PackReturnArg(func(instance, _InvokerHelper::UnpackArg<ParametersT>(args[As])...), ARG_4_PACK);
            }
#if UNITY_VER < 171
            static void *Invoke(IL2CPP::MethodInfo *method, void *obj, void **args) {
                IL2CPP::Il2CppMethodPointer ptr = method->methodPointer;
#else
            static void *Invoke(IL2CPP::Il2CppMethodPointer ptr, [[maybe_unused]] IL2CPP::MethodInfo *method, void *obj, void **args, void *returnValue) {
#endif
                auto func = (RetT(*)(T*, ParametersT...))(ptr);
                auto instance = (T *)(obj);
                auto seq = std::make_index_sequence<sizeof...(ParametersT)>();
                return InvokeMethod(func, instance, args, ARG_4_INVOKE, seq);
            }
        };

        template<typename RetT, typename ...ParametersT>
        struct GetMethodInvoker<true, RetT(*)(ParametersT...)> {
#if UNITY_VER > 174
            using FuncType = RetT(*)(ParametersT...);
#define Args _InvokerHelper::UnpackArg<ParametersT>(args[As])...
#else
            using FuncType = RetT(*)(void *, ParametersT...);
#define Args nullptr, _InvokerHelper::UnpackArg<ParametersT>(args[As])...
#endif
            template<std::size_t ...As>
            inline static void *InvokeMethod(FuncType func, void **args, void *data, std::index_sequence<As...>) {
                if constexpr (std::is_same_v<RetT, void>) {
                    func(Args);
                    return nullptr;
                } else return _InvokerHelper::PackReturnArg(func(Args), ARG_4_PACK);
            }
#undef Args
#if UNITY_VER < 171
            static void *Invoke(IL2CPP::MethodInfo *method, void *, void **args) {
                IL2CPP::Il2CppMethodPointer ptr = method->methodPointer;
#else
            static void *Invoke(IL2CPP::Il2CppMethodPointer ptr, [[maybe_unused]] IL2CPP::MethodInfo *method, void *, void **args, void *returnValue) {
#endif
                auto func = (FuncType) ptr;
                auto seq = std::make_index_sequence<sizeof...(ParametersT)>();
                return InvokeMethod(func, args, ARG_4_INVOKE, seq);
            }
        };

#undef ARG_4_PACK
#undef ARG_4_INVOKE
	}
    /// @endcond
    /**
        @brief Namespace with some advanced APIs of ClassesManagement
    */
    namespace ClassesManagement {
        /**
            @brief Process custom class at runtime

            @warning Should be used in main il2cpp thread!

            Using that method you can give to BNM class that it will process at runtime.
            Useful if you need add custom later.

            @param customClass Custom class info to process
        */
        void ProcessClassRuntime(BNM::MANAGEMENT_STRUCTURES::CustomClass *customClass);
    }
}


/**
    @defgroup CUSTOM_CLASSES_MACRO Custom classes macros

    Information about all macros of classes management.

    @{
*/

/**
    @brief Define info of C++ class for il2cpp
    @hideinitializer

    Use this macro to describe info about your C++ class to add it as valid class to il2cpp.<br>
    BNM will automatically convert all information to il2cpp valid format.

    This macro adds static field `BNMCustomClass` to target class.
    Using it you can access:
    @code
    BNM::IL2CPP::Il2CppClass *myClass // Generated/modified il2cpp class object of current custom class
    BNM::MonoType *type // Mono type of current custom class, to be used in, for example `AddComponent`
    // And there are some other internal data that are used by BNM to create/modify il2cpp class.
    @endcode

    @param _class_ Class itself
    @param _targetType_ BNM::CompileTimeClass or how class will be presented in il2cpp
    @param _baseType_ BNM::CompileTimeClass of base class type
    @param _owner_ BNM::CompileTimeClass of owner class type (to make this class nested)
    @param ... BNM::CompileTimeClass of interfaces that class should override
*/
#define BNM_CustomClass(_class_, _targetType_, _baseType_, _owner_, ...) \
struct _BNMCustomClass : BNM::MANAGEMENT_STRUCTURES::CustomClass { \
    inline _BNMCustomClass() : BNM::MANAGEMENT_STRUCTURES::CustomClass() { \
        _size = sizeof(_class_); \
        _targetType = _targetType_; \
        _baseType = _baseType_; \
        _owner = _owner_; \
        _interfaces = {__VA_ARGS__}; \
        BNM::MANAGEMENT_STRUCTURES::AddClass(this); \
    } \
}; \
inline static _BNMCustomClass BNMCustomClass __attribute__((init_priority (101))) {}; \
using _BNMCustomClassType = _class_

/**
    @brief Define info about C++ field for il2cpp
    @hideinitializer

    This macro adds static field `BNMCustomField_(_field_ param)` to target class.
    Using it you can access:
    @code
    BNM_PTR offset // Offset of current field in current class (useful for modifying class)
    BNM::IL2CPP::FieldInfo *myInfo // Generated il2cpp object of current field
    // And there are some other internal data that are used by BNM to create/add il2cpp field.
    @endcode

    @param _field_ Field itself
    @param _type_ BNM::CompileTimeClass of field type that will be presented in il2cpp
    @param _name_ Field name that will be presented in il2cpp
*/
#define BNM_CustomField(_field_, _type_, _name_) \
struct _BNMCustomField_##_field_ : BNM::MANAGEMENT_STRUCTURES::CustomField { \
    inline _BNMCustomField_##_field_() : BNM::MANAGEMENT_STRUCTURES::CustomField() { \
        _name = BNM_OBFUSCATE_TMP(_name_); \
        _size = sizeof(_field_); \
        _type = _type_; \
        offset = offsetof(_BNMCustomClassType, _field_); \
        BNMCustomClass._fields.push_back(this); \
    } \
}; \
inline static _BNMCustomField_##_field_ BNMCustomField_##_field_ __attribute__((init_priority (102))) {}

/**
    @brief Define info about C++ method for il2cpp
    @hideinitializer

    This macro adds static field `BNMCustomMethod_(_method_ param)` to target class.
    Using it you can access:
    @code
    BNM::IL2CPP::MethodInfo *myInfo // Generated il2cpp object of current method
    // And there are some other internal data that are used by BNM to create/add/hook il2cpp method.
    @endcode

    @param _method_ Method itself
    @param _isStatic_ Set it to true if method is static
    @param _type_ Method returns type that will be presented in il2cpp
    @param _name_ Method name that will be presented in il2cpp
    @param ... Method parameter types that will be presented in il2cpp
*/
#define BNM_CustomMethod(_method_, _isStatic_, _type_, _name_, ...) \
struct _BNMCustomMethod_##_method_ : BNM::MANAGEMENT_STRUCTURES::CustomMethod { \
    inline _BNMCustomMethod_##_method_() : BNM::MANAGEMENT_STRUCTURES::CustomMethod() { \
        constexpr auto p = &_BNMCustomClassType::_method_; _address = *(void **)&p; \
        _invoker = (void *) &BNM::MANAGEMENT_STRUCTURES::GetMethodInvoker<_isStatic_, decltype(&_BNMCustomClassType::_method_)>::Invoke; \
        _name = BNM_OBFUSCATE_TMP(_name_); \
        _returnType = _type_; \
        _isStatic = _isStatic_; \
        _parameterTypes = {__VA_ARGS__}; \
        BNMCustomClass._methods.push_back(this); \
    } \
}; \
inline static _BNMCustomMethod_##_method_ BNMCustomMethod_##_method_ __attribute__((init_priority (102))) {}

/**
    @brief Mark method to prefer InvokeHook.
    @hideinitializer

    If this macro is applied to method, BNM will prefer to use invoke hook for it.
    @param _method_ Target method
*/
#define BNM_CustomMethodMarkAsInvokeHook(_method_) inline static struct _BNMCustomMethodMarkAs_##_method_ { _BNMCustomMethodMarkAs_##_method_() { BNMCustomMethod_##_method_._isInvokeHook = true; } } _BNMCustomMethodMarkAs_##_method_ __attribute__((init_priority (103))) {}

/**
    @brief Mark method to prefer BasicHook.
    @hideinitializer

    If this macro is applied to method, BNM will prefer to use basic hook for it.
    @param _method_ Target method
*/
#define BNM_CustomMethodMarkAsBasicHook(_method_) inline static struct _BNMCustomMethodMarkAs_##_method_ { _BNMCustomMethodMarkAs_##_method_() { BNMCustomMethod_##_method_._isBasicHook = true; } } _BNMCustomMethodMarkAs_##_method_ __attribute__((init_priority (103))) {}

/**
    @brief Skip method parameters type matching.
    @hideinitializer

    If this macro is applied to method, BNM will skip type matching for method overriding/hooking and instead it will use parameters count.
    @param _method_ Target method
*/
#define BNM_CustomMethodSkipTypeMatch(_method_) inline static struct _BNMCustomMethodSkipTypeMatch_##_method_ { _BNMCustomMethodSkipTypeMatch_##_method_() { BNMCustomMethod_##_method_._skipTypeMatch = true; } } _BNMCustomMethodSkipTypeMatch_##_method_ __attribute__((init_priority (103))) {}

/**
    @brief Call method origin, if it exists.
    @hideinitializer

    This macro can be used to call overridden (base) method or origin (old code) of hooked method.
    @param _method_ Target method
    @param ... Method call arguments (for non-static method first argument should be `this`)
*/
#define BNM_CallCustomMethodOrigin(_method_, ...) BNM::MANAGEMENT_STRUCTURES::CustomMethod::OriginInvokeGetter<decltype(&_BNMCustomClassType::_method_)>(BNMCustomMethod_##_method_).Invoke(__VA_ARGS__)
/** @} */
#endif