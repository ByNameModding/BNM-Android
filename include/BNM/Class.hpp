#pragma once

#include <list>
#include <string>

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "Image.hpp"
#include "BasicMonoStructures.hpp"
#include "Defaults.hpp"

// NOLINTBEGIN
/**
    @brief Main BNM namespace
*/
namespace BNM {
    struct CompileTimeClass;

    struct FieldBase;
    struct MethodBase;
    struct PropertyBase;
    struct EventBase;

    /**
        @brief Class for working with il2cpp classes.

        This class allows to find classes or get class from types, objects and work with it.
    */
    struct Class {
        /**
            @brief Create empty class.
        */
        inline constexpr Class() = default;

        /**
            @brief Create class from il2cpp class.
            @param _class Il2cpp class
        */
        inline Class(const BNM::IL2CPP::Il2CppClass *_class) : _data((BNM::IL2CPP::Il2CppClass *) _class) {};

        /**
            @brief Create class from il2cpp object.

            Creates class using type of object, that passed as parameter.

            @param object Il2cpp object
        */
        Class(const BNM::IL2CPP::Il2CppObject *object);

        /**
            @brief Create class from il2cpp type.

            Creates class using type, that passed as parameter.

            @param type Il2cpp type
        */
        Class(const BNM::IL2CPP::Il2CppType *type);

        /**
            @brief Create class from mono type.

            Creates class using mono type (il2cpp vm's type), that passed as parameter.

            @param type Mono type
        */
        Class(const BNM::MonoType *type);

        /**
            @brief Create class from compile time class.

            Creates class using compile time class, that passed as parameter.

            @param compileTimeClass CompileTimeClass type
        */
        Class(const BNM::CompileTimeClass &compileTimeClass);

        /**
            @brief Create class from name and namespace.

            Trying to find class using namespace and name in all assemblies.

            @param _namespace Class namespace
            @param name Class name
        */
        Class(const std::string_view &_namespace, const std::string_view &name);

        /**
            @brief Create class from name, namespace and image.

            Trying to find class using namespace and name in target image.

            @param _namespace Class namespace
            @param name Class name
            @param image Target image
        */
        Class(const std::string_view &_namespace, const std::string_view &name, const BNM::Image &image);

        /**
            @brief Get all inner classes of target class.

            Gets all inner classes of target class and of its parents.

            @param includeParent Should include inner classes from parents

            @return Vector of inner classes if anyone of them exists.
        */
        [[nodiscard]] std::vector<BNM::Class> GetInnerClasses(bool includeParent = true) const;

        /**
            @brief Get all fields of target class.

            Gets all fields of target class and of its parents.

            @param includeParent Should include fields from parents

            @return Vector of fields if anyone of them exists.
        */
        [[nodiscard]] std::vector<BNM::FieldBase> GetFields(bool includeParent = true) const;

        /**
            @brief Get all methods of target class.

            Gets all methods of target class and of its parents.

            @param includeParent Should include methods from parents

            @return Vector of methods if anyone of them exists.
        */
        [[nodiscard]] std::vector<BNM::MethodBase> GetMethods(bool includeParent = true) const;

        /**
            @brief Get all properties of target class.

            Gets all properties of target class and of its parents.

            @param includeParent Should include properties from parents

            @return Vector of properties if anyone of them exists.
        */
        [[nodiscard]] std::vector<BNM::PropertyBase> GetProperties(bool includeParent = true) const;

        /**
            @brief Get all events of target class.

            Gets all events of target class and of its parent.

            @param includeParent Should include events from parents

            @return Vector of events if anyone of them exists.
        */
        [[nodiscard]] std::vector<BNM::EventBase> GetEvents(bool includeParent = true) const;

        /**
            @brief Get method by name and parameters count.

            Tries to get method using its name and parameters count.
            If method isn't found in target class, the code will search method in parents.

            @param name Target method name
            @param parameters Target method parameters count

            @return MethodBase object
        */
        [[nodiscard]] BNM::MethodBase GetMethod(const std::string_view &name, int parameters = -1) const;

        /**
            @brief Get method by name and parameters name.

            Tries to get method using its name and parameter names.
            If method isn't found in target class, the code will search method in parents.

            @param name Target method name
            @param parameterNames Target method parameter names

            @return MethodBase object
        */
        [[nodiscard]] BNM::MethodBase GetMethod(const std::string_view &name, const std::initializer_list<std::string_view> &parameterNames) const;

        /**
            @brief Get method by name and parameters types.

            Tries to get method using its name and parameter types.
            If method isn't found in target class, the code will search method in parents.

            @param name Target method name
            @param parameterTypes Target method parameter types

            @return MethodBase object
        */
        [[nodiscard]] BNM::MethodBase GetMethod(const std::string_view &name, const std::initializer_list<BNM::CompileTimeClass> &parameterTypes) const;

