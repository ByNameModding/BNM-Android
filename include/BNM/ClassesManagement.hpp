#pragma once

#include "UserSettings/GlobalSettings.hpp"

#ifdef BNM_CLASSES_MANAGEMENT

#include "Il2CppHeaders.hpp"
#include "Class.hpp"
#include "Utils.hpp"

namespace BNM::MANAGEMENT_STRUCTURES {

#pragma pack(push, 1)

    // Данные о новых полях
    struct CustomField {
        inline constexpr CustomField() noexcept = default;
        std::string_view _name{};
        size_t _size{};
        CompileTimeClass _type{};
        BNM_PTR offset{};
        BNM::IL2CPP::FieldInfo *myInfo{};
    };

    // Данные о новых методах
    struct CustomMethod {
        inline constexpr CustomMethod() noexcept = default;
        void *_address{};
        void *_invoker{};
        std::string_view _name{};
        CompileTimeClass _returnType{};
        std::vector<CompileTimeClass> _parameterTypes{};
        bool _isStatic{};
        void *_originalAddress{};
        BNM::IL2CPP::MethodInfo *myInfo{};
        template<typename> struct OriginInvokeGetter {};
        template<typename RetT, typename T, typename ...ArgsT>
        struct OriginInvokeGetter<RetT(T:: *)(ArgsT...)> {
            void *_originalAddress{};
            explicit OriginInvokeGetter(void *_originalAddress) : _originalAddress(_originalAddress) {}
            RetT Invoke(T *instance, ArgsT ...args) {
                if constexpr (std::is_same_v<RetT, void>) ((RetT (*)(T *, ArgsT...))_originalAddress)(instance, args...);
                else return ((RetT (*)(T *, ArgsT...))_originalAddress)(instance, args...);
            }
        };
        template<typename RetT, typename ...ArgsT>
        struct OriginInvokeGetter<RetT(*)(ArgsT...)> {
            void *_originalAddress{};
            explicit OriginInvokeGetter(void *_originalAddress) : _originalAddress(_originalAddress) {}
            RetT Invoke(ArgsT ...args) {
#if UNITY_VER > 174
                if constexpr (std::is_same_v<RetT, void>) ((RetT (*)(ArgsT...))_originalAddress)(args...);
                else return ((RetT (*)(ArgsT...))_originalAddress)(args...);
#else
                if constexpr (std::is_same_v<RetT, void>) ((RetT (*)(void *, ArgsT...))_originalAddress)(nullptr, args...);
                else return ((RetT (*)(void *, ArgsT...))_originalAddress)(nullptr, args...);
#endif
            }
        };
    };

    // Данные о новых классах
    struct CustomClass {
        inline constexpr CustomClass() noexcept = default;
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

    // Добавить новый класс в список
    void AddClass(CustomClass *klass);

    namespace _InvokerHelper {
        // Распаковать аргумент
        template<typename T>
        inline constexpr T UnpackArg(void *arg) {
            if constexpr (std::is_pointer<T>::value) return (T) arg;
            else return *(T *)arg;
        }

#if UNITY_VER >= 221
#define ARG_4_PACK data
#define ARG_4_INVOKE returnValue
        template<typename T>
        inline void *PackReturnArg(T arg, void *returnValue) {
            *(T *)returnValue = arg;
            return nullptr;
        }
#else
#define ARG_4_PACK (const IL2CPP::Il2CppType *) data
#define ARG_4_INVOKE (void *) method
        template<typename T>
        inline void *PackReturnArg(T arg, const IL2CPP::Il2CppType *type) {
            if constexpr (std::is_pointer<T>::value) return arg;
            return BNM::Class(type).BoxObject(&arg);
        }
#endif
    }

    template<bool, typename> struct GetMethodInvoker {};

    // Класс для создания инициатора вызовов для методов
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
        static void *invoke(IL2CPP::MethodInfo *method, void *obj, void **args) {
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
        static void *invoke(IL2CPP::MethodInfo *method, void *, void **args) {
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

#pragma pack(pop)

}

#define BNM_CustomClass(_class_, _targetType_, _baseType_, _owner_, ...) \
private: \
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
public: \
    inline static _BNMCustomClass BNMCustomClass __attribute__((init_priority (101))) {}; \
    using _BNMCustomClassType = _class_


#define BNM_CustomField(_field_, _type_, _name_) \
private: \
	struct _BNMCustomField_##_field_ : BNM::MANAGEMENT_STRUCTURES::CustomField { \
        inline _BNMCustomField_##_field_() : BNM::MANAGEMENT_STRUCTURES::CustomField() { \
            _name = OBFUSCATE_BNM(_name_); \
            _size = sizeof(_field_); \
            _type = _type_; \
            offset = offsetof(_BNMCustomClassType, _field_); \
            BNMCustomClass._fields.push_back(this); \
        } \
    }; \
public: \
    inline static _BNMCustomField_##_field_ BNMCustomField_##_field_ __attribute__((init_priority (102))) {}

#define BNM_CustomMethod(_method_, _isStatic_, _type_, _name_, ...) \
private: \
	struct _BNMCustomMethod_##_method_ : BNM::MANAGEMENT_STRUCTURES::CustomMethod { \
        inline _BNMCustomMethod_##_method_() : BNM::MANAGEMENT_STRUCTURES::CustomMethod() { \
            constexpr auto p = &_BNMCustomClassType::_method_; _address = *(void **)&p; \
            _invoker = (void *) &BNM::MANAGEMENT_STRUCTURES::GetMethodInvoker<_isStatic_, decltype(&_BNMCustomClassType::_method_)>::Invoke; \
            _name = OBFUSCATE_BNM(_name_); \
            _returnType = _type_; \
            _isStatic = _isStatic_; \
            _parameterTypes = {__VA_ARGS__}; \
            BNMCustomClass._methods.push_back(this); \
        } \
    }; \
public: \
    inline static _BNMCustomMethod_##_method_ BNMCustomMethod_##_method_ __attribute__((init_priority (102))) {}

#define BNM_CallOriginalCustomMethod(_method_, ...) BNM::MANAGEMENT_STRUCTURES::CustomMethod::OriginInvokeGetter<decltype(&_BNMCustomClassType::_method_)>(BNMCustomMethod_##_method_._originalAddress).Invoke(__VA_ARGS__)

#endif