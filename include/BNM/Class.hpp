#pragma once

#include <list>
#include <string>

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "Image.hpp"
#include "BasicMonoStructures.hpp"
#include "Defaults.hpp"

namespace BNM {
    struct CompileTimeClass;

    struct FieldBase;
    struct MethodBase;
    struct PropertyBase;
    struct EventBase;

    struct Class {
        inline constexpr Class() = default;
        inline Class(const BNM::IL2CPP::Il2CppClass *_class) : _data((BNM::IL2CPP::Il2CppClass *) _class) {};
        Class(const BNM::IL2CPP::Il2CppObject *object);
        Class(const BNM::IL2CPP::Il2CppType *type);
        Class(const BNM::MonoType *type);
        Class(const BNM::CompileTimeClass &compileTimeClass);
        Class(const std::string_view &_namespace, const std::string_view &name);
        Class(const std::string_view &_namespace, const std::string_view &name, const BNM::Image &image);

        [[nodiscard]] std::vector<BNM::Class> GetInnerClasses(bool includeParent = true) const;
        [[nodiscard]] std::vector<BNM::FieldBase> GetFields(bool includeParent = true) const;
        [[nodiscard]] std::vector<BNM::MethodBase> GetMethods(bool includeParent = true) const;
        [[nodiscard]] std::vector<BNM::PropertyBase> GetProperties(bool includeParent = true) const;
        [[nodiscard]] std::vector<BNM::EventBase> GetEvents(bool includeParent = true) const;

        [[nodiscard]] BNM::MethodBase GetMethod(const std::string_view &name, int parameters = -1) const;
        [[nodiscard]] BNM::MethodBase GetMethod(const std::string_view &name, const std::initializer_list<std::string_view> &parametersName) const;
        [[nodiscard]] BNM::MethodBase GetMethod(const std::string_view &name, const std::initializer_list<BNM::CompileTimeClass> &parametersType) const;
        [[nodiscard]] BNM::PropertyBase GetProperty(const std::string_view &name) const;
        [[nodiscard]] BNM::Class GetInnerClass(const std::string_view &name) const;
        [[nodiscard]] BNM::FieldBase GetField(const std::string_view &name) const;
        [[nodiscard]] BNM::EventBase GetEvent(const std::string_view &name) const;

        [[nodiscard]] BNM::Class GetParent() const;

        [[nodiscard]] BNM::Class GetArray() const; // To the array class (class[])
        [[nodiscard]] BNM::Class GetPointer() const; // To the pointer of the class (class *)
        [[nodiscard]] BNM::Class GetReference() const; // To the reference to the class (class &)
        [[nodiscard]] BNM::Class GetGeneric(const std::initializer_list<BNM::CompileTimeClass> &templateTypes) const; // Class <types from the list>

        [[nodiscard]] BNM::IL2CPP::Il2CppType *GetIl2CppType() const;
        [[nodiscard]] BNM::MonoType *GetMonoType() const;
        [[nodiscard]] inline BNM::IL2CPP::Il2CppClass *GetClass() const { TryInit(); return _data; }
        [[nodiscard]] BNM::CompileTimeClass GetCompileTimeClass() const;

        inline operator BNM::IL2CPP::Il2CppType *() const { return GetIl2CppType(); }
        inline operator BNM::MonoType *() const { return GetMonoType(); }
        inline operator BNM::IL2CPP::Il2CppClass *() const { return GetClass(); }
        operator BNM::CompileTimeClass() const;

        // The same as new Object() in C#, but without calling the constructor (.ctor)
        [[nodiscard]] BNM::IL2CPP::Il2CppObject *CreateNewInstance() const;

        // The same as new Object[] in C#
        template<typename T>
        BNM::Structures::Mono::Array<T> *NewArray(IL2CPP::il2cpp_array_size_t length = 0) const {
            BNM_LOG_ERR_IF(!_data, DBG_BNM_MSG_Class_Dead_Error);
            if (!_data) return nullptr;
            TryInit();
            return (BNM::Structures::Mono::Array<T> *) ArrayNew(_data, length);
        }

        // The same as new List<Object>() in C#
        template<typename T>
        Structures::Mono::List<T> *NewList() const;

