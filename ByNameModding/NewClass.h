#pragma once
#include "NewClass_structs.h"

static Il2CppAssembly* BNM_assembly = nullptr;
static Il2CppImage * BNM_Image = nullptr;
static std::vector<Il2CppClass *> BNM_classes;
std::vector<NewClass *> *Clases4Add;
Il2CppTypeEnum BNM_cls_type = (Il2CppTypeEnum)(Il2CppTypeEnum::IL2CPP_TYPE_VALUETYPE | 384); // I forgot what is 384

bool InitBNMAssembly() {
    DO_API(const Il2CppImage*, il2cpp_assembly_get_image, (const Il2CppAssembly * assembly));
    AssemblyVector *LoadedAssemblies = Assembly$$GetAllAssemblies();
    const Il2CppAssembly ** assemblies = &(*LoadedAssemblies)[0];
    for (int32_t i = 0; i < LoadedAssemblies->size(); i++){
        auto assembly = assemblies[i];
#if UNITY_VER > 174
        if (assembly->aname.name == OBFUSCATE("ByNameModding"))
            return true;
#else
        if (assembly->aname.nameIndex == -0x424e4d) //B(42) N(4e) M(4d) BNM
            return true;
#endif
    }

    if (!BNM_assembly){
        BNM_Image = new Il2CppImage();
        BNM_Image->name = OBFUSCATE("ByNameModding.dll");
#if UNITY_VER > 182
        BNM_Image->assembly = 0;
        BNM_Image->customAttributeCount = -1;
#if UNITY_VER < 201
        BNM_Image->customAttributeStart = -1;
#endif
#endif
        BNM_Image->nameNoExt = OBFUSCATE("ByNameModding");
#if UNITY_VER > 201
        BNM_Image->metadataHandle = (Il2CppMetadataImageHandle)malloc(sizeof(Il2CppImageDefinition));
        ((Il2CppImageDefinition*)BNM_Image->metadataHandle)->typeStart = -1;
        ((Il2CppImageDefinition*)BNM_Image->metadataHandle)->exportedTypeStart = -1;
        ((Il2CppImageDefinition*)BNM_Image->metadataHandle)->typeCount = 0;
        ((Il2CppImageDefinition*)BNM_Image->metadataHandle)->exportedTypeCount = 0;
        ((Il2CppImageDefinition*)BNM_Image->metadataHandle)->customAttributeCount = -1;
        ((Il2CppImageDefinition*)BNM_Image->metadataHandle)->customAttributeStart = -1;
#else
        BNM_Image->typeStart = -1;
        BNM_Image->exportedTypeStart = -1;
#endif
        BNM_Image->typeCount = 0;
        BNM_Image->exportedTypeCount = 0;
        BNM_assembly = new Il2CppAssembly();
#if UNITY_VER > 174
        BNM_assembly->image = BNM_Image;
        BNM_assembly->image->assembly = BNM_assembly;
        BNM_assembly->aname.name = BNM_Image->nameNoExt;
#else
        BNM_assembly->aname.nameIndex = -0x424e4d; //B(42) N(4e) M(4d) BNM
#endif
        BNM_assembly->referencedAssemblyStart = -1;
        BNM_assembly->referencedAssemblyCount = 0;
        LoadedAssemblies->push_back(BNM_assembly);
    }
    return true;
}
Il2CppClass* new_Class_FromIl2CppType(const Il2CppType* type){
    Il2CppClass* klass = old_Class_FromIl2CppType(type);
    if (!klass)
        for (auto clazz : BNM_classes) {
#if UNITY_VER < 181
            if (clazz->byval_arg == type)
#else
            if (&clazz->byval_arg == type)
#endif
            {
                klass = clazz;
                break;
            }
        }
    return klass;
}
Il2CppClass* new_Class_FromName(const Il2CppImage* image, const char* namespaze, const char *name){
    if (!image) return nullptr;
    if (!strcmp(image->name, OBFUSCATE_BNM("ByNameModding.dll"))){
        for (auto BNM_class : BNM_classes) {
            if (!strcmp(namespaze, BNM_class->namespaze) && !strcmp(name, BNM_class->name)){
                return BNM_class;
                break;
            }
        }
    } else {
        return old_Class_FromName(image, namespaze, name);
    }
}
bool new_Class_Init(Il2CppClass *klass) {
    if (!klass)
        return false;
    if (strcmp(klass->image->name, OBFUSCATE_BNM("ByNameModding.dll")))
        return old_Class_Init(klass);
    return true;
}

