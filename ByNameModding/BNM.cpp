#include <list>
#include <locale>
#include <atomic>
#include <array>
#include <thread>
#include <functional>
#include <algorithm>
#include <shared_mutex>
#include <dlfcn.h>
#include "BNM.hpp"
#include "BNM_data/utf8.h"

namespace BNM_Internal {
    using namespace BNM;
    using namespace Structures;

    // Internal variables
    bool bnmLoaded = false;
    void *il2cppLibraryHandle{};
    const char *il2cppLibraryAbsolutePath{};
    BNM_PTR il2cppLibraryAbsoluteAddress{};

#pragma pack(push, 1)
    // List with variables from the il2cpp virtual machine
    struct {
        BNM::LoadClass Object;
        BNM::Method<void *> Interlocked$$CompareExchange{};
        BNM::Method<BNM::MonoType *> RuntimeType$$MakeGenericType{};
        BNM::Method<BNM::MonoType *> RuntimeType$$MakePointerType{};
        BNM::Method<BNM::MonoType *> RuntimeType$$make_byref_type{};
        BNM::Method<IL2CPP::Il2CppReflectionMethod *> RuntimeMethodInfo$$MakeGenericMethod_impl{};
        Mono::monoString **String$$Empty{};
    } vmData;
#pragma pack(pop)
    // il2cpp methods to avoid finding them every BNM request
    struct {
        IL2CPP::Il2CppImage *(*il2cpp_get_corlib)(){};
        IL2CPP::Il2CppClass *(*il2cpp_class_from_name)(const IL2CPP::Il2CppImage *, const char *, const char *){};
        IL2CPP::Il2CppImage *(*il2cpp_assembly_get_image)(const IL2CPP::Il2CppAssembly *){};
        IL2CPP::Il2CppClass *(*il2cpp_image_get_class)(const IL2CPP::Il2CppImage *, unsigned int){};
        const char *(*il2cpp_method_get_param_name)(const IL2CPP::MethodInfo *, uint32_t){};
        IL2CPP::Il2CppClass *(*il2cpp_class_from_il2cpp_type)(const IL2CPP::Il2CppType *){};
        IL2CPP::Il2CppClass *(*il2cpp_array_class_get)(const IL2CPP::Il2CppClass *, uint32_t){};
        IL2CPP::Il2CppObject *(*il2cpp_type_get_object)(const IL2CPP::Il2CppType *){};
        IL2CPP::Il2CppObject *(*il2cpp_object_new)(const IL2CPP::Il2CppClass *){};
        IL2CPP::Il2CppObject *(*il2cpp_value_box)(const IL2CPP::Il2CppClass *, void *){};
        IL2CPP::Il2CppArray *(*il2cpp_array_new)(const IL2CPP::Il2CppClass *, IL2CPP::il2cpp_array_size_t){};
        void (*il2cpp_field_static_get_value)(const IL2CPP::FieldInfo *, void *){};
        void (*il2cpp_field_static_set_value)(const IL2CPP::FieldInfo *, void *){};
        Mono::monoString *(*il2cpp_string_new)(const char *){};
        void *(*il2cpp_resolve_icall)(const char *){};

#ifdef BNM_DEPRECATED
        IL2CPP::Il2CppDomain *(*il2cpp_domain_get)(){};
        IL2CPP::Il2CppThread *(*il2cpp_thread_attach)(const IL2CPP::Il2CppDomain *){};
        IL2CPP::Il2CppThread *(*il2cpp_thread_current)(){};
        void (*il2cpp_thread_detach)(const IL2CPP::Il2CppThread *){};
#endif
    } il2cppMethods;

#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
    std::shared_mutex findClassesMutex{};
    std::mutex addClassMutex{}, modClassMutex{};
#endif

    std::list<void(*)()> onIl2CppLoaded{};
    std::list<void(*)()> &GetEvents() { return onIl2CppLoaded; }

    std::string_view constructorName = OBFUSCATE_BNM(".ctor");
    BNM::LoadClass customListTemplateClass{};
    std::map<uint32_t, BNM::LoadClass> customListsMap{};
    int32_t finalizerSlot = -1;

    // Methods for new classes and for normal mode
    AssemblyVector *(*Assembly$$GetAllAssemblies)(){};
    void Empty() {}

#if !BNM_DISABLE_NEW_CLASSES
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
    std::shared_mutex classesFindAccessMutex{};
#endif
    // List with all new classes that BNM need add at runtime
    std::vector<NEW_CLASSES::NewClass *> *newClassesVector = nullptr;

    // List with all classes that BNM need modify at runtime
    std::vector<MODIFY_CLASSES::TargetClass *> *modClassesVector = nullptr;

    IL2CPP::Il2CppClass *(*old_Class$$FromIl2CppType)(IL2CPP::Il2CppType *type){};
    IL2CPP::Il2CppClass *Class$$FromIl2CppType(IL2CPP::Il2CppType *type);

    IL2CPP::Il2CppClass *(*old_Type$$GetClassOrElementClass)(IL2CPP::Il2CppType *type){};
    IL2CPP::Il2CppClass *Type$$GetClassOrElementClass(IL2CPP::Il2CppType *type);

    IL2CPP::Il2CppClass *(*old_Class$$FromName)(IL2CPP::Il2CppImage *image, const char *ns, const char *name){};
    IL2CPP::Il2CppClass *Class$$FromName(IL2CPP::Il2CppImage *image, const char *namespaze, const char *name);

    void ModifyClasses();
    void InitNewClasses();
#if UNITY_VER <= 174
    // Требуются, потому что в Unity 2017 и ниже в образах (image) и сборках (assembly) они хранятся по номерам

    IL2CPP::Il2CppImage *(*old_GetImageFromIndex)(IL2CPP::ImageIndex index);
    IL2CPP::Il2CppImage *new_GetImageFromIndex(IL2CPP::ImageIndex index);

    IL2CPP::Il2CppAssembly *Assembly$$Load(const char *name);
#endif

#endif

    void (*old_Image$$GetTypes)(const IL2CPP::Il2CppImage *image, bool exportedOnly, ClassVector *target){};
    void Image$$GetTypes(const IL2CPP::Il2CppImage *image, bool exportedOnly, ClassVector *target);

    void (*Class$$Init)(IL2CPP::Il2CppClass *klass){};

    void (*old_BNM_il2cpp_init)(const char *){};
    void BNM_il2cpp_init(const char *domain_name);

    void SetupBNM();

#ifndef BNM_DISABLE_AUTO_LOAD
    __attribute__((constructor)) void PrepareBNM();
#endif

    bool InitLibraryHandle(void *handle, const char *path = nullptr, bool external = false);

    // Methods for converting C# strings (monoString)
    typedef std::basic_string<IL2CPP::Il2CppChar> string16;
    std::string Utf16ToUtf8(IL2CPP::Il2CppChar *utf16String, size_t length) {
        std::string utf8String;
        utf8String.reserve(length);
        utf8::unchecked::utf16to8(utf16String, utf16String + length, std::back_inserter(utf8String));
        return utf8String;
    }
    string16 Utf8ToUtf16(const char *utf8String, size_t length) {
        string16 utf16String;
        if (utf8::is_valid(utf8String, utf8String + length)) {
            utf16String.reserve(length);
            utf8::unchecked::utf8to16(utf8String, utf8String + length, std::back_inserter(utf16String));
        }
        return utf16String;
    }

    IL2CPP::Il2CppClass *TryGetClassInImage(const IL2CPP::Il2CppImage *image, const std::string_view &namespaze, const std::string_view &name);

    LoadClass TryMakeGenericClass(LoadClass genericType, const std::vector<RuntimeTypeGetter> &templateTypes) {
        using namespace BNM::Operators;
        if (!vmData.RuntimeType$$MakeGenericType) return {};
        auto monoType = genericType.GetMonoType();
        auto monoGenericsList = Mono::monoArray<MonoType *>::Create(templateTypes.size());
        for (size_t i = 0; i < templateTypes.size(); ++i) (*monoGenericsList)[i] = templateTypes[i].ToLC().GetMonoType();

        LoadClass typedGenericType = vmData.RuntimeType$$MakeGenericType(monoType, monoGenericsList);

        monoGenericsList->Destroy();

        return typedGenericType;
    }

    MethodBase TryMakeGenericMethod(const MethodBase &genericMethod, const std::vector<RuntimeTypeGetter> &templateTypes) {
        using namespace BNM::Operators;
        if (!vmData.RuntimeMethodInfo$$MakeGenericMethod_impl || !genericMethod.GetInfo()->is_generic) return {};
        IL2CPP::Il2CppReflectionMethod reflectionMethod;
        reflectionMethod.method = genericMethod.GetInfo();
        auto monoGenericsList = Mono::monoArray<MonoType *>::Create(templateTypes.size());
        for (size_t i = 0; i < templateTypes.size(); ++i) (*monoGenericsList)[i] = templateTypes[i].ToLC().GetMonoType();

        MethodBase typedGenericMethod = ((&reflectionMethod)->*vmData.RuntimeMethodInfo$$MakeGenericMethod_impl)(monoGenericsList)->method;

        monoGenericsList->Destroy();

        return typedGenericMethod;
    }

    LoadClass GetPointer(LoadClass target) {
        using namespace BNM::Operators;
        if (!vmData.RuntimeType$$MakePointerType) return {};
        return vmData.RuntimeType$$MakePointerType(target.GetMonoType());
    }

    LoadClass GetReference(LoadClass target) {
        using namespace BNM::Operators;
        if (!vmData.RuntimeType$$make_byref_type) return {};
        return (target.GetMonoType()->*vmData.RuntimeType$$make_byref_type)();
    }

    template <class CompareMethod>
    IL2CPP::MethodInfo *IterateMethods(LoadClass target, CompareMethod compare) {
        auto curClass = target.klass;
        do {
            for (uint16_t i = 0; i < curClass->method_count; ++i) {
                auto method = curClass->methods[i];
                if (compare((IL2CPP::MethodInfo *)method)) return (IL2CPP::MethodInfo *) method;
            }
            curClass = curClass->parent;
        } while (curClass);
        return nullptr;
    }
}

namespace BNM {
    bool IsLoaded() { return BNM_Internal::bnmLoaded; }

    namespace Structures::Mono {
        void *CompareExchange4List(void *syncRoot) {  // The only normal way to call CompareExchange for syncRoot for monoList
            if (BNM_Internal::vmData.Interlocked$$CompareExchange) BNM_Internal::vmData.Interlocked$$CompareExchange((void **)&syncRoot, (void *)BNM_Internal::vmData.Object.CreateNewInstance(), (void *)nullptr);
            return syncRoot;
        }

        std::string monoString::str() {
            BNM_CHECK_SELF("ERROR: monoString dead");
            if (!length) return {};
            return BNM_Internal::Utf16ToUtf8(chars, length);
        }

        unsigned int monoString::GetHash() const {
            BNM_CHECK_SELF(0);
            const IL2CPP::Il2CppChar *p = chars;
            unsigned int h = 0;
            for (int i = 0; i < length; ++i) h = (h << 5) - h + *p; p++;
            return h;
        }

        // Create basic C# string like il2cpp
        monoString *monoString::Create(const char *str) {
            const size_t length = strlen(str);
            const size_t utf16Size = sizeof(IL2CPP::Il2CppChar) * length;
            auto ret = (monoString *) malloc(sizeof(monoString) + utf16Size);
            memset(ret, 0, sizeof(monoString) + utf16Size);
            ret->length = (int)length;
            auto u16 = BNM_Internal::Utf8ToUtf16(str, ret->length);
            memcpy(ret->chars, u16.data(), utf16Size);
            auto empty = Empty();
            if (empty) ret->klass = empty->klass;
            return (monoString *)ret;
        }
        monoString *monoString::Create(const std::string &str) { return Create(str.c_str()); }

        monoString *monoString::Empty() {
            if (!BNM_Internal::vmData.String$$Empty) return {};
            return *BNM_Internal::vmData.String$$Empty;
        }

#ifdef BNM_ALLOW_SELF_CHECKS
        bool monoString::SelfCheck() volatile const {
            auto *me = (monoString *)this;
            if (me != nullptr) return true;
            BNM_LOG_ERR("[monoString::SelfCheck] Trying to use a dead string!");
            return false;
        }
#endif

        void monoString::Destroy() {
            length = 0;
            free(this);
        }
    }

    /*** LoadClass ***/
    LoadClass::LoadClass(const IL2CPP::Il2CppClass *clazz) { klass = (IL2CPP::Il2CppClass *)clazz; }

    LoadClass::LoadClass(const IL2CPP::Il2CppObject *obj) {
        if (!obj) return;
        klass = obj->klass;
    }

    LoadClass::LoadClass(const IL2CPP::Il2CppType *type) {
        if (!type) return;
        klass = BNM_Internal::il2cppMethods.il2cpp_class_from_il2cpp_type(type);
    }

    LoadClass::LoadClass(const MonoType *type) {
        if (!type) return;
        klass = BNM_Internal::il2cppMethods.il2cpp_class_from_il2cpp_type(type->type);
    }

    LoadClass::LoadClass(RuntimeTypeGetter type) { klass = type; }