        // Almost the same as newList, but the new list will use BNM code instead of il2cpp
        template<typename T>
        Structures::Mono::List<T> *NewListBNM() const {
            BNM_LOG_ERR_IF(!_data, DBG_BNM_MSG_Class_Dead_Error);
            if (!_data) return nullptr;
            TryInit();
            BNM::Structures::Mono::Array<T> *array = NewArray<T>(1);
            auto *lst = (BNM::Structures::Mono::List<T> *) NewListInstance();
            if (!lst) {
                BNM_LOG_ERR(DBG_BNM_MSG_Class_NewList_Error, str().c_str());
                return nullptr;
            }
            lst->items = array;
            BNM::Structures::Mono::PRIVATE_MonoListData::InitMonoListVTable(lst);
            return lst;
        }

        // To pack an object
        template<typename T, typename = std::enable_if<!std::is_pointer<T>::value>>
        IL2CPP::Il2CppObject *BoxObject(T obj) const {
            BNM_LOG_ERR_IF(!_data, DBG_BNM_MSG_Class_Dead_Error);
            if (!_data) return nullptr;
            TryInit();
            return ObjBox(_data, (void *) obj);
        }

        // The same as new Object() in C# with calling the constructor by number of arguments
        template<typename ...Parameters>
        BNM::IL2CPP::Il2CppObject *CreateNewObjectParameters(Parameters ...parameters) const;

        template<typename ...Parameters>
        BNM::IL2CPP::Il2CppObject *CreateNewObjectTypes(const std::initializer_list<std::string_view> &parameterNames, Parameters ...parameters) const;

        // Checking if Class is alive
        [[nodiscard]] inline bool Valid() const { return _data != nullptr; }
        [[nodiscard]] inline bool Alive() const { return Valid(); }
        inline operator bool() const { return Valid(); }


#ifdef BNM_ALLOW_STR_METHODS
        // Get information about the class
        [[nodiscard]] inline std::string str() const {
            if (!_data) return BNM_OBFUSCATE(DBG_BNM_MSG_Class_str_nullptr);
            TryInit();
            std::string data{};
            if (_data->declaringType) {
                data += Class(_data->declaringType).str() + BNM_OBFUSCATE("<-");
                data += '[';
                data += _data->name;
                data += ']';
            } else {
                data += BNM_OBFUSCATE("[");
                data += _data->image->name + std::string(BNM_OBFUSCATE("]::["));
                data += _data->namespaze + std::string(BNM_OBFUSCATE("]::[")) + _data->name + BNM_OBFUSCATE("]");
            }

            return data;
        }
#endif

        BNM::IL2CPP::Il2CppClass *_data{};

    private:
        void TryInit() const;
        static IL2CPP::Il2CppObject *ObjBox(IL2CPP::Il2CppClass*, void*);
        static IL2CPP::Il2CppArray *ArrayNew(IL2CPP::Il2CppClass*, IL2CPP::il2cpp_array_size_t);
        static void *NewListInstance();
        static Class GetListClass();
        friend CompileTimeClass;
    };

    // A structure for storing data at compile time and then searching for it during code execution
    struct CompileTimeClass {
        struct _BaseInfo;
        enum class ModifierType : uint8_t {
            None, Array, Pointer, Reference
        };
        std::list<_BaseInfo *> _stack{};
        union {
            Class _loadedClass{};
            Defaults::Internal::ClassType *reference;
        };
        uint8_t _autoFree : 1{true}, _isReferenced{false};
        Class ToClass();
        IL2CPP::Il2CppType *ToIl2CppType();
        IL2CPP::Il2CppClass *ToIl2CppClass();
        [[nodiscard]] Class ToClass() const;
        [[nodiscard]] IL2CPP::Il2CppType *ToIl2CppType() const;
        [[nodiscard]] IL2CPP::Il2CppClass *ToIl2CppClass() const;
        operator IL2CPP::Il2CppType*();
        operator IL2CPP::Il2CppClass*();
        operator Class();
        operator IL2CPP::Il2CppType*() const;
        operator IL2CPP::Il2CppClass*() const;
        operator Class() const;
        void Free();
        enum class _BaseType : uint8_t {
            // We explicitly set the values, because the code uses a list
            None = 0, Class = 1, Inner = 2, Modifier = 3, Generic = 4, MaxCount = 5
        };
        struct _BaseInfo {
            _BaseInfo(_BaseType _baseType) : _baseType(_baseType) {}
            _BaseType _baseType{_BaseType::None};
        };
        struct _ClassInfo : _BaseInfo {
            _ClassInfo(const char *_namespace, const char *_name, const char *_imageName = nullptr) : _BaseInfo(_BaseType::Class), _namespace(_namespace), _name(_name), _imageName(_imageName) {}
            const char *_namespace{}, *_name{}, *_imageName{};
        };
        struct _InnerInfo : _BaseInfo {
            _InnerInfo(const char *_name) : _BaseInfo(_BaseType::Inner), _name(_name) {}
            const char *_name{};
        };
        struct _ModifierInfo : _BaseInfo {
            _ModifierInfo(ModifierType _modifierType) : _BaseInfo(_BaseType::Modifier), _modifierType(_modifierType) {}
            ModifierType _modifierType{ModifierType::None};
        };
        struct _GenericInfo : _BaseInfo {
            _GenericInfo(const std::initializer_list<CompileTimeClass> &_types) : _BaseInfo(_BaseType::Generic), _types(_types) {}
            std::vector<CompileTimeClass> _types{};
        };
    };