        /**
            @brief Get property by name.

            Tries to get property using its name.
            If property isn't found in target class, the code will search property in parents.

            @param name Target property name

            @return PropertyBase object
        */
        [[nodiscard]] BNM::PropertyBase GetProperty(const std::string_view &name) const;

        /**
            @brief Get inner class by name.

            Tries to get inner class using its name.
            If inner class isn't found in target class, the code will search inner class in parents.

            @param name Target property name

            @return PropertyBase object
        */
        [[nodiscard]] BNM::Class GetInnerClass(const std::string_view &name) const;

        /**
            @brief Get field by name.

            Tries to get field using its name.
            If field isn't found in target class, the code will search field in parents.

            @param name Target field name

            @return FieldBase object
        */
        [[nodiscard]] BNM::FieldBase GetField(const std::string_view &name) const;

        /**
            @brief Get event by name.

            Tries to get event using its name.
            If event isn't found in target class, the code will search event in parents.

            @param name Target event name

            @return EventBase object
        */
        [[nodiscard]] BNM::EventBase GetEvent(const std::string_view &name) const;

        /**
            @brief Get parent.

            Gets parent of current class.

            @return Parent class if it exists.
        */
        [[nodiscard]] BNM::Class GetParent() const;

        /**
            @brief Get array class.

            Gets array class of current class (class[]).

            @return Array class
        */
        [[nodiscard]] BNM::Class GetArray() const;

        /**
            @brief Get pointer to class.

            Gets pointer class of current class (class *).

            @return Pointer class
        */
        [[nodiscard]] BNM::Class GetPointer() const;

        /**
            @brief Get reference class.

            Gets reference class of current class (class &).

            @return Reference class
        */
        [[nodiscard]] BNM::Class GetReference() const;

        /**
            @brief Get typed generic class.

            Gets typed generic class of current class (class <types from the list>).

            @param templateTypes List of template types

            @return Typed generic class
        */
        [[nodiscard]] BNM::Class GetGeneric(const std::initializer_list<BNM::CompileTimeClass> &templateTypes) const; // Class <types from the list>


        /**
            @brief Get Il2CppType.

            Gets Il2CppType object of current class.

            @return Il2CppType of class
        */
        [[nodiscard]] BNM::IL2CPP::Il2CppType *GetIl2CppType() const;

        /**
            @brief Get MonoType.

            Gets MonoType object of current class.

            @return MonoType of class
        */
        [[nodiscard]] BNM::MonoType *GetMonoType() const;

        /**
            @brief Get Il2CppClass.

            Gets Il2CppClass object of current class.

            @return Il2CppClass
        */
        [[nodiscard]] inline BNM::IL2CPP::Il2CppClass *GetClass() const { TryInit(); return _data; }

        /**
            @brief Get CompileTimeClass.

            Gets CompileTimeClass of current class.

            @return CompileTimeClass of class
        */
        [[nodiscard]] BNM::CompileTimeClass GetCompileTimeClass() const;

        /**
            @brief Get Image of class.

            Gets Image of current class.

            @return Image
        */
        [[nodiscard]] BNM::Image GetImage() const;
        
        /**
            @brief Get Il2CppType.

            Alias of GetIl2CppType().

            @return Il2CppType of class
        */
        inline operator BNM::IL2CPP::Il2CppType *() const { return GetIl2CppType(); }
        
        /**
            @brief Get MonoType.

            Alias of GetMonoType().

            @return MonoType of class
        */
        inline operator BNM::MonoType *() const { return GetMonoType(); }

        /**
            @brief Get Il2CppClass.

            Alias of GetClass().

            @return Il2CppClass
        */
        inline operator BNM::IL2CPP::Il2CppClass *() const { return GetClass(); }
        
        /**
            @brief Get CompileTimeClass.

            Alias of GetCompileTimeClass().

            @return CompileTimeClass of class
        */
        operator BNM::CompileTimeClass() const;

        /**
            @brief Create class instance.

            Creates instance of current class.
            The same as `new Object()` in C#, but without calling the constructor (.ctor).

            @return New instance
        */
        [[nodiscard]] BNM::IL2CPP::Il2CppObject *CreateNewInstance() const;

        /**
            @brief Create array of this type.

            Creates new array with set size (capacity).
            The same as `new Object[]` in C#.

            @param size New array size
            @tparam T Array element type
            @return New array
        */
        template<typename T>
        BNM::Structures::Mono::Array<T> *NewArray(IL2CPP::il2cpp_array_size_t size = 0) const {
            BNM_LOG_ERR_IF(!_data, DBG_BNM_MSG_Class_Dead_Error);
            if (!_data) return nullptr;
            TryInit();
            return (BNM::Structures::Mono::Array<T> *) ArrayNew(_data, size);
        }