    LoadClass::LoadClass(const std::string_view &namespaze, const std::string_view &name) {
        auto assemblies = BNM_Internal::Assembly$$GetAllAssemblies();

        for (auto assembly : *assemblies) {
            auto image = BNM_Internal::il2cppMethods.il2cpp_assembly_get_image(assembly);
            if (klass = BNM_Internal::TryGetClassInImage(image, namespaze, name); klass) return;
        }
        BNM_LOG_WARN("Class: [%s]::[%s] - not found.", namespaze.data(), name.data());
    }

    LoadClass::LoadClass(const std::string_view &namespaze, const std::string_view &name, const std::string_view &dllName) {
        IL2CPP::Il2CppImage *image = nullptr;
        auto assemblies = BNM_Internal::Assembly$$GetAllAssemblies();

        for (auto assembly : *assemblies) {
            auto currentImage = BNM_Internal::il2cppMethods.il2cpp_assembly_get_image(assembly);
            if (dllName != currentImage->nameNoExt && dllName != currentImage->name) continue;
            image = currentImage;
            break;
        }


        if (!image) {
            BNM_LOG_WARN("Class: [%s]::[%s]::[%s] - dll not found.", dllName.data(), namespaze.data(), name.data());
            klass = nullptr;
            return;
        }

        if (klass = BNM_Internal::TryGetClassInImage(image, namespaze, name); klass) return;

        BNM_LOG_WARN("Class: [%s]::[%s]::[%s] - not found.", dllName.data(), namespaze.data(), name.data());
    }

    LoadClass::LoadClass(const std::string_view &namespaze, const std::string_view &name, const IL2CPP::Il2CppImage *image) {
        if (klass = BNM_Internal::TryGetClassInImage(image, namespaze, name); klass) return;

        BNM_LOG_WARN("Class: [%s]::[%s]::[%s] - not found.", image->name, namespaze.data(), name.data());
    }

    LoadClass::LoadClass(const std::string_view &namespaze, const std::string_view &name, const IL2CPP::Il2CppAssembly *assembly) {
        *this = LoadClass(namespaze, name, BNM_Internal::il2cppMethods.il2cpp_assembly_get_image(assembly));
    }

    std::vector<LoadClass> LoadClass::GetInnerClasses(bool includeParent) const {
        if (!klass) return {};
        TryInit();
        std::vector<LoadClass> ret{};
        auto curClass = klass;

        do {
            for (uint16_t i = 0; i < curClass->nested_type_count; ++i) ret.emplace_back(curClass->nestedTypes[i]);
            if (includeParent) curClass = curClass->parent;
            else break;
        } while (curClass);

        return std::move(ret);
    }

    std::vector<FieldBase> LoadClass::GetFields(bool includeParent) const {
        if (!klass) return {};
        TryInit();
        std::vector<FieldBase> ret{};
        auto curClass = klass;

        do {
            auto end = curClass->fields + curClass->field_count;
            for (IL2CPP::FieldInfo *currentField = curClass->fields; currentField != end; ++currentField) ret.emplace_back(currentField);
            if (includeParent) curClass = curClass->parent;
            else break;
        } while (curClass);

        return std::move(ret);
    }

    std::vector<MethodBase> LoadClass::GetMethods(bool includeParent) const {
        if (!klass) return {};
        TryInit();
        std::vector<MethodBase> ret{};
        auto curClass = klass;

        do {
            for (uint16_t i = 0; i < curClass->method_count; ++i) ret.emplace_back(curClass->methods[i]);
            if (includeParent) curClass = curClass->parent;
            else break;
        } while (curClass);

        return std::move(ret);
    }

    std::vector<PropertyBase> LoadClass::GetProperties(bool includeParent) const {
        if (!klass) return {};
        TryInit();
        std::vector<PropertyBase> ret{};
        auto curClass = klass;

        do {
            auto end = curClass->properties + curClass->property_count;
            for (auto currentProperty = curClass->properties; currentProperty != end; ++currentProperty) ret.emplace_back(currentProperty);
            if (includeParent) curClass = curClass->parent;
            else break;
        } while (curClass);

        return std::move(ret);
    }

    MethodBase LoadClass::GetMethodByName(const std::string_view &name, int parameters) const {
        if (!klass) return {};
        TryInit();

        auto method = BNM_Internal::IterateMethods(*this, [&name, &parameters](IL2CPP::MethodInfo *method) {
            return name == method->name && (method->parameters_count == parameters || parameters == -1);
        });

        if (method) return method;

        BNM_LOG_WARN("Method: [%s]::[%s].[%s], %d - not found.", klass->namespaze, klass->name, name.data(), parameters);
        return {};
    }

    MethodBase LoadClass::GetMethodByName(const std::string_view &name, const std::vector<std::string_view> &parametersName) const {
        if (!klass) return {};
        TryInit();

        auto parameters = (uint8_t) parametersName.size();

        auto method = BNM_Internal::IterateMethods(*this, [&name, &parameters, &parametersName](IL2CPP::MethodInfo *method) {
            if (name != method->name || method->parameters_count != parameters) return false;
            for (uint8_t i = 0; i < parameters; ++i) if (BNM_Internal::il2cppMethods.il2cpp_method_get_param_name(method, i) != parametersName[i]) return false;
            return true;
        });

        if (method) return method;

        BNM_LOG_WARN("Method: [%s]::[%s].(%s), %d - not found; using parameter names.", klass->namespaze, klass->name, name.data(), parameters);
        return {};
    }

    MethodBase LoadClass::GetMethodByName(const std::string_view &name, const std::vector<RuntimeTypeGetter> &parametersType) const {
        if (!klass) return {};
        TryInit();
        auto parameters = (uint8_t) parametersType.size();

        auto method = BNM_Internal::IterateMethods(*this, [&name, &parameters, &parametersType](IL2CPP::MethodInfo *method) {
            if (name != method->name || method->parameters_count != parameters) return false;
            for (uint8_t i = 0; i < parameters; ++i) {
#if UNITY_VER < 212
                auto param = (method->parameters + i)->parameter_type;
#else
                auto param = method->parameters[i];
#endif

                if (LoadClass(param).GetIl2CppClass() != parametersType[i].ToIl2CppClass()) return false;
            }
            return true;
        });

        if (method) return method;

        BNM_LOG_WARN("Method: [%s]::[%s].[%s], %d - not found; using parameter types.", klass->namespaze, klass->name, name.data(), parameters);
        return {};
    }

    PropertyBase LoadClass::GetPropertyByName(const std::string_view &name) {
        if (!klass) return {};
        TryInit();
        auto curClass = klass;

        do {
            auto end = curClass->properties + curClass->property_count;
            for (auto currentProperty = curClass->properties; currentProperty != end; ++currentProperty) if (name == currentProperty->name) return currentProperty;
            curClass = curClass->parent;
        } while (curClass);

        BNM_LOG_WARN("Property: %s.(%s) - not found.", str().c_str(), name.data());
        return {};
    }

    LoadClass LoadClass::GetInnerClass(const std::string_view &name) const {
        if (!klass) return {};
        TryInit();
        auto curClass = klass;

        do {
            for (uint16_t i = 0; i < curClass->nested_type_count; ++i) {
                auto cls = curClass->nestedTypes[i];
                if (name == cls->name) return cls;
            }
            curClass = curClass->parent;
        } while (curClass);

        BNM_LOG_WARN("Inner class: [%s]::[%s]::[%s] - not found.", klass->namespaze, klass->name, name.data());
        return {};
    }

    FieldBase LoadClass::GetFieldByName(const std::string_view &name) const {
        if (!klass) return {};
        TryInit();
        auto curClass = klass;

        do {
            auto end = curClass->fields + curClass->field_count;
            for (IL2CPP::FieldInfo *currentField = curClass->fields; currentField != end; ++currentField) {
                if (name != currentField->name) continue;
                return currentField;
            }
            curClass = curClass->parent;
        } while (curClass);

        BNM_LOG_WARN("Field: [%s]::[%s].(%s) - not found.", klass->namespaze, klass->name, name.data());
        return {};
    }

    LoadClass LoadClass::GetParent() const {
        if (!klass) return {};
        TryInit();
        return klass->parent;
    }

    LoadClass LoadClass::GetArray() const {
        if (!klass) return {};
        TryInit();
        return BNM_Internal::il2cppMethods.il2cpp_array_class_get(klass, 1);
    }

    LoadClass LoadClass::GetPointer() const {
        if (!klass) return {};
        TryInit();
        return BNM_Internal::GetPointer(*this);
    }

    LoadClass LoadClass::GetReference() const {
        if (!klass) return {};
        TryInit();
        return BNM_Internal::GetReference(*this);
    }


    LoadClass LoadClass::GetGeneric(const std::vector<RuntimeTypeGetter> &templateTypes) const {
        if (!klass) return {};
        TryInit();
        return BNM_Internal::TryMakeGenericClass(*this, templateTypes);
    }

    IL2CPP::Il2CppType *LoadClass::GetIl2CppType() const {
        if (!klass) return nullptr;
        TryInit();
#if UNITY_VER > 174
        return (IL2CPP::Il2CppType *)&klass->byval_arg;
#else
        return (IL2CPP::Il2CppType *)klass->byval_arg;
#endif
    }

    MonoType *LoadClass::GetMonoType() const {
        if (!klass) return nullptr;
        TryInit();
        return (MonoType *) BNM_Internal::il2cppMethods.il2cpp_type_get_object(GetIl2CppType());
    }

    IL2CPP::Il2CppClass *LoadClass::GetIl2CppClass() const {
        TryInit();
        return klass;
    }

    BNM::RuntimeTypeGetter LoadClass::GetRuntimeType() const {
        TryInit();
        return {nullptr, nullptr, RuntimeTypeGetter::RuntimeModifier::None, *this};
    }

    LoadClass::operator BNM::RuntimeTypeGetter() const { return GetRuntimeType(); }

    void *LoadClass::CreateNewInstance() const {
        if (!klass) return nullptr;
        TryInit();

        if ((klass->flags & (0x00000080 | 0x00000020))) // TYPE_ATTRIBUTE_ABSTRACT | TYPE_ATTRIBUTE_INTERFACE
            BNM_LOG_WARN("You trying to create instance of abstract or interface class %s?\nIn C# this is impossible.", str().c_str());

        auto obj = BNM_Internal::il2cppMethods.il2cpp_object_new(klass);
        if (obj) memset((char*)obj + sizeof(IL2CPP::Il2CppObject), 0, klass->instance_size - sizeof(IL2CPP::Il2CppObject));
        return (void *) obj;
    }

#ifdef BNM_ALLOW_STR_METHODS
    std::string LoadClass::str() const {
        if (!klass) return OBFUSCATES_BNM("Dead class");
        TryInit();
        return OBFUSCATES_BNM("[") + klass->image->name + OBFUSCATES_BNM("]::[") + klass->namespaze + OBFUSCATES_BNM("]::[") + klass->name + OBFUSCATES_BNM("]");
    }
#endif

    // Try initializing class if it is alive
    void LoadClass::TryInit() const { if (klass) BNM_Internal::Class$$Init(klass); }

    IL2CPP::Il2CppObject *LoadClass::ObjBox(IL2CPP::Il2CppClass *klass, void *data) {
        return BNM_Internal::il2cppMethods.il2cpp_value_box(klass, data);
    }

    IL2CPP::Il2CppArray *LoadClass::ArrayNew(IL2CPP::Il2CppClass *cls, IL2CPP::il2cpp_array_size_t length) {
        return BNM_Internal::il2cppMethods.il2cpp_array_new(cls, length);
    }

    void *LoadClass::NewListInstance() {
        return BNM_Internal::customListTemplateClass.CreateNewInstance();
    }

    // Method for getting and creating types for each list type
    IL2CPP::Il2CppClass *Structures::Mono::__PRIVATE_MonoListData::TryGetMonoListClass(uint32_t typeHash, std::array<__PRIVATE_MonoListData::MethodData, 16> &data) {
        auto &klass = BNM_Internal::customListsMap[typeHash];
        if (klass) return klass;

        std::map<size_t, IL2CPP::MethodInfo *> createdMethods{};
        auto templateClass = BNM_Internal::customListTemplateClass.klass;
        auto size = sizeof(IL2CPP::Il2CppClass) + templateClass->vtable_count * sizeof(IL2CPP::VirtualInvokeData);
        auto typedClass = (IL2CPP::Il2CppClass *) malloc(size);
        memcpy(typedClass, templateClass, size);
        auto hash = std::hash<std::string_view>();
        for (uint16_t i = 4 /* Skip System.Object's virtual methods */; i < typedClass->vtable_count; ++i) {
            auto &cur = typedClass->vtable[i];
            auto name = std::string_view(cur.method->name);
            auto dot = name.rfind('.');
            if (dot != std::string_view::npos) name = name.substr(dot + 1);

            auto iterator = data.begin();
            for (; iterator != data.end(); ++iterator) if (iterator->methodName == name) break;

            auto &methodInfo = createdMethods[hash(name)];

            if (methodInfo == nullptr) {
                methodInfo = (IL2CPP::MethodInfo *) malloc(sizeof(IL2CPP::MethodInfo));
                memcpy((void *) methodInfo, (void *) cur.methodPtr, sizeof(IL2CPP::MethodInfo));
                methodInfo->methodPointer = (decltype(methodInfo->methodPointer)) iterator->ptr;
            }
            cur.method = methodInfo;
            cur.methodPtr = cur.method->methodPointer;
        }
        klass = typedClass;
        return klass;
    }

