#pragma once

#include "UserSettings/GlobalSettings.hpp"

#ifdef BNM_CLASSES_MANAGEMENT

#include "Il2CppHeaders.hpp"
#include "Class.hpp"
#include "Utils.hpp"

namespace BNM::MANAGEMENT_STRUCTURES {

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
        std::vector<CompileTimeClass> _parameterTypes{};
        void *_originalAddress{};
        BNM::IL2CPP::MethodInfo *_origin{};
        BNM::IL2CPP::MethodInfo *myInfo{};
        uint8_t _isStatic : 1{0};
        uint8_t _isInvokeHook : 1{0};
        uint8_t _isBasicHook : 1{0};
        uint8_t _skipTypeMatch : 1{0};
        template<typename> struct OriginInvokeGetter {};
        template<typename RetT, typename T, typename ...ArgsT>
        struct OriginInvokeGetter<RetT(T:: *)(ArgsT...)> {
            BNM::IL2CPP::MethodInfo *_origin{};
            void *_originalAddress{};
            explicit OriginInvokeGetter(const CustomMethod &method) : _origin(method._origin), _originalAddress(method._originalAddress) {}
            inline RetT Invoke(T *instance, ArgsT ...args) {
                if (!_origin) return None();
                if constexpr (std::is_same_v<RetT, void>) ((RetT (*)(T *, ArgsT..., BNM::IL2CPP::MethodInfo *))_originalAddress)(instance, args..., _origin);
                else return ((RetT (*)(T *, ArgsT..., BNM::IL2CPP::MethodInfo *))_originalAddress)(instance, args..., _origin);
            }
            inline RetT None() { if constexpr (!std::is_same_v<RetT, void>) return {}; }
        };
        template<typename RetT, typename ...ArgsT>
        struct OriginInvokeGetter<RetT(*)(ArgsT...)> {
            BNM::IL2CPP::MethodInfo *_origin{};
            void *_originalAddress{};
            explicit OriginInvokeGetter(const CustomMethod &method) : _origin(method._origin), _originalAddress(method._originalAddress) {}
            inline RetT Invoke(ArgsT ...args) {
                if (!_origin) return None();
#if UNITY_VER > 174
                if constexpr (std::is_same_v<RetT, void>) ((RetT (*)(ArgsT..., BNM::IL2CPP::MethodInfo *))_originalAddress)(args..., _origin);
                else return ((RetT (*)(ArgsT..., BNM::IL2CPP::MethodInfo *))_originalAddress)(args..., _origin);
#else
                if constexpr (std::is_same_v<RetT, void>) ((RetT (*)(void *, ArgsT..., BNM::IL2CPP::MethodInfo *))_originalAddress)(nullptr, args..., _origin);
                else return ((RetT (*)(void *, ArgsT..., BNM::IL2CPP::MethodInfo *))_originalAddress)(nullptr, args..., _origin);
#endif
            }
            inline RetT None() { if constexpr (!std::is_same_v<RetT, void>) return {}; }
        };
    };

    // Data about new classes
    struct CustomClass {
        inline constexpr CustomClass() = default;
        size_t _size{};
        CompileTimeClass _targetType{};
        CompileTimeClass _baseType{};
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

    template<bool, typename> struct GetMethodInvoker {};

    // Class for creating method invoker
    template<typename RetT, typename T, typename ...ArgsT>
    struct GetMethodInvoker<false, RetT(T:: *)(ArgsT...)> {
        template<std::size_t ...As>
        inline static void *InvokeMethod(RetT(*func)(T*, ArgsT...), T *instance, void **args, void *data, std::index_sequence<As...>) {
            if constexpr (std::is_same_v<RetT, void>) {
                func(instance, _InvokerHelper::UnpackArg<ArgsT>(args[As])...);
                return nullptr;
            } else return _InvokerHelper::PackReturnArg(func(instance, _InvokerHelper::UnpackArg<ArgsT>(args[As])...), ARG_4_PACK);
        }
#if UNITY_VER < 171
        static void *Invoke(IL2CPP::MethodInfo *method, void *obj, void **args) {
            IL2CPP::Il2CppMethodPointer ptr = method->methodPointer;
#else
        static void *Invoke(IL2CPP::Il2CppMethodPointer ptr, IL2CPP::MethodInfo *method, void *obj, void **args, void *returnValue) {
#endif
            auto func = (RetT(*)(T*, ArgsT...))(ptr);
            auto instance = (T *)(obj);
            auto seq = std::make_index_sequence<sizeof...(ArgsT)>();
            return InvokeMethod(func, instance, args, ARG_4_INVOKE, seq);
        }
    };

    template<typename RetT, typename T, typename ...ArgsT>
    struct GetMethodInvoker<false, RetT(T:: *)(ArgsT...) const> {
        template<std::size_t ...As>
        inline static void *InvokeMethod(RetT(*func)(T*, ArgsT...), T *instance, void **args, void *data, std::index_sequence<As...>) {
            if constexpr (std::is_same_v<RetT, void>) {
                func(instance, _InvokerHelper::UnpackArg<ArgsT>(args[As])...);
                return nullptr;
            } else return _InvokerHelper::PackReturnArg(func(instance, _InvokerHelper::UnpackArg<ArgsT>(args[As])...), ARG_4_PACK);
        }
#if UNITY_VER < 171
        static void *Invoke(IL2CPP::MethodInfo *method, void *obj, void **args) {
            IL2CPP::Il2CppMethodPointer ptr = method->methodPointer;
#else
        static void *Invoke(IL2CPP::Il2CppMethodPointer ptr, IL2CPP::MethodInfo *method, void *obj, void **args, void *returnValue) {
#endif
            auto func = (RetT(*)(T*, ArgsT...))(ptr);
            auto instance = (T *)(obj);
            auto seq = std::make_index_sequence<sizeof...(ArgsT)>();
            return InvokeMethod(func, instance, args, ARG_4_INVOKE, seq);
        }
    };

    template<typename RetT, typename ...ArgsT>
    struct GetMethodInvoker<true, RetT(*)(ArgsT...)> {
#if UNITY_VER > 174
        using FuncType = RetT(*)(ArgsT...);
#define Args _InvokerHelper::UnpackArg<ArgsT>(args[As])...
#else
        using FuncType = RetT(*)(void *, ArgsT...);
#define Args nullptr, _InvokerHelper::UnpackArg<ArgsT>(args[As])...
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
        static void *Invoke(IL2CPP::Il2CppMethodPointer ptr, IL2CPP::MethodInfo *method, void *, void **args, void *returnValue) {
#endif
            auto func = (FuncType) ptr;
            auto seq = std::make_index_sequence<sizeof...(ArgsT)>();
            return InvokeMethod(func, args, ARG_4_INVOKE, seq);
        }
    };

#undef ARG_4_PACK
#undef ARG_4_INVOKE

}

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

#define BNM_CustomMethodMarkAsInvokeHook(_method_) inline static struct _BNMCustomMethodMarkAs_##_method_ { _BNMCustomMethodMarkAs_##_method_() { BNMCustomMethod_##_method_._isInvokeHook = true; } } _BNMCustomMethodMarkAs_##_method_ __attribute__((init_priority (103))) {}
#define BNM_CustomMethodMarkAsBasicHook(_method_) inline static struct _BNMCustomMethodMarkAs_##_method_ { _BNMCustomMethodMarkAs_##_method_() { BNMCustomMethod_##_method_._isBasicHook = true; } } _BNMCustomMethodMarkAs_##_method_ __attribute__((init_priority (103))) {}
#define BNM_CustomMethodSkipTypeMatch(_method_) inline static struct _BNMCustomMethodSkipTypeMatch_##_method_ { _BNMCustomMethodSkipTypeMatch_##_method_() { BNMCustomMethod_##_method_._skipTypeMatch = true; } } _BNMCustomMethodSkipTypeMatch_##_method_ __attribute__((init_priority (103))) {}

#define BNM_CallCustomMethodOrigin(_method_, ...) BNM::MANAGEMENT_STRUCTURES::CustomMethod::OriginInvokeGetter<decltype(&_BNMCustomClassType::_method_)>(BNMCustomMethod_##_method_).Invoke(__VA_ARGS__)

#endif