        /**
            @brief Create list of this type.

            Creates new list of this types.
            The same as `new List<Object>()` in C#.

            @tparam T List element type
            @return New list
        */
        template<typename T>
        Structures::Mono::List<T> *NewList() const;

        /**
            @brief Create list of this type.
            @warning It's not recommended to use for beginners!

            Creates new list of this types, but new list will use BNM code instead of il2cpp.
            This can be useful if you want to create list of struct that doesn't exist in target Unity app.

            @tparam T List element type
            @return New list
        */
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

        /**
            @brief Box object of this type.

            Box object that you pass into it, used to pass `int` and other system types as Il2CppObject.
            If you unsure, read C# documentation about object boxing.

            @tparam T Non pointer object type
            @return Boxed object
        */
        template<typename T, typename = std::enable_if<!std::is_pointer<T>::value>>
        IL2CPP::Il2CppObject *BoxObject(T obj) const {
            BNM_LOG_ERR_IF(!_data, DBG_BNM_MSG_Class_Dead_Error);
            if (!_data) return nullptr;
            TryInit();
            return BoxObject(_data, (void *) &obj);
        }

        /**
            @brief Create class instance.

            Creates instance of current class and calls constructor.
            The same as `new Object()` in C# with calling the constructor by number of parameters.

            @tparam Parameters Constructor parameter types
            @param parameters Constructor parameters
            @return New instance
        */
        template<typename ...Parameters>
        BNM::IL2CPP::Il2CppObject *CreateNewObjectParameters(Parameters ...parameters) const;

        /**
            @brief Create class instance.

            Creates instance of current class and calls constructor.
            The same as `new Object()` in C# with calling the constructor by parameter names.

            @tparam Parameters Constructor parameters
            @param parameters Constructor parameters
            @return New instance
        */
        template<typename ...Parameters>
        BNM::IL2CPP::Il2CppObject *CreateNewObjectTypes(const std::initializer_list<std::string_view> &parameterNames, Parameters ...parameters) const;

        /**
            @brief Check if class is valid.
            @return State of class
        */
        [[nodiscard]] inline bool IsValid() const { return _data != nullptr; }

        /**
            @brief Check if class is valid.

            Alias of IsValid().

            @return State of class
        */
        [[nodiscard]] inline bool Alive() const { return IsValid(); }

        /**
            @brief Check if class is valid.

            Alias of IsValid().

            @return State of class
        */
        inline operator bool() const { return IsValid(); }


#ifdef BNM_ALLOW_STR_METHODS

