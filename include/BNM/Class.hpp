#pragma once

#include <list>
#include <string>

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "Image.hpp"
#include "BasicMonoStructures.hpp"

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
            if (!_data) return OBFUSCATE_BNM(DBG_BNM_MSG_Class_str_nullptr);
            TryInit();
            std::string data{};
            if (_data->declaringType) {
                data += Class(_data->declaringType).str() + OBFUSCATE_BNM("<-");
                data += '[';
                data += _data->name;
                data += ']';
            } else {
                data += OBFUSCATE_BNM("[");
                data += _data->image->name + std::string(OBFUSCATE_BNM("]::["));
                data += _data->namespaze + std::string(OBFUSCATE_BNM("]::[")) + _data->name + OBFUSCATE_BNM("]");
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
        bool _autoFree{true};
        Class _loadedClass{};
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
            _InnerInfo(const char *_name) : _BaseInfo(_BaseType::Class), _name(_name) {}
            const char *_name{};
        };
        struct _ModifierInfo : _BaseInfo {
            _ModifierInfo(ModifierType _modifierType) : _BaseInfo(_BaseType::Modifier), _modifierType(_modifierType) {}
            ModifierType _modifierType{ModifierType::None};
        };
        struct _GenericInfo : _BaseInfo {
            _GenericInfo(const std::vector<CompileTimeClass> &_types) : _BaseInfo(_BaseType::Generic), _types(_types) {}
            std::vector<CompileTimeClass> _types{};
        };
    };

    struct ConstexprCompileTimeClass {
        const char *_namespace{}, *_name{}, *_imageName{};
        CompileTimeClass::ModifierType _modifier{};
        [[nodiscard]] inline CompileTimeClass ToCompileTimeClass() const {
            CompileTimeClass result{};

            auto info = (CompileTimeClass::_ClassInfo *) BNM_malloc(sizeof(CompileTimeClass::_ClassInfo));
            *info = CompileTimeClass::_ClassInfo{_namespace, _name, _imageName};
            result._stack.push_back(info);

            if (_modifier != CompileTimeClass::ModifierType::None) {
                auto modifier = (CompileTimeClass::_ModifierInfo *) BNM_malloc(sizeof(CompileTimeClass::_ModifierInfo));
                *modifier = CompileTimeClass::_ModifierInfo{_modifier};
                result._stack.push_back(modifier);
            }

            return std::move(result);
        }
        [[nodiscard]] inline Class ToClass() const {
            return ToCompileTimeClass().ToClass();
        }
        inline operator CompileTimeClass() const { return ToCompileTimeClass(); }
        inline operator Class() const { return ToClass(); }
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

    namespace Structures::Unity {
        struct Vector2;
        struct Vector3;
        struct Color;
        struct Color32;
        struct Ray;
        struct RaycastHit;
        struct Quaternion;
    }
    namespace UnityEngine {
        struct Object;
        struct MonoBehaviour;
    }
    namespace Coroutine { struct IEnumerator; }

    // Save the name of the base types at compile time
    template<typename T>
    constexpr ConstexprCompileTimeClass GetType(CompileTimeClass::ModifierType modifier = CompileTimeClass::ModifierType::None) noexcept {
        using namespace Structures::Unity;
        using namespace Structures::Mono;

        if constexpr (std::is_same_v<T, void>)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Void"), OBFUSCATE_BNM("mscorlib"), modifier};
        else if constexpr (std::is_same_v<T, bool>)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Boolean"), OBFUSCATE_BNM("mscorlib"), modifier};
        else if constexpr (std::is_same_v<T, uint8_t>)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Byte"), OBFUSCATE_BNM("mscorlib"), modifier};
        else if constexpr (std::is_same_v<T, int8_t>)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("SByte"), OBFUSCATE_BNM("mscorlib"), modifier};
        else if constexpr (std::is_same_v<T, int16_t>)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Int16"), OBFUSCATE_BNM("mscorlib"), modifier};
        else if constexpr (std::is_same_v<T, uint16_t>)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("UInt16"), OBFUSCATE_BNM("mscorlib"), modifier};
        else if constexpr (std::is_same_v<T, int32_t>)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Int32"), OBFUSCATE_BNM("mscorlib"), modifier};
        else if constexpr (std::is_same_v<T, uint32_t>)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("UInt32"), OBFUSCATE_BNM("mscorlib"), modifier};
        else if constexpr (std::is_same_v<T, intptr_t>)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("IntPtr"), OBFUSCATE_BNM("mscorlib"), modifier};
        else if constexpr (std::is_same_v<T, int64_t>)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Int64"), OBFUSCATE_BNM("mscorlib"), modifier};
        else if constexpr (std::is_same_v<T, uint64_t>)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("UInt64"), OBFUSCATE_BNM("mscorlib"), modifier};
        else if constexpr (std::is_same_v<T, float>)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Single"), OBFUSCATE_BNM("mscorlib"), modifier};
        else if constexpr (std::is_same_v<T, double>)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Double"), OBFUSCATE_BNM("mscorlib"), modifier};
        else if constexpr (std::is_same_v<T, BNM::IL2CPP::Il2CppString *> || std::is_same_v<T, Structures::Mono::String *>)
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("String"), OBFUSCATE_BNM("mscorlib"), modifier};
        else if constexpr (std::is_same_v<T, Vector3>)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Vector3"), OBFUSCATE_BNM("UnityEngine.CoreModule"), modifier};
        else if constexpr (std::is_same_v<T, Vector2>)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Vector2"), OBFUSCATE_BNM("UnityEngine.CoreModule"), modifier};
        else if constexpr (std::is_same_v<T, Color>)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Color"), OBFUSCATE_BNM("UnityEngine.CoreModule"), modifier};
        else if constexpr (std::is_same_v<T, Color32>)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Color32"), OBFUSCATE_BNM("UnityEngine.CoreModule"), modifier};
        else if constexpr (std::is_same_v<T, Ray>)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Ray"), OBFUSCATE_BNM("UnityEngine.CoreModule"), modifier};
        else if constexpr (std::is_same_v<T, RaycastHit>)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("RaycastHit"), OBFUSCATE_BNM("UnityEngine.PhysicsModule"), modifier};
        else if constexpr (std::is_same_v<T, Quaternion>)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Quaternion"), OBFUSCATE_BNM("UnityEngine.CoreModule"), modifier};
        else if constexpr (std::is_same_v<T, BNM::UnityEngine::Object *>)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Object"), OBFUSCATE_BNM("UnityEngine.CoreModule"), modifier};
        else if constexpr (std::is_same_v<T, BNM::UnityEngine::MonoBehaviour *>)
            return {OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("MonoBehaviour"), OBFUSCATE_BNM("UnityEngine.CoreModule"), modifier};
        else if constexpr (std::is_same_v<T, BNM::Coroutine::IEnumerator> || std::is_same_v<T, BNM::Coroutine::IEnumerator *>)
            return {OBFUSCATE_BNM("BNM"), OBFUSCATE_BNM("IEnumerator"), OBFUSCATE_BNM("Assembly-CSharp"), modifier};
        else
            return {OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Object"), OBFUSCATE_BNM("mscorlib"), modifier};
    }

    // Methods for checking object class
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, IL2CPP::Il2CppClass *klass) { return IsA<BNM::IL2CPP::Il2CppObject *>((IL2CPP::Il2CppObject *)object, klass); }
    template<> bool IsA<IL2CPP::Il2CppObject *>(IL2CPP::Il2CppObject *object, IL2CPP::Il2CppClass *klass);
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, Class klass) { return IsA(object, klass.GetClass()); }
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, IL2CPP::Il2CppObject *klass) { if (!klass) return false; return IsA(object, klass->klass); }
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, MonoType *type) { return IsA(object, Class(type)); }

#ifdef BNM_OLD_GOOD_DAYS
    typedef Class LoadClass;
#endif

}