    LoadClass RuntimeTypeGetter::ToLC() {
        if (loadedClass) return loadedClass;

        if (!name || !namespaze) return {};

        loadedClass = LoadClass(namespaze, name);
        if (modifier == RuntimeModifier::Array) loadedClass = loadedClass.GetArray();
        else if (modifier == RuntimeModifier::Pointer) loadedClass = loadedClass.GetPointer();
        else if (modifier == RuntimeModifier::Reference) loadedClass = loadedClass.GetReference();

        return loadedClass;
    }
    LoadClass RuntimeTypeGetter::ToLC() const {
        return ((RuntimeTypeGetter *)this)->ToLC();
    }
    RuntimeTypeGetter::operator LoadClass() { return ToLC(); }
    RuntimeTypeGetter::operator LoadClass() const { return ToLC(); }

    IL2CPP::Il2CppType *RuntimeTypeGetter::ToIl2CppType() { return ToLC().GetIl2CppType(); }
    IL2CPP::Il2CppType *RuntimeTypeGetter::ToIl2CppType() const { return ToLC().GetIl2CppType(); }
    RuntimeTypeGetter::operator IL2CPP::Il2CppType*() { return ToIl2CppType(); }
    RuntimeTypeGetter::operator IL2CPP::Il2CppType*() const { return ToIl2CppType(); }

    IL2CPP::Il2CppClass *RuntimeTypeGetter::ToIl2CppClass() { return ToLC().GetIl2CppClass(); }
    IL2CPP::Il2CppClass *RuntimeTypeGetter::ToIl2CppClass() const { return ToLC().GetIl2CppClass(); }
    RuntimeTypeGetter::operator IL2CPP::Il2CppClass*() { return ToIl2CppClass(); }
    RuntimeTypeGetter::operator IL2CPP::Il2CppClass*() const { return ToIl2CppClass(); }

    // For thread static fields
    namespace __PRIVATE_FieldUtils {
        void GetStaticValue(IL2CPP::FieldInfo *info, void *value) { return BNM_Internal::il2cppMethods.il2cpp_field_static_get_value(info, value); }

        void SetStaticValue(IL2CPP::FieldInfo *info, void *value) { return BNM_Internal::il2cppMethods.il2cpp_field_static_set_value(info, value); }
    }

    bool CheckIsFieldStatic(IL2CPP::FieldInfo *field) {
        if (!field || !field->type) return false;
        return (field->type->attrs & 0x0010) != 0 && field->offset != -1 && (field->type->attrs & 0x0040) == 0;
    }

    FieldBase::FieldBase(IL2CPP::FieldInfo *info) {
        init = info != nullptr;
        if (init) {
            isStatic = CheckIsFieldStatic(info);
            myInfo = info;
            isThreadStatic = myInfo->offset == -1;
        }
    }

    FieldBase::FieldBase(const FieldBase &other) {
        myInfo = other.myInfo;
        instance = other.instance;
        init = other.init;
        isStatic = other.isStatic;
        isThreadStatic = other.isThreadStatic;
    }

    FieldBase &FieldBase::SetInstance(IL2CPP::Il2CppObject *val)  {
        if (init && isStatic) {
            BNM_LOG_WARN("Trying to set an object to the static field %s. Please remove SetInstance call in the code.", str().c_str());
            return *this;
        }
        init = val && myInfo != nullptr;
        instance = val;
        return *this;
    }

    IL2CPP::FieldInfo *FieldBase::GetInfo() const {
        if (init) return myInfo;
        return {};
    }

    BNM_PTR FieldBase::GetOffset() const {
#ifndef BNM_ALLOW_GET_OFFSET
        BNM_LOG_ERR("GetOffset is forbidden to use! Check the examples if BNM can perform the action you need. If you really need GetOffset, and BNM does not provide an API for the action you intended, then enable BNM_ALLOW_GET_OFFSET in its settings.");
        return 0;
#endif
        if (!init) return 0;
        return myInfo->offset;
    }

    void *FieldBase::GetFieldPointer() const {
        if (!init) return nullptr;
        if (!isStatic && !CheckObj(instance)) {
            BNM_LOG_ERR("Can't get a non-static %s field pointer without an object! Please set the object before trying to get the pointer.", str().c_str());
            return nullptr;
        } else if (isStatic && !CheckObj(myInfo->parent)) {
            BNM_LOG_ERR("Something went wrong, the static field %s has no class.", str().c_str());
            return nullptr;
        } else if (isThreadStatic) {
            BNM_LOG_ERR("Getting a pointer to thread static field is not supported, field: %s.", str().c_str());
            return nullptr;
        }
        if (isStatic) return (void *) ((BNM_PTR) myInfo->parent->static_fields + myInfo->offset);
        return (void *) ((BNM_PTR) instance + myInfo->offset);
    }

    MethodBase::MethodBase(const IL2CPP::MethodInfo *info)  {
        init = (BNM::CheckObj(info) != nullptr);
        if (init) {
            isStatic = (info->flags & 0x0010) == 0x0010;
            isVirtual = info->slot != 65535;
            myInfo = (decltype(myInfo)) info;
        }
    }

    MethodBase::MethodBase(const MethodBase &other) {
        myInfo = other.myInfo;
        instance = other.instance;
        init = other.init;
        isStatic = other.isStatic;
        isVirtual = other.isVirtual;
    }

    MethodBase &MethodBase::SetInstance(IL2CPP::Il2CppObject *val)  {
        if (!init) return *this;
        if (init && isStatic) {
            BNM_LOG_WARN("Trying to set an object to the static field %s. Please remove SetInstance call in the code.", str().c_str());
            return *this;
        }
        instance = val;
        return *this;
    }

    IL2CPP::MethodInfo *MethodBase::GetInfo() const {
        if (init) return myInfo;
        return {};
    }

    BNM_PTR MethodBase::GetOffset() const {
#ifndef BNM_ALLOW_GET_OFFSET
        BNM_LOG_ERR("GetOffset is forbidden to use! Check the examples if BNM can perform the action you need. If you really need GetOffset, and BNM does not provide an API for the action you intended, then enable BNM_ALLOW_GET_OFFSET in its settings.");
        return 0;
#endif
        if (init) return (BNM_PTR) myInfo->methodPointer;
        return {};
    }

    MethodBase MethodBase::GetGeneric(const std::vector<RuntimeTypeGetter> &templateTypes) const {
        BNM_LOG_WARN_IF(!myInfo->is_generic, "%s method is not generic!", str().c_str());
        if (!myInfo->is_generic) return {};
        return BNM_Internal::TryMakeGenericMethod(*this, templateTypes);
    }

    PropertyBase::PropertyBase(const IL2CPP::PropertyInfo *info) {
        if (!info) return;
        hasGetter = hasSetter = false;
        if (info->get && info->get->methodPointer) {
            hasGetter = true;
            getter = info->get;
        }
        if (info->set && info->set->methodPointer) {
            hasSetter = true;
            setter = info->set;
        }
    }

    PropertyBase::PropertyBase(const PropertyBase &other) {
        myInfo = other.myInfo;
        getter = other.getter;
        setter = other.setter;
        hasGetter = other.hasGetter;
        hasSetter = other.hasSetter;
    }

    PropertyBase::PropertyBase(const MethodBase &newGetter, const MethodBase &newSetter)  {
        hasGetter = hasSetter = false;
        if (newGetter) {
            hasGetter = true;
            getter = newGetter;
        }
        if (newSetter) {
            hasSetter = true;
            setter = newSetter;
        }
    }

    PropertyBase &PropertyBase::SetInstance(IL2CPP::Il2CppObject *val) {
        if (hasGetter) getter.SetInstance(val);
        if (hasSetter) setter.SetInstance(val);
        return *this;
    }

#ifdef BNM_DEPRECATED
    bool AttachIl2Cpp() {
        if (CurrentIl2CppThread()) return false;
        BNM_Internal::il2cppMethods.il2cpp_thread_attach(BNM_Internal::il2cppMethods.il2cpp_domain_get());
        return true;
    }

    IL2CPP::Il2CppThread *CurrentIl2CppThread() {
        return BNM_Internal::il2cppMethods.il2cpp_thread_current();
    }

    void DetachIl2Cpp() {
        auto thread = CurrentIl2CppThread();
        if (!thread) return;
        BNM_Internal::il2cppMethods.il2cpp_thread_detach(thread);
    }
#endif
    // Create basic C# string using il2cpp
    Structures::Mono::monoString *CreateMonoString(const char *str) {
        return BNM_Internal::il2cppMethods.il2cpp_string_new(str);
    }
    Structures::Mono::monoString *CreateMonoString(const std::string_view &str) { return CreateMonoString(str.data()); }

    void *GetExternMethod(const std::string_view &str) {
        auto c_str = str.data();
        auto ret = BNM_Internal::il2cppMethods.il2cpp_resolve_icall(c_str);
        BNM_LOG_WARN_IF(!ret, "Extern method %s not found. Please check code.", c_str);
        return ret;
    }

    std::string_view GetIl2CppLibraryPath() {
        if (!BNM_Internal::il2cppLibraryHandle) return {};
        return BNM_Internal::il2cppLibraryAbsolutePath;
    }

    BNM_PTR GetIl2CppLibraryOffset() {
        return BNM_Internal::il2cppLibraryAbsoluteAddress;
    }

    void *GetIl2CppLibraryHandle() {
        return BNM_Internal::il2cppLibraryHandle;
    }

    bool InvokeHookImpl(IL2CPP::MethodInfo *m, void *newMet, void **oldMet) {
        if (!m) return false;
        if (oldMet) *oldMet = (void *) m->methodPointer;
        m->methodPointer = (IL2CPP::Il2CppMethodPointer) newMet;
        return true;
    }

    bool VirtualHookImpl(BNM::LoadClass targetClass, IL2CPP::MethodInfo *m, void *newMet, void **oldMet) {
        if (!m || !targetClass) return false;
        for (uint16_t i = 0; i < targetClass.klass->vtable_count; ++i) {
            auto &vTable = targetClass.klass->vtable[i];
            auto count = vTable.method->parameters_count;

            if (!strcmp(vTable.method->name, m->name) && count == m->parameters_count) {
                bool same = true;
                for (uint8_t p = 0; p < count; ++p) {
#if UNITY_VER < 212
                    auto type = (vTable.method->parameters + p)->parameter_type;
                    auto type2 = (m->parameters + p)->parameter_type;
#else
                    auto type = vTable.method->parameters[p];
                    auto type2 = m->parameters[p];
#endif

                    if (LoadClass(type).GetIl2CppClass() == LoadClass(type2).GetIl2CppClass()) continue;
                    same = false;
                    break;
                }
                if (!same) break;
                if (oldMet) *oldMet = (void *) vTable.methodPtr;
                vTable.methodPtr = (void(*)()) newMet;
                return true;

            }
        }
        return false;
    }

    template<> bool IsA<IL2CPP::Il2CppObject *>(IL2CPP::Il2CppObject *object, IL2CPP::Il2CppClass *klass) {
        if (!object || !klass) return false;
        for (auto cls = object->klass; cls; cls = cls->parent) if (cls == klass) return true;
        return false;
    }

    namespace HexUtils {
        // Reverse hex string (form 001122 to 221100)
        std::string ReverseHexString(const std::string &hex) {
            std::string out;
            for (size_t i = 0; i < hex.length(); i += 2) out.insert(0, hex.substr(i, 2));
            return out;
        }

        // Convert hex string to value
        unsigned long long HexStr2Value(const std::string &hex) { return strtoull(hex.c_str(), nullptr, 16); }

#if defined(__ARM_ARCH_7A__)

        // Check is assembly is `bl ...` or `b ...`
        bool IsBranchHex(const std::string &hex) {
            BNM_PTR hexW = HexStr2Value(ReverseHexString(hex));
            return (hexW & 0x0A000000) == 0x0A000000;
        }

#elif defined(__aarch64__)

        // Check is assembly is `bl ...` or `b ...`
        bool IsBranchHex(const std::string &hex) {
            BNM_PTR hexW = HexStr2Value(ReverseHexString(hex));
            return (hexW & 0xFC000000) == 0x14000000 || (hexW & 0xFC000000) == 0x94000000;
        }

#elif defined(__i386__) || defined(__x86_64__)

        // Check is assembly is `call ...`
        bool IsCallHex(const std::string &hex) { return hex.substr(0, 2) == OBFUSCATE_BNM("E8"); }
#elif defined(__riscv)
#error "Is it out for Android?"
#else
#error "BNM support only arm64, arm, x86 and x86_64"
#endif
        const char *hexChars = OBFUSCATE_BNM("0123456789ABCDEF");
        // Read memory as hex string
        std::string ReadFiveBytesOfMemoryMax(BNM_PTR address, size_t len) {
            char temp[5]; memset(temp, 0, len);
            std::string ret;
            if (memcpy(temp, (void *)address, len) == nullptr) return std::move(ret);
            ret.resize(len * 2, 0);
            auto buf = (char *)ret.data();
            for (size_t i = 0; i < len; ++i) {
                *buf++ = hexChars[temp[i] >> 4];
                *buf++ = hexChars[temp[i] & 0x0F];
            }
            return std::move(ret);
        }

