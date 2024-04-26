#include <BNM/Class.hpp>
#include <BNM/FieldBase.hpp>
#include <BNM/MethodBase.hpp>
#include <BNM/PropertyBase.hpp>
#include <BNM/EventBase.hpp>
#include <BNM/DebugMessages.hpp>
#include "Internals.hpp"

using namespace BNM;


Class::Class(const IL2CPP::Il2CppObject *obj) {
    if (!obj) return;
    _data = obj->klass;
}

Class::Class(const IL2CPP::Il2CppType *type) {
    if (!type) return;
    _data = Internal::il2cppMethods.il2cpp_class_from_il2cpp_type(type);
}

Class::Class(const MonoType *type) {
    if (!type) return;
    _data = Internal::il2cppMethods.il2cpp_class_from_il2cpp_type(type->type);
}

Class::Class(const CompileTimeClass &type) { _data = type; }

IL2CPP::Il2CppClass *TryGetClassWithoutImage(const std::string_view &_namespace, const std::string_view &_name) {
    auto &assemblies = *Internal::Assembly$$GetAllAssemblies();

    for (auto assembly : assemblies) {
        auto image = Internal::il2cppMethods.il2cpp_assembly_get_image(assembly);
        if (auto _data = Internal::TryGetClassInImage(image, _namespace, _name); _data) return _data;
    }

    return nullptr;
}

Class::Class(const std::string_view &_namespace, const std::string_view &_name) {
    if (_data = TryGetClassWithoutImage(_namespace, _name); _data) return;
    BNM_LOG_WARN(DBG_BNM_MSG_Class_Constructor_NotFound, _namespace.data(), _name.data());
}

IL2CPP::Il2CppClass *TryGetClassWithImage(const std::string_view &_namespace, const std::string_view &_name, IL2CPP::Il2CppImage *image) {
    if (auto _data = Internal::TryGetClassInImage(image, _namespace, _name); _data) return _data;
    return nullptr;
}

Class::Class(const std::string_view &_namespace, const std::string_view &_name, const BNM::Image &image) {
    if (!image) {
        BNM_LOG_WARN(DBG_BNM_MSG_Class_Constructor_Image_Warn, image.str().data(), _namespace.data(), _name.data());
        _data = nullptr;
        return;
    }

    if (_data = Internal::TryGetClassInImage(image, _namespace, _name); _data) return;

    BNM_LOG_WARN(DBG_BNM_MSG_Class_Constructor_Image_NotFound, image.str().data(), _namespace.data(), _name.data());
}

std::vector<Class> Class::GetInnerClasses(bool includeParent) const {
    if (!_data) return {};
    TryInit();
    std::vector<Class> ret{};
    auto curClass = _data;

    do {
        for (uint16_t i = 0; i < curClass->nested_type_count; ++i) ret.emplace_back(curClass->nestedTypes[i]);
        if (includeParent) curClass = curClass->parent;
        else break;
    } while (curClass);

    return std::move(ret);
}

std::vector<FieldBase> Class::GetFields(bool includeParent) const {
    if (!_data) return {};
    TryInit();
    std::vector<FieldBase> ret{};
    auto curClass = _data;

    do {
        auto end = curClass->fields + curClass->field_count;
        for (IL2CPP::FieldInfo *currentField = curClass->fields; currentField != end; ++currentField) ret.emplace_back(currentField);
        if (includeParent) curClass = curClass->parent;
        else break;
    } while (curClass);

    return std::move(ret);
}

std::vector<MethodBase> Class::GetMethods(bool includeParent) const {
    if (!_data) return {};
    TryInit();
    std::vector<MethodBase> ret{};
    auto curClass = _data;

    do {
        for (uint16_t i = 0; i < curClass->method_count; ++i) ret.emplace_back(curClass->methods[i]);
        if (includeParent) curClass = curClass->parent;
        else break;
    } while (curClass);

    return std::move(ret);
}