    struct CompileTimeClassBuilder {
        CompileTimeClass _data{};
        inline CompileTimeClassBuilder(const char *_namespace, const char *_name, const char *_imageName = nullptr, bool autoFree = true) {
            _data._autoFree = autoFree;
            auto info = (CompileTimeClass::_ClassInfo *) BNM_malloc(sizeof(CompileTimeClass::_ClassInfo));
            *info = CompileTimeClass::_ClassInfo{_namespace, _name, _imageName};
            _data._stack.push_back(info);
        }
        inline CompileTimeClassBuilder &Class(const char *_name) {
            auto info = (CompileTimeClass::_InnerInfo *) BNM_malloc(sizeof(CompileTimeClass::_InnerInfo));
            *info = CompileTimeClass::_InnerInfo{_name};
            _data._stack.push_back(info);
            return *this;
        }
        inline CompileTimeClassBuilder &Modifier(CompileTimeClass::ModifierType type) {
            auto modifier = (CompileTimeClass::_ModifierInfo *) BNM_malloc(sizeof(CompileTimeClass::_ModifierInfo));
            *modifier = CompileTimeClass::_ModifierInfo{type};
            _data._stack.push_back(modifier);
            return *this;
        }
        inline CompileTimeClassBuilder &Generic(const std::initializer_list<CompileTimeClass> &genericTypes) {
            auto generic = (CompileTimeClass::_GenericInfo *) BNM_malloc(sizeof(CompileTimeClass::_GenericInfo));
            memset(generic, 0, sizeof(CompileTimeClass::_GenericInfo));
            *generic = CompileTimeClass::_GenericInfo{genericTypes};
            _data._stack.push_back(generic);
            return *this;
        }
        inline CompileTimeClass Build() { return std::move(_data); }
    };


    template<typename T>
    Structures::Mono::List<T> *Class::NewList() const {
        BNM_LOG_ERR_IF(!_data, DBG_BNM_MSG_Class_Dead_Error);
        if (!_data) return nullptr;
        TryInit();
        auto lst = (BNM::Structures::Mono::List<T> *) GetListClass().GetGeneric({GetCompileTimeClass()}).CreateNewObjectParameters();
        if (!lst) {
            BNM_LOG_ERR(DBG_BNM_MSG_Class_NewList_Error, str().c_str());
            return nullptr;
        }
        return lst;
    }

    // Methods for checking object class
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, IL2CPP::Il2CppClass *_class) { return IsA<BNM::IL2CPP::Il2CppObject *>((IL2CPP::Il2CppObject *)object, _class); }
    template<> bool IsA<IL2CPP::Il2CppObject *>(IL2CPP::Il2CppObject *object, IL2CPP::Il2CppClass *_class);
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, Class _class) { return IsA(object, _class.GetClass()); }
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, IL2CPP::Il2CppObject *_class) { if (!_class) return false; return IsA(object, _class->klass); }
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, MonoType *type) { return IsA(object, Class(type)); }

#ifdef BNM_OLD_GOOD_DAYS
    typedef Class LoadClass;
#endif

}