void new_Image_GetTypes(const Il2CppImage* image, bool exportedOnly_UNUSED_IN_IL2CPP_SRC, TypeVector* target) {
    if (strcmp(image->name, OBFUSCATE_BNM("ByNameModding.dll"))) {
        return old_Image_GetTypes(image, exportedOnly_UNUSED_IN_IL2CPP_SRC, target);
    } else {
        for (auto BNM_class : BNM_classes) {
            target->push_back(BNM_class);
        }
    }
}

void AddNewClass(NewClass *klass){
    if (!Clases4Add)
        Clases4Add = new std::vector<NewClass *>();
    Clases4Add->push_back(klass);
}

#define NewClassInit(namespaze, name, base_namespaze, base_name, base_size)\
private: \
struct _BNMClass : NewClass { \
    _BNMClass() { \
        Name = OBFUSCATE_BNM(#name); \
        NameSapce = OBFUSCATE_BNM(#namespaze); \
        BaseName = OBFUSCATE_BNM(base_name); \
        BaseNameSapce = OBFUSCATE_BNM(base_namespaze); \
        this->size = sizeof(Me_Type); \
        AddNewClass(this); \
    } \
}; \
static inline _BNMClass BNMClass = _BNMClass(); \
public: \
uint8_t _baseFields[base_size]; \
using Me_Type = name



#define NewMethodInit(_type, _name, args, ...) \
private: \
struct _BNMMethod_##_name : NewMethod { \
    _BNMMethod_##_name() { \
        thizMethod = new MethodInfo(); \
        thizMethod->parameters_count = args; \
        ret_type = _type; \
        thizMethod->methodPointer = (Il2CppMethodPointer)&GetNewMethodCalls<decltype(&Me_Type::_name)>::get<&Me_Type::_name>; \
        thizMethod->invoker_method = (InvokerMethod)&GetNewMethodCalls<decltype(&Me_Type::_name)>::invoke; \
        args_types = new std::vector<TypeFinder>({__VA_ARGS__}); \
        Name = OBFUSCATE_BNM(#_name); \
        BNMClass.AddNewMethod(this); \
    } \
}; \
public: \
static inline _BNMMethod_##_name BNMMethod_##_name = _BNMMethod_##_name()

#define NewStaticNethodInit(_type, _name, args, ...) \
private: \
struct _BNMStaticMethod_##_name : NewMethod { \
    _BNMStaticMethod_##_name() { \
        thizMethod = new MethodInfo(); \
        thizMethod->parameters_count = args; \
        ret_type = _type; \
        thizMethod->methodPointer = (Il2CppMethodPointer)&Me_Type::_name; \
        thizMethod->invoker_method = (InvokerMethod)&GetNewStaticMethodCalls<decltype(&Me_Type::_name)>::invoke; \
        args_types = new std::vector<TypeFinder>({__VA_ARGS__}); \
        Name = OBFUSCATE_BNM(#_name); \
        statik = true; \
        BNMClass.AddNewMethod(this); \
    } \
}; \
public: \
static inline _BNMStaticMethod_##_name BNMStaticMethod_##_name = _BNMStaticMethod_##_name()


#define NewFieldInit(_name, type) \
private: \
struct _BNMField_##_name : NewField { \
    _BNMField_##_name() { \
        thizField = new FieldInfo(); \
        Name = OBFUSCATE_BNM(#_name); \
        offset = offsetof(Me_Type, _name); \
        attributes |= 0x0006;      \
        MYtype = type; \
        BNMClass.AddNewField(this, false); \
    } \
}; \
static inline _BNMField_##_name BNMField_##_name = _BNMField_##_name()