std::vector<PropertyBase> Class::GetProperties(bool includeParent) const {
    if (!_data) return {};
    TryInit();
    std::vector<PropertyBase> ret{};
    auto curClass = _data;

    do {
        auto end = curClass->properties + curClass->property_count;
        for (auto currentProperty = curClass->properties; currentProperty != end; ++currentProperty) ret.emplace_back(currentProperty);
        if (includeParent) curClass = curClass->parent;
        else break;
    } while (curClass);

    return std::move(ret);
}
std::vector<EventBase> Class::GetEvents(bool includeParent) const {
    if (!_data) return {};
    TryInit();
    std::vector<EventBase> ret{};
    auto curClass = _data;

    do {
        auto end = curClass->events + curClass->event_count;
        for (auto currentEvent = curClass->events; currentEvent != end; ++currentEvent) ret.emplace_back(currentEvent);
        if (includeParent) curClass = curClass->parent;
        else break;
    } while (curClass);

    return std::move(ret);
}

MethodBase Class::GetMethod(const std::string_view &name, int parameters) const {
    if (!_data) return {};
    TryInit();

    auto method = Internal::IterateMethods(*this, [&name, &parameters](IL2CPP::MethodInfo *method) {
        return name == method->name && (method->parameters_count == parameters || parameters == -1);
    });

    if (method != nullptr) return method;

    BNM_LOG_WARN(DBG_BNM_MSG_Class_GetMethod_Count_NotFound, _data->namespaze, _data->name, name.data(), parameters);
    return {};
}

MethodBase Class::GetMethod(const std::string_view &name, const std::initializer_list<std::string_view> &parametersName) const {
    if (!_data) return {};
    TryInit();

    auto parameters = (uint8_t) parametersName.size();

    auto method = Internal::IterateMethods(*this, [&name, &parameters, &parametersName](IL2CPP::MethodInfo *method) {
        if (name != method->name || method->parameters_count != parameters) return false;
        for (uint8_t i = 0; i < parameters; ++i) if (Internal::il2cppMethods.il2cpp_method_get_param_name(method, i) != parametersName.begin()[i]) return false;
        return true;
    });

    if (method != nullptr) return method;

    BNM_LOG_WARN(DBG_BNM_MSG_Class_GetMethod_Names_NotFound, _data->namespaze, _data->name, name.data(), parameters);
    return {};
}

MethodBase Class::GetMethod(const std::string_view &name, const std::initializer_list<BNM::CompileTimeClass> &parametersType) const {
    if (!_data) return {};
    TryInit();
    auto parameters = (uint8_t) parametersType.size();

    auto method = Internal::IterateMethods(*this, [&name, parameters, &parametersType](IL2CPP::MethodInfo *method) {
        if (name != method->name || method->parameters_count != parameters) return false;
        for (uint8_t i = 0; i < parameters; ++i) {
#if UNITY_VER < 212
            auto param = (method->parameters + i)->parameter_type;
#else
            auto param = method->parameters[i];
#endif

            if (Class(param).GetClass() != parametersType.begin()[i].ToIl2CppClass()) return false;
        }
        return true;
    });

    if (method != nullptr) return method;

    BNM_LOG_WARN(DBG_BNM_MSG_Class_GetMethod_Types_NotFound, _data->namespaze, _data->name, name.data(), parameters);
    return {};
}

PropertyBase Class::GetProperty(const std::string_view &name) const {
    if (!_data) return {};
    TryInit();
    auto curClass = _data;

    do {
        auto end = curClass->properties + curClass->property_count;
        for (auto currentProperty = curClass->properties; currentProperty != end; ++currentProperty) if (name == currentProperty->name) return currentProperty;
        curClass = curClass->parent;
    } while (curClass);

    BNM_LOG_WARN(DBG_BNM_MSG_Class_GetProperty_NotFound, str().c_str(), name.data());
    return {};
}

Class Class::GetInnerClass(const std::string_view &name) const {
    if (!_data) return {};
    TryInit();
    auto curClass = _data;

    do {
        for (uint16_t i = 0; i < curClass->nested_type_count; ++i) {
            auto cls = curClass->nestedTypes[i];
            if (name == cls->name) return cls;
        }
        curClass = curClass->parent;
    } while (curClass);

    BNM_LOG_WARN(DBG_BNM_MSG_Class_GetInnerClass_NotFound, _data->namespaze, _data->name, name.data());
    return {};
}

FieldBase Class::GetField(const std::string_view &name) const {
    if (!_data) return {};
    TryInit();
    auto curClass = _data;

    do {
        auto end = curClass->fields + curClass->field_count;
        for (auto currentField = curClass->fields; currentField != end; ++currentField) {
            if (name != currentField->name) continue;
            return currentField;
        }
        curClass = curClass->parent;
    } while (curClass);

    BNM_LOG_WARN(DBG_BNM_MSG_Class_GetField_NotFound, _data->namespaze, _data->name, name.data());
    return {};
}