        // Decoding b and bl and getting address that it jumps
        bool DecodeBranchOrCall(const std::string &hex, BNM_PTR offset, BNM_PTR &outOffset) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
            if (!IsBranchHex(hex)) return false;
#if defined(__aarch64__)
            uint8_t add = 0;
#else
            uint8_t add = 8;
#endif
            // This line is based on capstone code
            outOffset = ((int32_t)(((((HexStr2Value(ReverseHexString(hex))) & (((uint32_t)1 << 24) - 1) << 0) >> 0) << 2) << (32 - 26)) >> (32 - 26)) + offset + add;
#elif defined(__i386__) || defined(__x86_64__)
            if (!IsCallHex(hex)) return false;
            // Offest + address from call + size of instruction
            outOffset = offset + HexStr2Value(ReverseHexString(hex).substr(0, 8)) + 5;
#else
#error "BNM support only arm64, arm, x86 and x86_64"
            return false;
#endif
            return true;
        }

        // Parsing memory and trying to find b, or bl, or call
        // Than get address from it
        // index: 0 - first, 1 - second and etc.
        BNM_PTR FindNextJump(BNM_PTR start, int index) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
            BNM_PTR offset = 0;
            std::string curHex = ReadFiveBytesOfMemoryMax(start, 4);
            BNM_PTR outOffset = 0;
            bool out;
            while (!(out = DecodeBranchOrCall(curHex, start + offset, outOffset)) || index != 1) {
                offset += 4;
                curHex = ReadFiveBytesOfMemoryMax(start + offset, 4);
                if (out) index--;
            }
            return outOffset;
#elif defined(__i386__) || defined(__x86_64__)
            BNM_PTR offset = 0;
            std::string curHex = ReadFiveBytesOfMemoryMax(start, 1);
            BNM_PTR outOffset = 0;
            bool out;
            while (!(out = IsCallHex(curHex)) || index != 1) {
                offset += 1;
                curHex = ReadFiveBytesOfMemoryMax(start + offset, 1);
                if (out) index--;
            }
            DecodeBranchOrCall(ReadFiveBytesOfMemoryMax(start + offset, 5), start + offset, outOffset);
            return outOffset;
#elif defined(__riscv)
#error "Is it out for Android?"
#endif
        }
    }
#if !BNM_DISABLE_NEW_CLASSES
    void MODIFY_CLASSES::AddTargetClass(TargetClass *klass) {
        if (!BNM_Internal::modClassesVector) BNM_Internal::modClassesVector = new std::vector<TargetClass *>();
        // Add a class to all target classes
        BNM_Internal::modClassesVector->push_back(klass);
    }
    void NEW_CLASSES::AddNewClass(NewClass *klass) {
        if (!BNM_Internal::newClassesVector) BNM_Internal::newClassesVector = new std::vector<NewClass *>();
        // Add class to all new classes list
        BNM_Internal::newClassesVector->push_back(klass);
    }
#endif
#if defined(__ARM_ARCH_7A__)
#    define CURRENT_ARCH "armeabi-v7a"
#elif defined(__aarch64__)
#    define CURRENT_ARCH "arm64-v8a"
#elif defined(__i386__)
#    define CURRENT_ARCH "x86"
#elif defined(__x86_64__)
#    define CURRENT_ARCH "x86_64"
#elif defined(__riscv)
#    define CURRENT_ARCH "riscv64"
#endif
    void TryForceLoadIl2CppByPath(JNIEnv *env, jobject context) {
        if (!env || BNM_Internal::il2cppLibraryHandle || BNM_Internal::bnmLoaded) return;

        // Get the path to libil2cpp.so using JNI
        if (context == nullptr) {
            jclass activityThread = env->FindClass(OBFUSCATE_BNM("android/app/ActivityThread"));
            auto currentActivityThread = env->CallStaticObjectMethod(activityThread, env->GetStaticMethodID(activityThread, OBFUSCATE_BNM("currentActivityThread"), OBFUSCATE_BNM("()Landroid/app/ActivityThread;")));
            context = env->CallObjectMethod(currentActivityThread, env->GetMethodID(activityThread, OBFUSCATE_BNM("getApplication"), OBFUSCATE_BNM("()Landroid/app/Application;")));
            env->DeleteLocalRef(currentActivityThread);
        }
        auto applicationInfo = env->CallObjectMethod(context, env->GetMethodID(env->GetObjectClass(context), OBFUSCATE_BNM("getApplicationInfo"), OBFUSCATE_BNM("()Landroid/content/pm/ApplicationInfo;")));
        auto applicationInfoClass = env->GetObjectClass(applicationInfo);

        auto flags = env->GetIntField(applicationInfo, env->GetFieldID(applicationInfoClass, OBFUSCATE_BNM("flags"), OBFUSCATE_BNM("I")));
        bool isLibrariesExtracted = (flags & 0x10000000) == 0x10000000; // ApplicationInfo.FLAG_EXTRACT_NATIVE_LIBS

        jstring jDir;
        if (isLibrariesExtracted)
            jDir = (jstring)env->GetObjectField(applicationInfo, env->GetFieldID(applicationInfoClass, OBFUSCATE_BNM("nativeLibraryDir"), OBFUSCATE_BNM("Ljava/lang/String;")));
        else
            jDir = (jstring)env->GetObjectField(applicationInfo, env->GetFieldID(applicationInfoClass, OBFUSCATE_BNM("sourceDir"), OBFUSCATE_BNM("Ljava/lang/String;")));

        std::string file;
        auto cDir = std::string_view(env->GetStringUTFChars(jDir, nullptr));
        env->DeleteLocalRef(applicationInfo); env->DeleteLocalRef(applicationInfoClass);

        if (isLibrariesExtracted)
            // By path to library /data/app/.../package name-.../lib/arch/libil2cpp.so
            file = std::string(cDir) + OBFUSCATE_BNM("/libil2cpp.so");
        else
            // From base.apk /data/app/.../package name-.../base.apk!/lib/arch/libil2cpp.so
            file = std::string(cDir) + OBFUSCATE_BNM("!/lib/" CURRENT_ARCH "/libil2cpp.so");

        // Try to load il2cpp using this path
        auto fileCopy = (char *) malloc(file.size() + 1);
        memcpy(fileCopy, file.data(), file.size());
        fileCopy[file.size()] = 0;
        auto handle = BNM_dlopen(fileCopy, RTLD_LAZY);
        if (!BNM_Internal::InitLibraryHandle(handle, fileCopy)) {
            BNM_LOG_ERR_IF(isLibrariesExtracted, "Failed to load libil2cpp.so by path!");
            free(fileCopy);
        } else goto END;
        if (isLibrariesExtracted) goto END;
        file.clear();

        // From split_config.arch.apk /data/app/.../the package name is.../split_config.arch.apk!/lib/arch/libil2cpp.so
        file = std::string(cDir).substr(0, cDir.length() - 8) + OBFUSCATE_BNM("split_config." CURRENT_ARCH ".apk!/lib/" CURRENT_ARCH "/libil2cpp.so");
        fileCopy = (char *) malloc(file.size() + 1);
        memcpy(fileCopy, file.data(), file.size());
        fileCopy[file.size()] = 0;
        handle = BNM_dlopen(fileCopy, RTLD_LAZY);
        if (!BNM_Internal::InitLibraryHandle(handle, fileCopy)) {
            BNM_LOG_ERR("Failed to load libil2cpp.so by path!");
            free(fileCopy);
        }

        END:
        env->ReleaseStringUTFChars(jDir, cDir.data()); env->DeleteLocalRef(jDir);
    }
#undef CURRENT_ARCH
    namespace External {
        bool TryInitHandle(void *handle, const char *path, bool external) {
            return BNM_Internal::InitLibraryHandle(handle, path, external);
        }

        void TryLoad(void *handle) {
            if (BNM_Internal::InitLibraryHandle(handle, nullptr, true)) {
                BNM_Internal::SetupBNM();
#if !BNM_DISABLE_NEW_CLASSES
                BNM_Internal::InitNewClasses();
                BNM_Internal::ModifyClasses();
#endif
                BNM_Internal::bnmLoaded = true;
            } else BNM_LOG_WARN("[External::LoadBNM] handle is dead or incorrect, BNM is not loaded!");
        }

        void ForceLoad(void *handle) {
            BNM_Internal::il2cppLibraryHandle = handle; // Forcibly set handle
            BNM_Internal::SetupBNM();
#if !BNM_DISABLE_NEW_CLASSES
            BNM_Internal::InitNewClasses();
            BNM_Internal::ModifyClasses();
#endif
            BNM_Internal::bnmLoaded = true;
        }
    }

    void AddOnLoadedEvent(void (*event)()) {
        if (event) BNM_Internal::onIl2CppLoaded.push_back(event);
    }

    void ClearOnLoadedEvents() {
        BNM_Internal::onIl2CppLoaded.clear();
    }

#ifdef BNM_DEBUG
    namespace Utils {
        void *OffsetInLib(void *offsetInMemory) {
            if (offsetInMemory == nullptr) return nullptr;
            Dl_info info; BNM_dladdr(offsetInMemory, &info);
            return (void *) ((BNM_PTR) offsetInMemory - (BNM_PTR) info.dli_fbase);
        }
    }
#endif

}
namespace BNM_Internal {
#if !BNM_DISABLE_NEW_CLASSES
    struct { // Struct for fast finding classes by image
        // Add class to image
        void addClass(const IL2CPP::Il2CppImage *image, IL2CPP::Il2CppClass *cls) {
            return addClass((BNM_PTR)image, cls);
        }

        void addClass(BNM_PTR image, IL2CPP::Il2CppClass *cls) {
#ifndef BNM_DISABLE_MULTI_THREADING_SYNC
            std::shared_lock lock(classesFindAccessMutex);
#endif
            map[image].emplace_back(cls);
        }

        // Iterate all classes added to image
        void forEachByImage(const IL2CPP::Il2CppImage *image, const std::function<bool(IL2CPP::Il2CppClass *)> &func) {
#ifndef BNM_DISABLE_MULTI_THREADING_SYNC
            std::shared_lock lock(classesFindAccessMutex);
#endif
            return forEachByImage((BNM_PTR)image, func);
        }

        void forEachByImage(BNM_PTR image, const std::function<bool(IL2CPP::Il2CppClass *)> &func) {
#ifndef BNM_DISABLE_MULTI_THREADING_SYNC
            std::shared_lock lock(classesFindAccessMutex);
#endif
            for (auto item : map[image]) if (func(item)) break;
        }

        // Iterate all images and classes added to it
        void forEach(const std::function<bool(IL2CPP::Il2CppImage *, std::vector<IL2CPP::Il2CppClass *>)> &func) {
#ifndef BNM_DISABLE_MULTI_THREADING_SYNC
            std::shared_lock lock(classesFindAccessMutex);
#endif
            for (auto &[img, classes] : map)
                if (func((IL2CPP::Il2CppImage *)img, classes))
                    break;
        }
    private:
        std::map<BNM_PTR, std::vector<IL2CPP::Il2CppClass *>> map;
    } BNMClassesMap;

#define BNM_I2C_NEW(type) (IL2CPP::type *) malloc(sizeof(IL2CPP::type))

    // Create a new image
    IL2CPP::Il2CppImage *makeImage(NEW_CLASSES::NewClass *cls) {
        auto newImg = BNM_I2C_NEW(Il2CppImage);

        auto dllName = cls->GetDllName();
        auto nameLen = strlen(dllName);
        newImg->nameNoExt = (char *) malloc(nameLen + 1);
        memcpy((void *)newImg->nameNoExt, (void *)dllName, nameLen);
        ((char *)newImg->nameNoExt)[nameLen] = 0;

        newImg->name = (char *) malloc(nameLen + 5);
        memcpy((void *)newImg->name, (void *)dllName, nameLen);
        auto nameEnd = ((char *)(newImg->name + nameLen));
        nameEnd[0] = '.'; nameEnd[1] = 'd'; nameEnd[2] = 'l'; nameEnd[3] = 'l'; nameEnd[4] = 0;

#if UNITY_VER > 182
        newImg->assembly = nullptr;
        newImg->customAttributeCount = 0;
#if UNITY_VER < 201
        newImg->customAttributeStart = -1;
#endif
#endif

#if UNITY_VER > 201
        // Create an dummy Il2CppImageDefinition
        auto handle = (IL2CPP::Il2CppImageDefinition *) malloc(sizeof(IL2CPP::Il2CppImageDefinition));
        memset(handle, 0, sizeof(IL2CPP::Il2CppImageDefinition));
        handle->typeStart = -1;
        handle->exportedTypeStart = -1;
        handle->typeCount = 0;
        handle->exportedTypeCount = 0;
        handle->nameIndex = -1;
        handle->assemblyIndex = -1;
        handle->customAttributeCount = 0;
        handle->customAttributeStart = -1;
        newImg->metadataHandle = (decltype(newImg->metadataHandle))handle;
#else
        // Set to -1 for disabling parsing by il2cpp
        newImg->typeStart = -1;
        newImg->exportedTypeStart = -1;
#endif
        // Initialize variables
        newImg->typeCount = 0;
        newImg->exportedTypeCount = 0;
        newImg->token = 1;

        // Create a new assembly for the image
        auto newAsm = BNM_I2C_NEW(Il2CppAssembly);
#if UNITY_VER > 174
        // Set assembly and image
        newAsm->image = newImg;
        newAsm->image->assembly = newAsm;
        newAsm->aname.name = newImg->name;
#else
        // Negative value for disabling parsing by il2cpp
        static int newAsmCount = 1;
        newImg->assemblyIndex = newAsm->imageIndex = -newAsmCount;
        newAsmCount++;
#endif
        // Initialize these variables just in case
        newAsm->aname.major = 0;
        newAsm->aname.minor = 0;
        newAsm->aname.build = 0;
        newAsm->aname.revision = 0;
        newAsm->referencedAssemblyStart = -1;
        newAsm->referencedAssemblyCount = 0;

        // Using this, BNM can check if it has created this assembly
        newImg->nameToClassHashTable = (decltype(newImg->nameToClassHashTable)) -0x424e4d;

        // Add an assembly to the list
        Assembly$$GetAllAssemblies()->push_back(newAsm);

        BNM_LOG_INFO("Added new assembly: [%s].", dllName);

        return newImg;
    }