        /**
            @brief Get full class name.

            Returns string with full class name: [Image name]::[Namespace]::[Name]->[Inner name]

            @return Class full name or "Dead class"
        */
        [[nodiscard]] inline std::string str() const {
            if (!_data) return BNM_OBFUSCATE(DBG_BNM_MSG_Class_str_nullptr);
            TryInit();
            std::string data{};
            if (_data->declaringType) {
                data += Class(_data->declaringType).str() + BNM_OBFUSCATE("->");
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
        static IL2CPP::Il2CppObject *BoxObject(IL2CPP::Il2CppClass*, void*);
        static IL2CPP::Il2CppArray *ArrayNew(IL2CPP::Il2CppClass*, IL2CPP::il2cpp_array_size_t);
        static void *NewListInstance();
        static Class GetListClass();
        friend CompileTimeClass;
    };

    /**
        @brief Stores class info at compile time.

        A structure for storing data at compile time and then searching for it during code execution.
    */
    struct CompileTimeClass {
        struct _BaseInfo;
        enum class ModifierType : uint8_t {
            None, Array, Pointer, Reference
        };
        /// @cond
        std::list<_BaseInfo *> _stack{};
        union {
            Class _loadedClass{};
            Defaults::Internal::ClassType *_reference;
        };
        uint8_t _autoFree : 1{true}, _isReferenced{false};
        /// @endcond

        /**
            @brief Get BNM::Class from CompileTimeClass.

            Tries to get BNM::Class from information, that CompileTimeClass has.

            @return BNM::Class
        */
        [[nodiscard]] Class ToClass() const;

        /**
            @brief Get BNM::Class from CompileTimeClass.

            Tries to get BNM::Class from information, that CompileTimeClass has.

            @return BNM::Class
        */
        Class ToClass();

        /**
            @brief Get Il2CppType from CompileTimeClass.

            Alias for ToClass().GetIl2CppType()

            @return Il2CppType
        */
        [[nodiscard]] IL2CPP::Il2CppType *ToIl2CppType() const;

        /**
            @brief Get Il2CppClass from CompileTimeClass.

            Alias for ToClass().GetClass()

            @return Il2CppClass
        */
        [[nodiscard]] IL2CPP::Il2CppClass *ToIl2CppClass() const;
        operator IL2CPP::Il2CppType*() const;
        operator IL2CPP::Il2CppClass*() const;
        operator Class() const;
        void Free();
        /// @cond
        enum class _BaseType : uint8_t {
            // We explicitly set the values, because the code uses an array
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
        /// @endcond
    };

    /**
        @brief Struct for building CompileTimeClass.

        A structure for building CompileTimeClass structure.
    */
    struct CompileTimeClassBuilder {
        CompileTimeClass _data{};

        /**
            @brief Create builder.

            Creates builder using class namespace, name and image name.

            @param _namespace Target class namespace
            @param _name Target class name
            @param _imageName Target class image name
            @param autoFree Should CompileTimeClass be freed after class was found or shouldn't (used mostly internally).
        */
        inline CompileTimeClassBuilder(const char *_namespace, const char *_name, const char *_imageName = nullptr, bool autoFree = true) {
            _data._autoFree = autoFree;
            auto info = (CompileTimeClass::_ClassInfo *) BNM_malloc(sizeof(CompileTimeClass::_ClassInfo));
            *info = CompileTimeClass::_ClassInfo{_namespace, _name, _imageName};
            _data._stack.push_back(info);
        }

        /**
            @brief Specify inner class name.

            Adds inner class to stack of CompileTimeClass.

            @param _name Target inner class name

            @return Reference to current builder
        */
        inline CompileTimeClassBuilder &Class(const char *_name) {
            auto info = (CompileTimeClass::_InnerInfo *) BNM_malloc(sizeof(CompileTimeClass::_InnerInfo));
            *info = CompileTimeClass::_InnerInfo{_name};
            _data._stack.push_back(info);
            return *this;
        }

        /**
            @brief Specify modifier of target class.

            Adds modifier to target class: array, pointer or reference.

            @param type Target class modifier

            @return Reference to current builder
        */
        inline CompileTimeClassBuilder &Modifier(CompileTimeClass::ModifierType type) {
            auto modifier = (CompileTimeClass::_ModifierInfo *) BNM_malloc(sizeof(CompileTimeClass::_ModifierInfo));
            *modifier = CompileTimeClass::_ModifierInfo{type};
            _data._stack.push_back(modifier);
            return *this;
        }

        /**
            @brief Specify generic types of target class.

            Adds template types to target class.

            @param templateTypes List of template types

            @return Reference to current builder
        */
        inline CompileTimeClassBuilder &Generic(const std::initializer_list<CompileTimeClass> &templateTypes) {
            auto generic = (CompileTimeClass::_GenericInfo *) BNM_malloc(sizeof(CompileTimeClass::_GenericInfo));
            memset(generic, 0, sizeof(CompileTimeClass::_GenericInfo));
            *generic = CompileTimeClass::_GenericInfo{templateTypes};
            _data._stack.push_back(generic);
            return *this;
        }


        /**
            @brief Build CompileTimeClass.

            Builds CompileTimeClass.

            @return New CompileTimeClass object
        */
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

    /**
        @brief Check if object is inherited from class.

        Class is specified using Il2CppClass.

        @param object Any pointer
        @param _class Il2CppClass pointer
        @tparam T Object type

        @return True if object is inherited from target class.
    */
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, IL2CPP::Il2CppClass *_class) { return IsA<BNM::IL2CPP::Il2CppObject *>((IL2CPP::Il2CppObject *)object, _class); }

    /**
        @brief Check if Il2CppObject is inherited from class.

        @param object Il2CppObject pointer
        @param _class Il2CppClass pointer

        @return True if Il2CppObject is inherited from target class.
    */
    template<>
    bool IsA<IL2CPP::Il2CppObject *>(IL2CPP::Il2CppObject *object, IL2CPP::Il2CppClass *_class);

    /**
        @brief Check if object is inherited from class.

        Class is specified using BNM::Class

        @param object Any pointer
        @param _class BNM::Class
        @tparam T Object type

        @return True if object is inherited from target class.
    */
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, Class _class) { return IsA(object, _class.GetClass()); }

    /**
        @brief Check if object is inherited from object's class.

        Class is specified using object of target class.

        @param object Any pointer
        @param _object Il2CppObject pointer
        @tparam T Object type

        @return True if object is inherited from target class.
    */
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, IL2CPP::Il2CppObject *_object) { if (!_object) return false; return IsA(object, _object->klass); }

    /**
        @brief Check if object is inherited from object's class.

        Class is specified using MonoType.

        @param object Any pointer
        @param _type MonoType pointer
        @tparam T Object type

        @return True if object is inherited from target type.
    */
    template<typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    bool IsA(T object, MonoType *_type) { return IsA(object, Class(_type)); }

#ifdef BNM_OLD_GOOD_DAYS
    /**
        @brief For people how love old BNM
    */
    typedef Class LoadClass;
#endif

}
// NOLINTEND