BNM::EventBase Class::GetEvent(const std::string_view &name) const {
    if (!_data) return {};
    TryInit();
    auto curClass = _data;

    do {
        auto end = curClass->events + curClass->event_count;
        for (auto currentEvent = curClass->events; currentEvent != end; ++currentEvent) {
            if (name != currentEvent->name) continue;
            return currentEvent;
        }
        curClass = curClass->parent;
    } while (curClass);

    BNM_LOG_WARN(DBG_BNM_MSG_Class_GetEvent_NotFound, _data->namespaze, _data->name, name.data());
    return {};
}

Class Class::GetParent() const {
    if (!_data) return {};
    TryInit();
    return _data->parent;
}

Class Class::GetArray() const {
    if (!_data) return {};
    TryInit();
    return Internal::il2cppMethods.il2cpp_array_class_get(_data, 1);
}

Class Class::GetPointer() const {
    if (!_data) return {};
    TryInit();
    return Internal::GetPointer(*this);
}

Class Class::GetReference() const {
    if (!_data) return {};
    TryInit();
    return Internal::GetReference(*this);
}


Class Class::GetGeneric(const std::initializer_list<CompileTimeClass> &templateTypes) const {
    if (!_data) return {};
    TryInit();
    return Internal::TryMakeGenericClass(*this, templateTypes);
}

IL2CPP::Il2CppType *Class::GetIl2CppType() const {
    if (!_data) return nullptr;
    TryInit();
#if UNITY_VER > 174
    return (IL2CPP::Il2CppType *)&_data->byval_arg;
#else
    return (IL2CPP::Il2CppType *)_data->byval_arg;
#endif
}

MonoType *Class::GetMonoType() const {
    if (!_data) return nullptr;
    TryInit();
    return (MonoType *) Internal::il2cppMethods.il2cpp_type_get_object(GetIl2CppType());
}

BNM::CompileTimeClass Class::GetCompileTimeClass() const {
    TryInit();
    return {._autoFree = false, ._loadedClass = *this};
}

Class::operator BNM::CompileTimeClass() const { return GetCompileTimeClass(); }

BNM::IL2CPP::Il2CppObject *Class::CreateNewInstance() const {
    if (!_data) return nullptr;
    TryInit();

    if ((_data->flags & (0x00000080 | 0x00000020))) // TYPE_ATTRIBUTE_ABSTRACT | TYPE_ATTRIBUTE_INTERFACE
        BNM_LOG_WARN(DBG_BNM_MSG_Class_CreateNewInstance_Abstract_Warn, str().c_str());

    auto obj = Internal::il2cppMethods.il2cpp_object_new(_data);
    if (obj) memset((char*)obj + sizeof(IL2CPP::Il2CppObject), 0, _data->instance_size - sizeof(IL2CPP::Il2CppObject));
    return (BNM::IL2CPP::Il2CppObject *) obj;
}

// Попробовать инициализировать класс, если он жив
void Class::TryInit() const { if (_data) Internal::Class$$Init(_data); }

IL2CPP::Il2CppObject *Class::ObjBox(IL2CPP::Il2CppClass *_data, void *data) {
    return Internal::il2cppMethods.il2cpp_value_box(_data, data);
}

IL2CPP::Il2CppArray *Class::ArrayNew(IL2CPP::Il2CppClass *cls, IL2CPP::il2cpp_array_size_t length) {
    return Internal::il2cppMethods.il2cpp_array_new(cls, length);
}

void *Class::NewListInstance() {
    return Internal::customListTemplateClass.CreateNewInstance();
}

namespace CompileTimeClassProcessors {
    typedef void (*ProcessorType)(CompileTimeClass &target, CompileTimeClass::_BaseInfo *info);

    void Warn(CompileTimeClass&, CompileTimeClass::_BaseInfo*) {
        BNM_LOG_WARN(DBG_BNM_MSG_CompileTimeClass_ToClass_default_Warn);
    }
    