    // Hook `FromIl2CppType' to prevent il2cpp from crashing when trying to get a class from a type created by BNM
    IL2CPP::Il2CppClass *Class$$FromIl2CppType(IL2CPP::Il2CppType *type) {
        if (!type) return nullptr;

        // Check is type created by BNM
        if (type->num_mods == 31) return (IL2CPP::Il2CppClass *)type->data.dummy;

        return old_Class$$FromIl2CppType(type);
    }

    // Hook `GetClassOrElementClass`, to prevent il2cpp from crashing when unity tries to load a package with a field whose class is created by BNM
    IL2CPP::Il2CppClass *Type$$GetClassOrElementClass(IL2CPP::Il2CppType *type) {
        if (!type) return nullptr;

        // Check is type created by BNM
        if (type->num_mods == 31) return (IL2CPP::Il2CppClass *)type->data.dummy;

        return old_Type$$GetClassOrElementClass(type);
    }

    // Hook `FromName`, to prevent il2cpp from crashing when trying to find a class created by BNM
    IL2CPP::Il2CppClass *Class$$FromName(IL2CPP::Il2CppImage *image, const char *namespaze, const char *name) {
        if (!image) return nullptr;

        IL2CPP::Il2CppClass *ret = nullptr;

        // Check is image created by BNM
        if (image->nameToClassHashTable != (decltype(image->nameToClassHashTable))-0x424e4d)
            ret = old_Class$$FromName(image, namespaze, name);

        // If by BNM, try find class
        if (!ret) BNMClassesMap.forEachByImage(image, [namespaze, name, &ret](IL2CPP::Il2CppClass *BNM_class) -> bool {
                if (!strcmp(namespaze, BNM_class->namespaze) && !strcmp(name, BNM_class->name)) {
                    ret = BNM_class;
                    // Found, break for
                    return true;
                }
                return false;
            });

        return ret;
    }

    // Need due to Image and Assembly in 2017.x- has index instead of pointer to Image and Assembly
#if UNITY_VER <= 174
    IL2CPP::Il2CppImage *new_GetImageFromIndex(IL2CPP::ImageIndex index) {
        // Index lower than 0, so it is BNM image
        if (index < 0) {
            IL2CPP::Il2CppImage *ret = nullptr;

            // Iterate all images and check is index is same
            BNMClassesMap.forEach([index, &ret](IL2CPP::Il2CppImage *img, const std::vector<IL2CPP::Il2CppClass *> &classes) -> bool {
                if (img->assemblyIndex == index) {
                    ret = img;
                    return true;
                }
                return false;
            });

            return ret;
        }

        return old_GetImageFromIndex(index);
    }
    // All requests redirected to BNM and they are processed by it
    // In unity 2017 and lower name stored as metadata index so we can't use it
    // But we can check name by images
    IL2CPP::Il2CppAssembly *Assembly$$Load(const char *name) {
        auto LoadedAssemblies = Assembly$$GetAllAssemblies();

        for (auto assembly : *LoadedAssemblies) {

            // Get image from assembly
            auto image = new_GetImageFromIndex(assembly->imageIndex);

            // Check is name is same
            if (!strcmp(image->name, name) || !strcmp(image->nameNoExt, name)) return assembly;
        }

        return nullptr;
    }
#endif

    namespace NewOrModTypes_Internal {
#pragma pack(push, 1)
        struct NewMethodInfo {
            IL2CPP::Il2CppMethodPointer address{};
            IL2CPP::InvokerMethod invoker{};
            std::string_view name;
            std::vector<RuntimeTypeGetter> *argumentsTypes{};
            RuntimeTypeGetter returnType;
            uint8_t argumentsCount{};
            uint8_t isStatic : 1{};
        };
#pragma pack(pop)
        IL2CPP::MethodInfo *CreateMethod(NewMethodInfo &info) {
            auto *myInfo = BNM_I2C_NEW(MethodInfo);
            myInfo->methodPointer = info.address;
            myInfo->invoker_method = info.invoker;
            myInfo->parameters_count = info.argumentsCount;

            auto name = (char *) malloc(info.name.size() + 1);
            memcpy((void *)name, info.name.data(), info.name.size());
            name[info.name.size()] = 0;
            myInfo->name = name;

            // Set method flags
            myInfo->flags = 0x0006 | 0x0080; // PUBLIC | HIDE_BY_SIG
            if (info.isStatic) myInfo->flags |= 0x0010; // |= STATIC
            if (info.name == constructorName) myInfo->flags |= 0x0800 | 0x1000; // |= SPECIAL_NAME | RT_SPECIAL_NAME

            // BNM does not support creating generic methods
            myInfo->is_generic = false;

            // Set the return type
            auto methodType = info.returnType.ToLC();
            if (!methodType) methodType = vmData.Object;
            myInfo->return_type = methodType.GetIl2CppType();

            // Create arguments
            auto argsCount = myInfo->parameters_count;
            if (argsCount) {
                auto &types = *info.argumentsTypes;
#if UNITY_VER < 212
                myInfo->parameters = (IL2CPP::ParameterInfo *)malloc(argsCount * sizeof(IL2CPP::ParameterInfo));
                memset((void *)myInfo->parameters, 0, argsCount * sizeof(IL2CPP::ParameterInfo));

                auto parameter = (IL2CPP::ParameterInfo *)myInfo->parameters;
                for (uint8_t p = 0; p < argsCount; ++p) {
#ifdef BNM_DEPRECATED
                    auto stringIndex = std::to_string(p);

                    parameter->name = (char *) malloc(1 + stringIndex.size() + 1);
                    ((char *)parameter->name)[0] = 'a';
                    memcpy((void *)(parameter->name + 1), (void *)stringIndex.data(), stringIndex.size());
                    ((char *)parameter->name)[stringIndex.size() + 1] = 0;
#else
                    parameter->name = nullptr; // The name of the parameter does not interest engine in any way at all
#endif
                    parameter->position = p;

                    // Set type anyway
                    auto type = p < types.size() ? types[p].ToLC() : vmData.Object;
                    if (!type) type = vmData.Object;
                    parameter->parameter_type = type.GetIl2CppType();
                    parameter->token = parameter->parameter_type->attrs | p;

                    ++parameter;
                }
#else

                auto parameters = (IL2CPP::Il2CppType **) malloc(argsCount * sizeof(IL2CPP::Il2CppType *));

                myInfo->parameters = (const IL2CPP::Il2CppType **) parameters;
                for (uint8_t p = 0; p < argsCount; ++p) {
                    auto parameter = BNM_I2C_NEW(Il2CppType);

                    // Set type anyway
                    auto type = p < types.size() ? types[p].ToLC() : vmData.Object;
                    if (!type) type = vmData.Object;
                    memcpy(parameter, type.GetIl2CppType(), sizeof(IL2CPP::Il2CppType));

                    parameters[p] = parameter;
                }
#endif
            }
#if UNITY_VER >= 212
            // We can't normally without hooks set args name, because names moved to metadata
            myInfo->methodMetadataHandle = nullptr;
#endif
            return myInfo;
        }

        bool hasInterface(IL2CPP::Il2CppClass *parent, IL2CPP::Il2CppClass *interface) {
            if (!parent || !interface) return false;
            for (uint16_t i = 0; i < parent->interfaces_count; ++i) if (parent->implementedInterfaces[i] == interface) return true;
            if (parent->parent) return hasInterface(parent->parent, interface);
            return false;
        }
    }