#define NewStaticFieldInit(_name, type, cpptype) \
private: \
struct _BNMStaticField_##_name : NewField { \
    _BNMStaticField_##_name() { \
        thizField = new FieldInfo(); \
        Name = OBFUSCATE_BNM(#_name); \
        size = sizeof(cpptype); \
        cppOffset = (size_t)&_name; \
        attributes |= 0x0006 | 0x0010; \
        MYtype = type; \
        BNMClass.AddNewField(this, true); \
    } \
}; \
static inline _BNMStaticField_##_name BNMStaticField_##_name = _BNMStaticField_##_name()


void InitNewClasses(){
    for (int i = 0; i < Clases4Add->size(); i++){
        NewClass *klass = (*Clases4Add)[i];
        auto* type = new Il2CppType();
        type->type = BNM_cls_type;
        type->pinned = 30;
        type->byref = 30;
        type->num_mods = 0;
        type->data.type = type;
        if (!BNM_assembly)
            InitBNMAssembly();
        BNM_Image->typeCount++;
#if UNITY_VER > 201
        ((Il2CppImageDefinition*)BNM_Image->metadataHandle)->typeCount++;
#endif
#if UNITY_VER < 202
        type->data.klassIndex
#else
        type->data.__klassIndex
#endif
        = BNM_Image->typeCount;
        auto parentLC = LoadClass(klass->GetBaseNameSapce(), klass->GetBaseName());
        std::vector<VirtualInvokeData> newVTable;
        std::vector<Il2CppRuntimeInterfaceOffsetPair> newInterOffsets;
        if (parentLC.klass->interfaceOffsets) {
            for (uint16_t nI = 0; nI < parentLC.klass->interface_offsets_count; ++nI) {
                newInterOffsets.push_back(parentLC.klass->interfaceOffsets[nI]);
            }
        }
        for (uint16_t v = 0; v < parentLC.klass->vtable_count; ++v) {
            newVTable.push_back(parentLC.klass->vtable[v]);
        }
        const MethodInfo **methods = NULL;
        if (klass->Methods4Add != 0 && !klass->Methods4Add->empty()) {
            methods = (const MethodInfo **) calloc(klass->Methods4Add->size(), sizeof(MethodInfo *));
            for (int m = 0; m < klass->Methods4Add->size(); m++) {
                auto method = (*klass->Methods4Add)[m];
                auto virtualMethod = method->virtualMethod;
                if (virtualMethod && !method->statik){
                    bool vInit = false;
                    if (!parentLC.GetIl2CppClass()->initialized){
                        Class$$Init(parentLC.GetIl2CppClass());
                    }
                    if (parentLC.GetIl2CppClass()->interfaceOffsets){
                        for (uint16_t mi = 0; mi < parentLC.GetIl2CppClass()->interface_offsets_count; ++mi) {
                            auto &inter = parentLC.GetIl2CppClass()->interfaceOffsets[mi];
#if UNITY_VER > 174
                            if (inter.interfaceType == virtualMethod->klass){
#else
                            if (inter.interfaceType == virtualMethod->declaring_type){
#endif
                                method->thizMethod->slot = virtualMethod->slot + inter.offset;
                                newVTable[method->thizMethod->slot].method = method->thizMethod;
                                newVTable[method->thizMethod->slot].methodPtr = method->thizMethod->methodPointer;
                                vInit = true;
                                break;
                            }
                        }
                    }
                    if (parentLC.GetIl2CppClass()->interfaceOffsets && !vInit){
                        auto* b = parentLC.GetIl2CppClass();
                        while (b != nullptr) {
#if UNITY_VER > 174
                            if (virtualMethod->klass == b) {
#else
                            if (virtualMethod->declaring_type == b){
#endif
                                method->thizMethod->slot = virtualMethod->slot;
                                newVTable[method->thizMethod->slot].method = method->thizMethod;
                                newVTable[method->thizMethod->slot].methodPtr = method->thizMethod->methodPointer;
                                vInit = true;
                                break;
                            }
                            b = b->parent;
                        }
                    }
                    if (!vInit && !klass->Interfaces.empty()) {
                        for (auto inter : klass->Interfaces) {
#if UNITY_VER > 174
                            if (virtualMethod->klass == inter) {
#else
                            if (virtualMethod->declaring_type == inter){
#endif
                                auto& off = newInterOffsets.emplace_back();
                                off.interfaceType = inter;
                                off.offset = newVTable.size();
                                for (int q = 0; q < inter->method_count; q++) {
                                    newVTable.emplace_back();
                                }
                                method->thizMethod->slot = off.offset + virtualMethod->slot;
                                newVTable[method->thizMethod->slot].method = method->thizMethod;
                                newVTable[method->thizMethod->slot].methodPtr = method->thizMethod->methodPointer;
                                break;
                            }
                        }
                    }
                }
                method->thizMethod->name = method->GetName();
                if (method->statik){
                    method->thizMethod->flags =
                            Mono_Method_attributes::PUBLIC | Mono_Method_attributes::STATIC | Mono_Method_attributes::HIDE_BY_SIG;
                } else {
                    method->thizMethod->flags =
                            Mono_Method_attributes::PUBLIC | Mono_Method_attributes::HIDE_BY_SIG;
                }
                method->thizMethod->is_generic = false;
                method->thizMethod->return_type = method->ret_type.ToIl2CppType();
                for (int ip = 0; ip < method->thizMethod->parameters_count; ip++){
                    auto newParam = new ParameterInfo ();
                    newParam->name = (OBFUSCATES_BNM("arg") + to_string(ip)).c_str();
                    newParam->position = ip;
                    if (method->args_types && !method->args_types->empty() && i < method->args_types->size())
                        newParam->parameter_type = (*method->args_types)[ip].ToIl2CppType();
                }
                methods[m] = method->thizMethod;
            }
        }
        klass->thizClass = (Il2CppClass*)malloc(sizeof(Il2CppClass) + newVTable.size() * sizeof(VirtualInvokeData));
        if (klass->Methods4Add && !klass->Methods4Add->empty()){
            for (int im = 0; im < klass->Methods4Add->size(); im++){
                ((MethodInfo *)methods[im])
#if UNITY_VER > 174
                ->klass = klass->thizClass;
#else
                ->declaring_type = klass->thizClass;
#endif
            }
            klass->thizClass->method_count = klass->Methods4Add->size();
            klass->thizClass->methods = methods;
            klass->Methods4Add->clear();
        } else {
            klass->thizClass->method_count = 0;
            klass->thizClass->methods = NULL;
        }
        klass->thizClass->parent = parentLC.GetIl2CppClass();
        klass->thizClass->image = BNM_Image;
        klass->thizClass->name = klass->GetName();
        klass->thizClass->namespaze = klass->GetNameSapce();
        klass->thizClass->byval_arg = klass->thizClass->this_arg =
#if UNITY_VER > 174
        *type;
#else
        type;
#endif
        klass->thizClass->flags = klass->thizClass->parent->flags & ~0x00000080; // TYPE_ATTRIBUTE_ABSTRACT
        klass->thizClass->element_class = klass->thizClass;
        klass->thizClass->castClass = klass->thizClass;
#if UNITY_VER > 174
        klass->thizClass->klass = klass->thizClass;
#endif
        klass->thizClass->native_size -1;
        klass->thizClass->actualSize = klass->size;
        klass->thizClass->instance_size = klass->size;
        klass->thizClass->vtable_count = newVTable.size();
        for (int vi = 0; vi < newVTable.size(); vi++) {
            klass->thizClass->vtable[vi] = newVTable[vi];
        }
        klass->thizClass->interface_offsets_count = newInterOffsets.size();
        klass->thizClass->interfaceOffsets = (Il2CppRuntimeInterfaceOffsetPair*)(calloc(newInterOffsets.size(), sizeof(Il2CppRuntimeInterfaceOffsetPair)));
        for (int ii = 0; ii < newInterOffsets.size(); ii++) {
            klass->thizClass->interfaceOffsets[ii] = newInterOffsets[ii];
        }
        if (!klass->Interfaces.empty()){
            auto intfs = klass->Interfaces;
            klass->thizClass->interfaces_count = intfs.size();
            klass->thizClass->implementedInterfaces = (Il2CppClass**)calloc(intfs.size(), sizeof(Il2CppClass*));
            for (int ii = 0; ii < intfs.size(); ii++) {
                klass->thizClass->implementedInterfaces[ii] = intfs[ii];
            }
        }
        klass->thizClass->generic_class = nullptr;
        klass->thizClass->genericRecursionDepth = 1;
        klass->thizClass->initialized = 1;
#if UNITY_VER > 182
        klass->thizClass->initialized_and_no_error = 1;
        klass->thizClass->has_initialization_error = 0;
        klass->thizClass->naturalAligment = 8;
#endif
        klass->thizClass->init_pending = 0;
#if UNITY_VER < 202
        klass->thizClass->genericContainerIndex = -1;
#else
        klass->thizClass->genericContainerHandle = NULL;
#endif
#if UNITY_VER == 202
        klass->thizClass->valuetype = 1;
#endif
        klass->thizClass->token = -1;
        klass->thizClass->has_references = 1;
        klass->thizClass->has_finalize = 0;
        klass->thizClass->size_inited = 1;
        klass->thizClass->has_cctor = 0;
        klass->thizClass->enumtype = 0;
        klass->thizClass->minimumAlignment = 8;
        klass->thizClass->is_generic = 0;
        size_t fields_size = 0;
        if (klass->Fields4Add)
            fields_size += klass->Fields4Add->size();
        if (klass->StaticFields4Add)
            fields_size += klass->StaticFields4Add->size();
        klass->thizClass->field_count = fields_size;
        if (klass->thizClass->field_count > 0){
            auto fields = (FieldInfo *)calloc(klass->thizClass->field_count, sizeof(FieldInfo));
            FieldInfo* newField = fields;
            if (klass->Fields4Add && !klass->Fields4Add->empty()) {
                for (int f = 0; f < klass->Fields4Add->size(); f++) {
                    auto field = (*klass->Fields4Add)[f];
                    newField->name = field->GetName();
                    newField->type = field->MYtype.ToIl2CppType();
                    newField->parent = klass->thizClass;
                    newField->offset = field->offset;
                    ((Il2CppType *) newField->type)->attrs = newField->token = field->attributes; // FIELD_ATTRIBUTE_PUBLIC
                    newField++;
                }
                klass->Fields4Add->clear();
            }
            if (klass->StaticFields4Add && !klass->StaticFields4Add->empty()){
                for (int sf = 0; sf < klass->StaticFields4Add->size(); sf++){
                    auto field = (*klass->StaticFields4Add)[sf];
                    newField->name = field->GetName();
                    newField->type = field->MYtype.ToIl2CppType();
                    newField->parent = klass->thizClass;
                    newField->offset = field->offset;
                    ((Il2CppType *)newField->type)->attrs = newField->token = field->attributes; // FIELD_ATTRIBUTE_PUBLIC and FIELD_ATTRIBUTE_STATIC
                    newField++;
                }
                klass->StaticFields4Add->clear();
                klass->thizClass->static_fields = (void *)klass->staticFieldsAdress;
                klass->thizClass->static_fields_size = klass->staticFieldOffset;
            }
            klass->thizClass->fields = fields;
        }
        BNM_classes.push_back(klass->thizClass);
        LOGIBNM(OBFUSCATE_BNM("[InitNewClasses] Added new class: [%s]::[%s]"), klass->GetNameSapce(), klass->GetName());
    }
    Clases4Add->clear();
}