    // _ClassInfo
    void ProcessClassInfo(CompileTimeClass &target, CompileTimeClass::_BaseInfo *info) {
        auto classInfo = (CompileTimeClass::_ClassInfo *) info;

        if (target._loadedClass) target._loadedClass = target._loadedClass.GetInnerClass(classInfo->_name);
        else {
            auto _namespace = classInfo->_namespace ? classInfo->_namespace : OBFUSCATE_BNM("");
            if (classInfo->_imageName && strlen(classInfo->_imageName)) target._loadedClass = TryGetClassWithImage(_namespace, classInfo->_name, Image(classInfo->_imageName));
            else target._loadedClass = TryGetClassWithoutImage(_namespace, classInfo->_name);
        }
    }

    // _ModifierInfo
    void ProcessModifierInfo(CompileTimeClass &target, CompileTimeClass::_BaseInfo *info) {
        if (!target._loadedClass) {
            BNM_LOG_WARN(DBG_BNM_MSG_CompileTimeClass_ToClass_Modifier_Warn);
            return;
        }
        
        auto modifierInfo = (CompileTimeClass::_ModifierInfo *) info;
        
        switch (modifierInfo->_modifierType) {
            case CompileTimeClass::ModifierType::Pointer: {
                target._loadedClass = target._loadedClass.GetPointer();
            } break;
            case CompileTimeClass::ModifierType::Reference: {
                target._loadedClass = target._loadedClass.GetReference();
            } break;
            case CompileTimeClass::ModifierType::Array: {
                target._loadedClass = target._loadedClass.GetArray();
            } break;
            case CompileTimeClass::ModifierType::None: break;
        }
    }

    // _GenericInfo
    void ProcessGenericInfo(CompileTimeClass &target, CompileTimeClass::_BaseInfo *info) {
        if (!target._loadedClass) {
            BNM_LOG_WARN(DBG_BNM_MSG_CompileTimeClass_ToClass_Generic_Warn);
            return;
        }
        
        auto genericInfo = (CompileTimeClass::_GenericInfo *) info;

        // Экономия памяти, даже если пользователь не хочет
        for (auto &type : genericInfo->_types) ((BNM::CompileTimeClass &)type)._autoFree = true;
        target._loadedClass = Internal::TryMakeGenericClass(target._loadedClass, genericInfo->_types);
        genericInfo->_types.clear();
        genericInfo->_types.shrink_to_fit();
    }

    ProcessorType processors[(uint8_t) CompileTimeClass::_BaseType::MaxCount] = {
            Warn,
            ProcessClassInfo,
            ProcessModifierInfo,
            ProcessGenericInfo,
    };
}


Class CompileTimeClass::ToClass() {
    if (_loadedClass) return _loadedClass;
    if (_stack.empty()) return {(IL2CPP::Il2CppClass *) nullptr};

    for (auto info : _stack) {
        auto index = (uint8_t) info->_baseType;
        if (index >= (uint8_t) CompileTimeClass::_BaseType::MaxCount) {
            BNM_LOG_WARN(DBG_BNM_MSG_CompileTimeClass_ToClass_OoB_Warn, (size_t)index);
            continue;
        }
        CompileTimeClassProcessors::processors[index](*this, (_BaseInfo *) info);
    }

    if (_autoFree) {
        _autoFree = false;
        Free();
    }

    return _loadedClass;
}
Class CompileTimeClass::ToClass() const {
    return ((CompileTimeClass *)this)->ToClass();
}
CompileTimeClass::operator Class() { return ToClass(); }
CompileTimeClass::operator Class() const { return ToClass(); }

IL2CPP::Il2CppType *CompileTimeClass::ToIl2CppType() { return ToClass().GetIl2CppType(); }
IL2CPP::Il2CppType *CompileTimeClass::ToIl2CppType() const { return ToClass().GetIl2CppType(); }
CompileTimeClass::operator IL2CPP::Il2CppType*() { return ToIl2CppType(); }
CompileTimeClass::operator IL2CPP::Il2CppType*() const { return ToIl2CppType(); }

IL2CPP::Il2CppClass *CompileTimeClass::ToIl2CppClass() { return ToClass().GetClass(); }
IL2CPP::Il2CppClass *CompileTimeClass::ToIl2CppClass() const { return ToClass().GetClass(); }
CompileTimeClass::operator IL2CPP::Il2CppClass*() { return ToIl2CppClass(); }
CompileTimeClass::operator IL2CPP::Il2CppClass*() const { return ToIl2CppClass(); }
void CompileTimeClass::Free() {
    if (_autoFree) return;

    _autoFree = true;

    for (auto info : _stack) delete info;
    _stack.clear();
}