    void ModifyClasses() {
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
        std::lock_guard<std::mutex> lock(modClassMutex);
#endif
        if (modClassesVector == nullptr) return;
        for (auto modClass : *modClassesVector) {
            auto type = modClass->GetTargetType();
            auto lc = type.ToLC();
            auto fields4Add = modClass->GetFields();
            auto methods4Add = modClass->GetMethods();

            if (!lc) {
                BNM_LOG_WARN("[ModifyClasses] Failed to add %lu methods and %lu fields to the class.", methods4Add.size(), fields4Add.size());
                continue;
            }

            auto klass = lc.GetIl2CppClass();
            auto newMethodsCount = methods4Add.size();
            auto newFieldsCount = fields4Add.size();
            auto targetModifier = modClass->GetTargetModifier();

            // Set parent type
            if (targetModifier.newParentGetter)
                klass->parent = targetModifier.newParentGetter().ToIl2CppClass();

            IL2CPP::Il2CppClass *owner = nullptr;
            if (targetModifier.newOwnerGetter)
                owner = targetModifier.newOwnerGetter().ToIl2CppClass();

            // Set owner type
            if (owner) {
                auto oldOwner = klass->declaringType;
                auto oldInnerList = owner->nestedTypes;

                klass->declaringType = owner;

                // Add class to new owner's list
                auto newInnerList = (IL2CPP::Il2CppClass **) malloc(sizeof(IL2CPP::Il2CppClass) * (owner->nested_type_count + 1));
                memcpy(newInnerList, owner->nestedTypes, sizeof(IL2CPP::Il2CppClass) * owner->nested_type_count);
                newInnerList[owner->nested_type_count++] = klass;
                owner->nestedTypes = newInnerList;

                // Mark class to use less memory
                if ((owner->flags & 0x99000000) == 0x99000000) free(oldInnerList);
                owner->flags |= 0x99000000;

                // Remove class from the old owner's list
                if (oldOwner) {
                    oldInnerList = oldOwner->nestedTypes;
                    newInnerList = (IL2CPP::Il2CppClass **) malloc(sizeof(IL2CPP::Il2CppClass) * (oldOwner->nested_type_count - 1));
                    uint8_t skipped = 0;
                    for (uint16_t i = 0; i < oldOwner->nested_type_count; ++i) {
                        if (skipped == 0) if (skipped = (oldInnerList[i] == klass); skipped) continue;
                        newInnerList[i - skipped] = oldInnerList[i];
                    }
                    oldOwner->nestedTypes = newInnerList;
                    --oldOwner->nested_type_count;

                    // Mark class to use less memory
                    if ((oldOwner->flags & 0x99000000) == 0x99000000) free(oldInnerList);
                    oldOwner->flags |= 0x99000000;
                }
            }

            if (newMethodsCount) {
                auto oldCount = klass->method_count;
                auto oldMethods = klass->methods;

                // Create a new list of methods
                auto newMethods = (IL2CPP::MethodInfo **) malloc((oldCount + newMethodsCount) * sizeof(IL2CPP::MethodInfo *));

                // Copy old methods, if any
                if (oldCount) memcpy(newMethods, oldMethods, oldCount * sizeof(IL2CPP::MethodInfo *));

                // Create and add all methods
                for (size_t i = 0; i < newMethodsCount; ++i, ++oldCount) {
                    auto &method = methods4Add[i];

                    NewOrModTypes_Internal::NewMethodInfo info;

                    info.address = (IL2CPP::Il2CppMethodPointer) method->GetAddress();
                    info.invoker = (IL2CPP::InvokerMethod) method->GetInvoker();
                    info.argumentsCount = method->GetArgsCount();
                    info.name = method->GetName();
                    info.isStatic = method->IsStatic();
                    info.returnType = method->GetRetType();

                    auto argumentsTypes = method->GetArgTypes();
                    info.argumentsTypes = &argumentsTypes;

                    method->myInfo = NewOrModTypes_Internal::CreateMethod(info);

                    newMethods[oldCount] = method->myInfo;
                    BNM_LOG_DEBUG("[ModifyClasses] Added %smethod %s %d to %s.", (info.isStatic == 1) ? "статический " : "", info.name.data(), info.argumentsCount, lc.str().c_str());
                }

                klass->methods = (const IL2CPP::MethodInfo **)newMethods;
                klass->method_count += newMethodsCount;
            }

            if (newFieldsCount) {
                auto oldCount = klass->field_count;

                // Create a new list of fields
                auto newFields = (IL2CPP::FieldInfo *)malloc((oldCount + newFieldsCount) * sizeof(IL2CPP::FieldInfo));

                // Copy old fields, if any
                if (oldCount) memcpy(newFields, klass->fields, oldCount * sizeof(IL2CPP::FieldInfo));

                // Address of new fields
                auto currentAddress = klass->instance_size;

                IL2CPP::FieldInfo *newField = newFields + oldCount;
                for (auto field : fields4Add) {
                    auto name = field->GetName();
                    auto len = strlen(name);
                    newField->name = (char *) malloc(len + 1);
                    memcpy((void *)newField->name, name, len);
                    ((char *)newField->name)[len] = 0;

                    // Copy type
                    newField->type = BNM_I2C_NEW(Il2CppType);
                    auto fieldType = field->GetType().ToLC();
                    if (!fieldType) fieldType = vmData.Object;
                    memcpy((void *)newField->type, (void *)fieldType.GetIl2CppType(), sizeof(IL2CPP::Il2CppType));

                    ((IL2CPP::Il2CppType*)newField->type)->attrs |= 0x0006; // PUBLIC
                    newField->token = newField->type->attrs;
                    newField->parent = klass;

                    field->offset = currentAddress;
                    newField->offset = (int32_t) currentAddress;
                    field->myInfo = newField;

                    // Get offset of next field
                    currentAddress += field->GetSize();

                    newField++;
                    BNM_LOG_DEBUG("[ModifyClasses] Added field %s to %s.", name, lc.str().c_str());
                }

                klass->fields = newFields;
                klass->field_count += newFieldsCount;

                // Увеличить размер класса
                klass->instance_size = (uint32_t) currentAddress;
            }

            BNM_LOG_INFO("[ModifyClasses] Class %s successfully changed.", lc.str().c_str());
        }

        // Clear modClassesVector
        modClassesVector->clear(); modClassesVector->shrink_to_fit();
        delete modClassesVector;
        modClassesVector = nullptr;
    }
    void InitNewClasses() {
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
        std::lock_guard<std::mutex> lock(addClassMutex);
#endif
        if (!newClassesVector) return;

        for (auto klass : *newClassesVector) {
            IL2CPP::Il2CppImage *curImg = nullptr;

            auto className = klass->GetName();
            auto classNamespace = klass->GetNamespace();

            // Checking if class already exists in il2cpp
            {
                LoadClass existLS{};

                auto assemblies = BNM_Internal::Assembly$$GetAllAssemblies();

                // Try to find an image
                auto dllName = klass->GetDllName();
                for (auto assembly : *assemblies) {
                    auto image = il2cppMethods.il2cpp_assembly_get_image(assembly);

                    if (strcmp(dllName, image->nameNoExt) != 0) continue;

                    curImg = image;
                    break;
                }

                // Check if the image is found
                if (curImg) existLS = TryGetClassInImage(curImg, classNamespace, className);
                else curImg = makeImage(klass); // Create new image for a new class

                // If it exists, warn and set class and type
                if (existLS) {
                    BNM_LOG_WARN("Класс %s уже существует, он не может быть добавлен в il2cpp! Пожалуйста, проверьте код.", existLS.str().c_str());
                    // In case
                    klass->myClass = existLS.klass;
                    klass->type = existLS;
                    continue;
                }
            }

            // Create type for new class
            IL2CPP::Il2CppType classType;
            memset(&classType, 0, sizeof(IL2CPP::Il2CppType));
            classType.type = IL2CPP::Il2CppTypeEnum::IL2CPP_TYPE_CLASS;
            classType.attrs = 0x1; // Public
            classType.pinned = 0;
            classType.byref = 0;
            classType.num_mods = 31;

            // Get parent type
            IL2CPP::Il2CppClass *parent = klass->GetBaseType().ToIl2CppClass();
            if (!parent) parent = vmData.Object.GetIl2CppClass();

            // Get owner type
            IL2CPP::Il2CppClass *owner = nullptr;
            auto ownerGetter = klass->GetOwnerGetter();
            if (ownerGetter) owner = ownerGetter().ToIl2CppClass();

            std::vector<IL2CPP::Il2CppRuntimeInterfaceOffsetPair> newInterOffsets{};
            if (parent->interfaceOffsets)
                for (uint16_t i = 0; i < parent->interface_offsets_count; ++i)
                    newInterOffsets.push_back(parent->interfaceOffsets[i]);

            auto newVtableSize = parent->vtable_count;

            // Explore interfaces
            auto allInterfaces = klass->GetInterfaces();
            std::vector<IL2CPP::Il2CppClass *> interfaces{};
            for (auto &interface : allInterfaces)
                if (auto cls = interface.ToIl2CppClass(); !NewOrModTypes_Internal::hasInterface(parent, cls))
                    interfaces.push_back(cls);

            // Need for overriding virtual methods
            std::vector<IL2CPP::VirtualInvokeData> newVTable(newVtableSize);
            for (uint16_t i = 0; i < parent->vtable_count; ++i) newVTable[i] = parent->vtable[i];
            for (auto interface : interfaces) {
                newInterOffsets.push_back({interface, newVtableSize});
                for (uint16_t i = 0; i < interface->method_count; ++i) {
                    auto v = interface->methods[i];
                    ++newVtableSize;
                    newVTable.push_back({nullptr, v});
                }
            }

            // Create all new methods
            const IL2CPP::MethodInfo **methods = nullptr;
            auto methods4Add = klass->GetMethods();
            uint8_t hasFinalize = 0;

            // Check if need to add default constructor to the class
            methods = (const IL2CPP::MethodInfo **) malloc(methods4Add.size() * sizeof(IL2CPP::MethodInfo *));

            // Create all methods
            for (size_t i = 0; i < methods4Add.size(); ++i) {
                auto &method = methods4Add[i];

                NewOrModTypes_Internal::NewMethodInfo info;

                info.address = (IL2CPP::Il2CppMethodPointer) method->GetAddress();
                info.invoker = (IL2CPP::InvokerMethod) method->GetInvoker();
                info.argumentsCount = method->GetArgsCount();
                info.name = method->GetName();
                info.isStatic = method->IsStatic();
                info.returnType = method->GetRetType();

                auto argumentsTypes = method->GetArgTypes();
                info.argumentsTypes = &argumentsTypes;

                method->myInfo = NewOrModTypes_Internal::CreateMethod(info);

                // Replacing methods in virtual table
                for (uint16_t v = 0; v < newVtableSize; ++v) {
                    auto &vTable = newVTable[v];
                    auto count = vTable.method->parameters_count;

                    if (!strcmp(vTable.method->name, method->myInfo->name) && count == method->myInfo->parameters_count && argumentsTypes.size() == count) {
                        bool same = true;
                        for (uint8_t p = 0; p < count; ++p) {
#if UNITY_VER < 212
                            auto type = (vTable.method->parameters + p)->parameter_type;
#else
                            auto type = vTable.method->parameters[p];
#endif
                            if (LoadClass(type).GetIl2CppClass() == argumentsTypes[p].ToIl2CppClass()) continue;

                            same = false;
                            break;

                        }
                        if (!same) break;
                        if (!hasFinalize) hasFinalize = v == finalizerSlot;
                        vTable.method = method->myInfo;
                        vTable.methodPtr = method->myInfo->methodPointer;
                        break;

                    }
                }

                methods[i] = method->myInfo;
            }

            // Create a new class
            klass->myClass = (IL2CPP::Il2CppClass *)malloc(sizeof(IL2CPP::Il2CppClass) + newVTable.size() * sizeof(IL2CPP::VirtualInvokeData));
            memset(klass->myClass, 0, sizeof(IL2CPP::Il2CppClass) + newVTable.size() * sizeof(IL2CPP::VirtualInvokeData));
            klass->myClass->parent = parent;

            // Add a class to the owner
            klass->myClass->declaringType = owner;
            if (owner) {
                auto oldInnerList = owner->nestedTypes;
                auto newInnerList = (IL2CPP::Il2CppClass **) malloc(sizeof(IL2CPP::Il2CppClass) * (owner->nested_type_count + 1));
                memcpy(newInnerList, owner->nestedTypes, sizeof(IL2CPP::Il2CppClass) * owner->nested_type_count);
                newInnerList[owner->nested_type_count++] = klass->myClass;
                owner->nestedTypes = newInnerList;

                // Mark a class to use less memory
                if ((owner->flags & 0x99000000) == 0x99000000) free(oldInnerList);
                owner->flags |= 0x99000000;
            }

#if UNITY_VER > 174
#define kls klass
#else
#define kls declaring_type
#endif
            // Completing method creation
            for (size_t i = 0; i < methods4Add.size(); ++i) ((IL2CPP::MethodInfo *)methods[i])->kls = klass->myClass;
#undef kls
            klass->myClass->method_count = methods4Add.size();
            klass->myClass->methods = methods;

            // Complete creation of fields
            auto fields4Add = klass->GetFields();
            klass->myClass->field_count = fields4Add.size();
            if (klass->myClass->field_count > 0) {
                // Create list of fields
                auto fields = (IL2CPP::FieldInfo *)malloc(klass->myClass->field_count * sizeof(IL2CPP::FieldInfo));

                // Get first field
                IL2CPP::FieldInfo *newField = fields;
                for (auto field : fields4Add) {
                    auto name = field->GetName();
                    auto len = strlen(name);
                    newField->name = (char *) malloc(len + 1);
                    memcpy((void *)newField->name, name, len);
                    ((char *)newField->name)[len] = 0;

                    // Copy type
                    newField->type = BNM_I2C_NEW(Il2CppType);
                    auto fieldType = field->GetType().ToLC();
                    if (!fieldType) fieldType = vmData.Object;
                    memcpy((void *)newField->type, (void *)fieldType.GetIl2CppType(), sizeof(IL2CPP::Il2CppType));

                    ((IL2CPP::Il2CppType*)newField->type)->attrs |= field->GetAttributes(); // PUBLIC
                    newField->token = newField->type->attrs;
                    newField->parent = klass->myClass;
                    newField->offset = (int32_t) field->GetOffset();
                    field->myInfo = newField;

                    newField++;
                }
                klass->myClass->fields = fields;
            } else {
                klass->myClass->static_fields_size = 0;
                klass->myClass->static_fields = nullptr;
                klass->myClass->fields = nullptr;
            }

            // Make type hierarchy
            klass->myClass->typeHierarchyDepth = parent->typeHierarchyDepth + 1;
            klass->myClass->typeHierarchy = (IL2CPP::Il2CppClass **)malloc(klass->myClass->typeHierarchyDepth * sizeof(IL2CPP::Il2CppClass *));
            memcpy(klass->myClass->typeHierarchy, parent->typeHierarchy, parent->typeHierarchyDepth * sizeof(IL2CPP::Il2CppClass *));
            klass->myClass->typeHierarchy[parent->typeHierarchyDepth] = klass->myClass;

            // Set image
            klass->myClass->image = curImg;

            auto len = strlen(className);
            klass->myClass->name = (char *) malloc(len + 1);
            memcpy((void *)klass->myClass->name, className, len);
            ((char *)klass->myClass->name)[len] = 0;

            len = strlen(classNamespace);
            klass->myClass->namespaze = (char *) malloc(len + 1);
            memcpy((void *)klass->myClass->namespaze, classNamespace, len);
            ((char *)klass->myClass->namespaze)[len] = 0;

            // Set types
            classType.data.dummy = klass->myClass;
#if UNITY_VER > 174
            klass->myClass->this_arg = klass->myClass->byval_arg = classType;
            klass->myClass->this_arg.byref = 1;
#else
            klass->myClass->byval_arg = BNM_I2C_NEW(Il2CppType);
            memcpy((void *)klass->myClass->byval_arg, &classType, sizeof(IL2CPP::Il2CppType));
            klass->myClass->this_arg = BNM_I2C_NEW(Il2CppType);
            classType.byref = 1;
            memcpy((void *)klass->myClass->this_arg, &classType, sizeof(IL2CPP::Il2CppType));
#endif

            // Copy parent flags and remove ABSTRACT flag is exists
            klass->myClass->flags = klass->myClass->parent->flags & ~0x00000080; // TYPE_ATTRIBUTE_ABSTRACT

            // Set classes
            klass->myClass->element_class = klass->myClass;
            klass->myClass->castClass = klass->myClass;
#if UNITY_VER > 174
            klass->myClass->klass = klass->myClass;
#endif
            // Initialize sizes
            klass->myClass->native_size = -1;
            klass->myClass->element_size = 0;
            klass->myClass->instance_size = klass->myClass->actualSize = klass->GetSize();

            // Set virtual table
            klass->myClass->vtable_count = newVTable.size();
            for (size_t i = 0; i < newVTable.size(); ++i) klass->myClass->vtable[i] = newVTable[i];

            // Set interface addresses
            klass->myClass->interface_offsets_count = newInterOffsets.size();
            klass->myClass->interfaceOffsets = (IL2CPP::Il2CppRuntimeInterfaceOffsetPair *)malloc(newInterOffsets.size() * sizeof(IL2CPP::Il2CppRuntimeInterfaceOffsetPair));
            memcpy(klass->myClass->interfaceOffsets, newInterOffsets.data(), newInterOffsets.size() * sizeof(IL2CPP::Il2CppRuntimeInterfaceOffsetPair));

            // Add Interfaces
            if (!interfaces.empty()) {
                klass->myClass->interfaces_count = interfaces.size();
                klass->myClass->implementedInterfaces = (IL2CPP::Il2CppClass **)malloc(interfaces.size() * sizeof(IL2CPP::Il2CppClass *));
                memcpy(klass->myClass->implementedInterfaces, interfaces.data(), interfaces.size() * sizeof(IL2CPP::Il2CppClass *));
            } else {
                klass->myClass->interfaces_count = 0;
                klass->myClass->implementedInterfaces = nullptr;
            }

            // Prevent il2cpp from calling LivenessState::TraverseGCDescriptor for class
            klass->myClass->gc_desc = nullptr;


            // BNM does not support creating generic classes
            klass->myClass->generic_class = nullptr;
            klass->myClass->genericRecursionDepth = 1;
#if UNITY_VER < 202
            klass->myClass->genericContainerIndex = -1;
#else
            klass->myClass->genericContainerHandle = nullptr;
            klass->myClass->typeMetadataHandle = nullptr;
#endif

#if UNITY_VER < 211
            klass->myClass->valuetype = 0;
#endif
            // BNM does not support creating static field constructor (.cctor). Il2Cpp is designed so that it will never be called, and BNM does not allow you to create static fields
            klass->myClass->has_cctor = 0;

            // Other variables that need to be set
            klass->myClass->interopData = nullptr;
            klass->myClass->nestedTypes = nullptr;
            klass->myClass->properties = nullptr;
            klass->myClass->rgctx_data = nullptr;
            klass->myClass->has_references = 0;
            klass->myClass->has_finalize = hasFinalize;
            klass->myClass->size_inited = klass->myClass->is_vtable_initialized = 1;
            klass->myClass->initialized = 1;
#if UNITY_VER > 182
            klass->myClass->initialized_and_no_error = 1;
#if UNITY_VER >= 231
            klass->myClass->initializationExceptionGCHandle = nullptr;
#else
            klass->myClass->initializationExceptionGCHandle = 0;
#endif
#if UNITY_VER < 212
            klass->myClass->has_initialization_error = 0;
#endif
#if UNITY_VER < 222
            klass->myClass->naturalAligment = 0;
#endif
#endif
            klass->myClass->init_pending = 0;
            klass->myClass->enumtype = 0;
            klass->myClass->minimumAlignment = 8;
            klass->myClass->is_generic = 0;
            klass->myClass->rank = 0;
            klass->myClass->nested_type_count = 0;
            klass->myClass->thread_static_fields_offset = 0;
            klass->myClass->thread_static_fields_size = -1;
            klass->myClass->cctor_started = 0;
#if UNITY_VER >= 203 && (UNITY_VER != 211 || UNITY_PATCH_VER >= 24)
            klass->myClass->size_init_pending = 0;
#endif
#if UNITY_VER >= 212
            klass->myClass->cctor_finished_or_no_cctor = 1;
#else
            klass->myClass->cctor_finished = 1;
#endif
            klass->myClass->cctor_thread = 0;

            // Add a class to the list of created classes
            BNMClassesMap.addClass(curImg, klass->myClass);

            // Get C# type
            klass->type = LoadClass(klass->myClass);

            BNM_LOG_INFO("[InitNewClasses] Added new class (%p) [%s]::[%s], parent is - [%s]::[%s], to [%s].", klass->myClass, klass->myClass->namespaze, klass->myClass->name, parent->namespaze, parent->name, klass->myClass->image->name);
        }
        // Clear newClassesVector
        newClassesVector->clear(); newClassesVector->shrink_to_fit();
        delete newClassesVector;
        newClassesVector = nullptr;
    }

#undef BNM_I2C_NEW

#endif
    IL2CPP::Il2CppClass *TryGetClassInImage(const IL2CPP::Il2CppImage *image, const std::string_view &namespaze, const std::string_view &name) {
        if (!image) return nullptr;

#if !BNM_DISABLE_NEW_CLASSES
        // Get BNM classes
        if (image->nameToClassHashTable == (decltype(image->nameToClassHashTable))-0x424e4d) goto NEW_CLASSES;
#endif

        if (BNM_Internal::il2cppMethods.il2cpp_image_get_class) {
            size_t typeCount = image->typeCount;

            for (size_t i = 0; i < typeCount; ++i) {
                auto cls = il2cppMethods.il2cpp_image_get_class(image, i);
                if (strcmp(OBFUSCATE_BNM("<Module>"), cls->name) == 0 || cls->declaringType) continue;
                if (namespaze == cls->namespaze && name == cls->name) return cls;
            }
        } else {
            ClassVector classes{};
            BNM_Internal::Image$$GetTypes(image, false, &classes);

            for (auto cls : classes) {
                if (!cls) continue;
                BNM_Internal::Class$$Init(cls);
                if (cls->declaringType) continue;
                if (cls->namespaze == namespaze && cls->name == name) return cls;
            }
        }

#if !BNM_DISABLE_NEW_CLASSES
        NEW_CLASSES:
        IL2CPP::Il2CppClass *result = nullptr;
        BNMClassesMap.forEachByImage(image, [&namespaze, &name, &result](IL2CPP::Il2CppClass *BNM_class) -> bool {
            if (namespaze != BNM_class->namespaze || name != BNM_class->name) return false;

            result = BNM_class;
            return true;
        });
#endif

        return result;
    }

    void Image$$GetTypes(const IL2CPP::Il2CppImage *image, bool, ClassVector *target) {
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
        std::shared_lock<std::shared_mutex> lock(findClassesMutex);
#endif
        // Check the image and target
        if (!image || !target) return;

        // Get non BNM classes
        if (image->nameToClassHashTable != (decltype(image->nameToClassHashTable))-0x424e4d) {
            if (il2cppMethods.il2cpp_image_get_class) {
                auto typeCount = image->typeCount;
                for (uint32_t i = 0; i < typeCount; ++i) {
                    auto type = il2cppMethods.il2cpp_image_get_class(image, i);
                    if (strcmp(OBFUSCATE_BNM("<Module>"), type->name) == 0) continue;
                    target->push_back(type);
                }
            } else old_Image$$GetTypes(image, false, target);
        }

#if !BNM_DISABLE_NEW_CLASSES
        // Get BNM classes
        BNMClassesMap.forEachByImage(image, [&target](IL2CPP::Il2CppClass *BNM_class) -> bool {
            target->push_back(BNM_class);
            return false;
        });
#endif
    }

    void SetupBNM() {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
        int count = 1;
#elif defined(__i386__) || defined(__x86_64__)
        // x86 имеет один вызов до кода метода
        int count = 2;
#endif

        //! il2cpp::vm::Class::Init
        if (!Class$$Init) {
            // Path:
            // il2cpp_array_new_specific ->
            // il2cpp::vm::Array::NewSpecific ->
            // il2cpp::vm::Class::Init
            Class$$Init = (decltype(Class$$Init)) HexUtils::FindNextJump(HexUtils::FindNextJump((BNM_PTR) BNM_dlsym(il2cppLibraryHandle, OBFUSCATE_BNM("il2cpp_array_new_specific")), count), count);
            BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::Class::Init in lib: %p.", Utils::OffsetInLib((void *)Class$$Init));
        }

#define INIT_IL2CPP_API(name) il2cppMethods.name = (decltype(il2cppMethods.name)) BNM_dlsym(BNM_Internal::il2cppLibraryHandle, OBFUSCATE_BNM(#name))

        INIT_IL2CPP_API(il2cpp_image_get_class);
        INIT_IL2CPP_API(il2cpp_get_corlib);
        INIT_IL2CPP_API(il2cpp_class_from_name);
        INIT_IL2CPP_API(il2cpp_assembly_get_image);
        INIT_IL2CPP_API(il2cpp_image_get_class);
        INIT_IL2CPP_API(il2cpp_method_get_param_name);
        INIT_IL2CPP_API(il2cpp_class_from_il2cpp_type);
        INIT_IL2CPP_API(il2cpp_array_class_get);
        INIT_IL2CPP_API(il2cpp_type_get_object);
        INIT_IL2CPP_API(il2cpp_object_new);
        INIT_IL2CPP_API(il2cpp_value_box);
        INIT_IL2CPP_API(il2cpp_array_new);
        INIT_IL2CPP_API(il2cpp_field_static_get_value);
        INIT_IL2CPP_API(il2cpp_field_static_set_value);
        INIT_IL2CPP_API(il2cpp_string_new);
        INIT_IL2CPP_API(il2cpp_resolve_icall);

#ifdef BNM_DEPRECATED
        INIT_IL2CPP_API(il2cpp_domain_get);
        INIT_IL2CPP_API(il2cpp_thread_attach);
        INIT_IL2CPP_API(il2cpp_thread_current);
        INIT_IL2CPP_API(il2cpp_thread_detach);
#endif
#undef INIT_IL2CPP_API
        //! il2cpp::vm::Image::GetTypes
        if (!old_Image$$GetTypes && il2cppMethods.il2cpp_image_get_class == nullptr) {
            auto assemblyClass = il2cppMethods.il2cpp_class_from_name(il2cppMethods.il2cpp_get_corlib(), OBFUSCATE_BNM("System.Reflection"), OBFUSCATE_BNM("Assembly"));
            BNM_PTR GetTypesAdr = LoadClass(assemblyClass).GetMethodByName(OBFUSCATE_BNM("GetTypes"), 1).GetOffset();
            const int sCount
#if UNITY_VER >= 211
                    = count;
#elif UNITY_VER > 174
            = count + 1;
#else
            = count + 2;
#endif
            // Path:
            // System.Reflection.Assembly.GetTypes(bool) ->
            // il2cpp::icalls::mscorlib::System::Reflection::Assembly::GetTypes ->
            // il2cpp::icalls::mscorlib::System::Module::InternalGetTypes ->
            // il2cpp::vm::Image::GetTypes
            auto Image$$GetTypes_t = HexUtils::FindNextJump(HexUtils::FindNextJump(HexUtils::FindNextJump(GetTypesAdr, count), sCount), count);
#if !BNM_DISABLE_NEW_CLASSES
            HOOK(Image$$GetTypes_t, Image$$GetTypes, old_Image$$GetTypes);
#else
            old_Image$$GetTypes = (decltype(old_Image$$GetTypes)) Image$$GetTypes_t;
#endif
            BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::Image::GetTypes in lib: %p.", Utils::OffsetInLib((void *)Image$$GetTypes_t));
        } else if (il2cppMethods.il2cpp_image_get_class != nullptr) {
            BNM_LOG_DEBUG("[SetupBNM] code has il2cpp_image_get_class. BNM will use it.");
        }
#if !BNM_DISABLE_NEW_CLASSES

        //! il2cpp::vm::Class::FromIl2CppType
        if (!old_Class$$FromIl2CppType) {
            // Path:
            // il2cpp_class_from_type ->
            // il2cpp::vm::Class::FromIl2CppType
            auto from_type_adr = HexUtils::FindNextJump((BNM_PTR) BNM_dlsym(il2cppLibraryHandle, OBFUSCATE_BNM("il2cpp_class_from_type")), count);
            HOOK(from_type_adr, Class$$FromIl2CppType, old_Class$$FromIl2CppType);
            BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::Class::FromIl2CppType in lib: %p.", Utils::OffsetInLib((void *)from_type_adr));
        }

        //! il2cpp::vm::Type::GetClassOrElementClass
        if (!old_Type$$GetClassOrElementClass) {
            // Path:
            // il2cpp_type_get_class_or_element_class ->
            // il2cpp::vm::Type::GetClassOrElementClass
            auto from_type_adr = HexUtils::FindNextJump((BNM_PTR) BNM_dlsym(il2cppLibraryHandle, OBFUSCATE_BNM("il2cpp_type_get_class_or_element_class")), count);
            HOOK(from_type_adr, Type$$GetClassOrElementClass, old_Type$$GetClassOrElementClass);
            BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::Type::GetClassOrElementClass in lib: %p.", Utils::OffsetInLib((void *)from_type_adr));
        }

        //! il2cpp::vm::Image::ClassFromName
        if (!old_Class$$FromName) {
            // Path:
            // il2cpp_class_from_name ->
            // il2cpp::vm::Class::FromName ->
            // il2cpp::vm::Image::ClassFromName
            auto from_name_adr = HexUtils::FindNextJump(HexUtils::FindNextJump((BNM_PTR) il2cppMethods.il2cpp_class_from_name, count), count);
            HOOK(from_name_adr, Class$$FromName, old_Class$$FromName);
            BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::Image::FromName in lib: %p.", Utils::OffsetInLib((void *)from_name_adr));
        }
#if UNITY_VER <= 174

        //! il2cpp::vm::MetadataCache::GetImageFromIndex
        if (!old_GetImageFromIndex) {
            // Path:
            // il2cpp_assembly_get_image ->
            // il2cpp::vm::Assembly::GetImage ->
            // il2cpp::vm::MetadataCache::GetImageFromIndex
            auto GetImageFromIndexOffset = HexUtils::FindNextJump(HexUtils::FindNextJump((BNM_PTR) il2cppMethods.il2cpp_assembly_get_image, count), count);
            HOOK(GetImageFromIndexOffset, new_GetImageFromIndex, old_GetImageFromIndex);
            BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::MetadataCache::GetImageFromIndex in lib: %p.", Utils::OffsetInLib((void *)GetImageFromIndexOffset));
        }

        //! il2cpp::vm::Assembly::Load
        static void *old_AssemblyLoad = nullptr;
        if (!old_AssemblyLoad) {
            // Path:
            // il2cpp_domain_assembly_open ->
            // il2cpp::vm::Assembly::Load
            BNM_PTR AssemblyLoadOffset = HexUtils::FindNextJump((BNM_PTR)BNM_dlsym(il2cppLibraryHandle, OBFUSCATE_BNM("il2cpp_domain_assembly_open")), count);
            HOOK(AssemblyLoadOffset, Assembly$$Load, old_AssemblyLoad);
            BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::Assembly::Load in lib: %p.", Utils::OffsetInLib((void *)AssemblyLoadOffset));
        }

#endif
#endif

        //! il2cpp::vm::Assembly::GetAllAssemblies
        if (!Assembly$$GetAllAssemblies) {
#ifdef BNM_USE_APPDOMAIN
            auto assemblyClass = il2cppMethods.il2cpp_class_from_name(il2cppMethods.il2cpp_get_corlib(), OBFUSCATE_BNM("System"), OBFUSCATE_BNM("AppDomain"));
            auto getAssembly = LoadClass(assemblyClass).GetMethodByName(OBFUSCATE_BNM("GetAssemblies"), 1);
            if (getAssembly) {
                const int sCount
#if !defined(__aarch64__) && UNITY_VER >= 211
                    = count;
#else
                    = count + 1;
#endif
                // Path:
                // System.AppDomain.GetAssemblies(bool) ->
                // il2cpp::icalls::mscorlib::System::AppDomain::GetAssemblies ->
                // il2cpp::vm::Assembly::GetAllAssemblies
                BNM_PTR GetAssembliesAdr = HexUtils::FindNextJump(getAssembly.GetOffset(), count);
                Assembly$$GetAllAssemblies = (AssemblyVector *(*)())(HexUtils::FindNextJump(GetAssembliesAdr, sCount));
                BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::Assembly::GetAllAssemblies using AppDomain in lib: %p.", Utils::OffsetInLib((void *)Assembly$$GetAllAssemblies));
            } else {
#endif
            // Path:
            // il2cpp_domain_get_assemblies ->
            // il2cpp::vm::Assembly::GetAllAssemblies
            auto adr = (BNM_PTR) BNM_dlsym(il2cppLibraryHandle, OBFUSCATE_BNM("il2cpp_domain_get_assemblies"));
            Assembly$$GetAllAssemblies = (AssemblyVector *(*)())(HexUtils::FindNextJump(adr, count));
            BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::Assembly::GetAllAssemblies using domain in lib: %p.", Utils::OffsetInLib((void *)Assembly$$GetAllAssemblies));
#ifdef BNM_USE_APPDOMAIN
            }
#endif
        }
        auto mscorlib = il2cppMethods.il2cpp_get_corlib();

        auto runtimeMethodInfoClassPtr = TryGetClassInImage(mscorlib, OBFUSCATE_BNM("System.Reflection"), OBFUSCATE_BNM("RuntimeMethodInfo"));
        if (!runtimeMethodInfoClassPtr) runtimeMethodInfoClassPtr = TryGetClassInImage(mscorlib, OBFUSCATE_BNM("System.Reflection"), OBFUSCATE_BNM("MonoMethod"));
        auto runtimeMethodInfoClass = LoadClass(runtimeMethodInfoClassPtr);

        auto runtimeTypeClass = LoadClass(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("RuntimeType"), mscorlib);
        auto stringClass = LoadClass(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("String"), mscorlib);
        auto interlockedClass = LoadClass(OBFUSCATE_BNM("System.Threading"), OBFUSCATE_BNM("Interlocked"), mscorlib);
        auto objectClass = LoadClass(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Object"), mscorlib);
        for (uint16_t slot = 0; slot < objectClass.klass->vtable_count; slot++) {
            const BNM::IL2CPP::MethodInfo* vmethod = objectClass.klass->vtable[slot].method;
            if (strcmp(vmethod->name, OBFUSCATE_BNM("Finalize")) != 0) continue;
            finalizerSlot = slot;
            break;
        }
        vmData.Object = objectClass;
        vmData.Interlocked$$CompareExchange = interlockedClass.GetMethodByName(OBFUSCATE_BNM("CompareExchange"), {objectClass, objectClass, objectClass});
        vmData.RuntimeType$$MakeGenericType = runtimeTypeClass.GetMethodByName(OBFUSCATE_BNM("MakeGenericType"), 2);
        vmData.RuntimeType$$MakePointerType = runtimeTypeClass.GetMethodByName(OBFUSCATE_BNM("MakePointerType"), 1);
        vmData.RuntimeType$$make_byref_type = runtimeTypeClass.GetMethodByName(OBFUSCATE_BNM("make_byref_type"), 0);
        vmData.RuntimeMethodInfo$$MakeGenericMethod_impl = runtimeMethodInfoClass.GetMethodByName(OBFUSCATE_BNM("MakeGenericMethod_impl"));
        vmData.String$$Empty = stringClass.GetFieldByName(OBFUSCATE_BNM("Empty")).cast<Mono::monoString *>().GetPointer();


        auto listClass = LoadClass(OBFUSCATE_BNM("System.Collections.Generic"), OBFUSCATE_BNM("List`1"));
        auto cls = listClass.klass;
        auto size = sizeof(IL2CPP::Il2CppClass) + cls->vtable_count * sizeof(IL2CPP::VirtualInvokeData);
        listClass.klass = (IL2CPP::Il2CppClass *) malloc(size);
        memcpy(listClass.klass, cls, size);
        listClass.klass->has_finalize = 0;
        listClass.klass->instance_size = sizeof(Mono::monoList<void*>);

        // Bypassing the creation of a static _emptyArray field because it cannot exist
        listClass.klass->has_cctor = 0;
        listClass.klass->cctor_started = 0;
#if UNITY_VER >= 212
        listClass.klass->cctor_finished_or_no_cctor = 1;
#else
        listClass.klass->cctor_finished = 1;
#endif

        auto constructor = listClass.GetMethodByName(BNM_Internal::constructorName, 0).myInfo;

        auto newMethods = (IL2CPP::MethodInfo **) malloc(sizeof(IL2CPP::MethodInfo *) * listClass.klass->method_count);
        memcpy(newMethods, listClass.klass->methods, sizeof(IL2CPP::MethodInfo *) * listClass.klass->method_count);

        auto newConstructor = (IL2CPP::MethodInfo *) malloc(sizeof(IL2CPP::MethodInfo));
        memcpy(newConstructor, constructor, sizeof(IL2CPP::MethodInfo));
        newConstructor->methodPointer = (decltype(newConstructor->methodPointer)) BNM_Internal::Empty;
        newConstructor->invoker_method = (decltype(newConstructor->invoker_method)) BNM_Internal::Empty;

        for (uint16_t i = 0; i < listClass.klass->method_count; ++i) {
            if (listClass.klass->methods[i] == constructor) {
                newMethods[i] = newConstructor;
                continue;
            }
            newMethods[i] = (IL2CPP::MethodInfo *) listClass.klass->methods[i];
        }
        listClass.klass->methods = (const IL2CPP::MethodInfo **) newMethods;
        customListTemplateClass = listClass;
    }

    void BNM_il2cpp_init(const char *domain_name) {
        old_BNM_il2cpp_init(domain_name);

        // Load BNM
        SetupBNM();
#if !BNM_DISABLE_NEW_CLASSES
        InitNewClasses();
        ModifyClasses();
#endif
        bnmLoaded = true;

        // Call all events after il2cpp loaded
        auto &events = GetEvents();
        for (auto event : events) if (event) event();
        events.clear();
    }

#ifndef BNM_DISABLE_AUTO_LOAD
    __attribute__((constructor))
    void PrepareBNM() {
        // Try get libil2cpp.so at library load time
        auto lib = BNM_dlopen(OBFUSCATE_BNM("libil2cpp.so"), RTLD_LAZY);
        if (InitLibraryHandle(lib)) return;
        else BNM_dlclose(lib);

        // Try get libil2cpp.so at background
        BNM_thread loader([]() {
            do {
                if (bnmLoaded) break;
                auto lib = BNM_dlopen(OBFUSCATE_BNM("libil2cpp.so"), RTLD_LAZY);
                if (InitLibraryHandle(lib)) break;
                else BNM_dlclose(lib);
            } while (true);
        });
        loader.detach();
    }
#endif

    // Checking if library is valid
    bool InitLibraryHandle(void *handle, const char *path, bool external) {
        if (!handle) return false;
        void *init = BNM_dlsym(handle, OBFUSCATE_BNM("il2cpp_init"));
        if (!init) return false;

        Dl_info info{};
        int ret = BNM_dladdr(init, &info);
        if (!ret) return false;

        if (!path) {
            auto len = strlen(info.dli_fname);
            path = (char *) malloc(len + 1);
            memcpy((void *) path, (void *) info.dli_fname, len);
            ((char *)path)[len] = 0;
            il2cppLibraryAbsolutePath = path;
        } else il2cppLibraryAbsolutePath = path;

        il2cppLibraryAbsoluteAddress = (BNM_PTR)info.dli_fbase;

        // Hook il2cpp_init, if BNM used internally
        if (!external) HOOK(init, BNM_il2cpp_init, old_BNM_il2cpp_init);

        il2cppLibraryHandle = handle;
        return true;
    }
}

namespace BNM::Structures::Unity {
    void *RaycastHit::GetCollider() const {
        if (!m_Collider || (BNM_PTR) m_Collider < 0) return {};
#if UNITY_VER > 174
        static void * (*FromId)(int);
        if (!FromId) InitResolveFunc(FromId, OBFUSCATE_BNM("UnityEngine.Object::FindObjectFromInstanceID"));
        return FromId(m_Collider);
#else
        return m_Collider;
#endif
    }
    const float Vector4::infinity = std::numeric_limits<float>::infinity();
    const Vector4 Vector4::infinityVec = {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
    const Vector4 Vector4::zero = {0, 0, 0, 0};
    const Vector4 Vector4::one = {1, 1, 1, 1};
    const Vector3 Vector3::back = {0.f, 0.f, -1.f};
    const Vector3 Vector3::down = {0.f, -1.f, 0.f};
    const Vector3 Vector3::forward = {0.f, 0.f, 1.f};
    const float Vector3::kEpsilon = 1E-05f;
    const float Vector3::kEpsilonNormalSqrt = 1E-15f;
    const Vector3 Vector3::left = {-1.f, 0.f, 0.f};
    const Vector3 Vector3::negativeInfinity = {-INFINITY, -INFINITY, -INFINITY};
    const Vector3 Vector3::one = {1.f, 1.f, 1.f};
    const Vector3 Vector3::positiveInfinity = {INFINITY, INFINITY, INFINITY};
    const Vector3 Vector3::right = {1.f, 0.f, 0.f};
    const Vector3 Vector3::up = {0.f, 1.f, 0.f};
    const Vector3 Vector3::zero = {0.f, 0.f, 0.f};
    const Matrix4x4 Matrix4x4::identity(kIdentity);

    Matrix3x3& Matrix3x3::operator=(const Matrix4x4& other) {
        m_Data[0] = other.m_Data[0];
        m_Data[1] = other.m_Data[1];
        m_Data[2] = other.m_Data[2];
        m_Data[3] = other.m_Data[4];
        m_Data[4] = other.m_Data[5];
        m_Data[5] = other.m_Data[6];
        m_Data[6] = other.m_Data[8];
        m_Data[7] = other.m_Data[9];
        m_Data[8] = other.m_Data[10];
        return *this;
    }
    Matrix3x3::Matrix3x3(const Matrix4x4& other) {
        m_Data[0] = other.m_Data[0];
        m_Data[1] = other.m_Data[1];
        m_Data[2] = other.m_Data[2];
        m_Data[3] = other.m_Data[4];
        m_Data[4] = other.m_Data[5];
        m_Data[5] = other.m_Data[6];
        m_Data[6] = other.m_Data[8];
        m_Data[7] = other.m_Data[9];
        m_Data[8] = other.m_Data[10];
    }
    Matrix3x3& Matrix3x3::operator*=(const Matrix4x4& inM) {
        for (int i = 0; i < 3; i++) {
            float v[3] = {Get(i, 0), Get(i, 1), Get(i, 2)};
            Get(i, 0) = v[0] * inM.Get(0, 0) + v[1] * inM.Get(1, 0) + v[2] * inM.Get(2, 0);
            Get(i, 1) = v[0] * inM.Get(0, 1) + v[1] * inM.Get(1, 1) + v[2] * inM.Get(2, 1);
            Get(i, 2) = v[0] * inM.Get(0, 2) + v[1] * inM.Get(1, 2) + v[2] * inM.Get(2, 2);
        }
        return *this;
    }
    bool Matrix3x3::Invert() {
        Matrix4x4 m = *this;
        bool success = InvertMatrix4x4_Full(m.GetPtr(), m.GetPtr());
        *this = m;
        return success;
    }
    Vector2::operator Vector3() const {
        return {x, y, 0};
    }
}