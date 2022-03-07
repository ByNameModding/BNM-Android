#pragma once


std::vector<NewClass *> *Clases4Add;


Il2CppImage *makeOrGetImage(std::string name) {
    DO_API(Il2CppImage*, il2cpp_assembly_get_image, (const Il2CppAssembly * assembly));
    AssemblyVector *LoadedAssemblies = Assembly$$GetAllAssemblies();
    for (auto assembly : *LoadedAssemblies) {
        Il2CppImage *img = il2cpp_assembly_get_image(assembly);
        if (img->nameNoExt == name) {
            if (!BNM_classes_map[(DWORD)img])
                BNM_classes_map[(DWORD)img] = new std::vector<Il2CppClass *>();
            return img;
        }
    }

    Il2CppImage* newImg = (Il2CppImage*)malloc(sizeof(Il2CppImage));
    std::vector<Il2CppClass *> *newVec = new std::vector<Il2CppClass *>();

    newImg->nameNoExt = (char*)calloc(name.size() + 1, sizeof(char));
    strcpy((char*)newImg->nameNoExt, name.c_str());
    std::string nameExt = (name + OBFUSCATES_BNM(".dll"));
    newImg->name = (char*)calloc(nameExt.size() + 1, sizeof(char));
    strcpy((char*)newImg->name, nameExt.c_str());

#if UNITY_VER > 182
    newImg->assembly = 0;
    newImg->customAttributeCount = 0;
#if UNITY_VER < 201
    newImg->customAttributeStart = -1;
#endif
#endif
#if UNITY_VER > 201
    newImg->metadataHandle = (Il2CppMetadataImageHandle)malloc(sizeof(Il2CppImageDefinition));
    ((Il2CppImageDefinition*)newImg->metadataHandle)->typeStart = -1;
    ((Il2CppImageDefinition*)newImg->metadataHandle)->exportedTypeStart = -1;
    ((Il2CppImageDefinition*)newImg->metadataHandle)->typeCount = 0;
    ((Il2CppImageDefinition*)newImg->metadataHandle)->exportedTypeCount = 0;
    ((Il2CppImageDefinition*)newImg->metadataHandle)->nameIndex = -1;
    ((Il2CppImageDefinition*)newImg->metadataHandle)->assemblyIndex = -1;
    ((Il2CppImageDefinition*)newImg->metadataHandle)->customAttributeCount = 0;
    ((Il2CppImageDefinition*)newImg->metadataHandle)->customAttributeStart = -1;
#else
    newImg->typeStart = -1;
    newImg->exportedTypeStart = -1;
#endif
    newImg->typeCount = 0;
    newImg->exportedTypeCount = 0;

    BNM_classes_map.insert(std::make_pair((DWORD)newImg, newVec));
    Il2CppAssembly* newAsm = (Il2CppAssembly*)malloc(sizeof(Il2CppAssembly));
#if UNITY_VER > 174
    newAsm->image = newImg;
    newAsm->image->assembly = newAsm;
    newAsm->aname.name = (char*)calloc(name.size() + 1, sizeof(char));
    strcpy((char*)newAsm->aname.name, name.c_str());
#else
    newImg->assemblyIndex = newAsm->imageIndex = -BNM_classes_map.size();
#endif
    newImg->nameToClassHashTable = (decltype(newImg->nameToClassHashTable))-0x424e4d;
    newAsm->referencedAssemblyStart = -1;
    newAsm->referencedAssemblyCount = 0;

    Assembly$$GetAllAssemblies()->push_back(newAsm);
    return newImg;
}

Il2CppClass* new_Class_FromIl2CppType(const Il2CppType* type){
    if (!type) return nullptr;
    Il2CppClass *cls = 0;
    for (auto clazz : BNM_classes) {
#if UNITY_VER < 181
        if (clazz->byval_arg == type)
#else
        if (&clazz->byval_arg == type)
#endif
        {
            cls = clazz;
        }
    }
    if (!cls)
        cls = old_Class_FromIl2CppType(type);
    return cls;
}
Il2CppClass* new_Class_FromName(const Il2CppImage* image, const char* namespaze, const char *name){
    if (!image) return nullptr;
    Il2CppClass *ret = 0;
    if (((DWORD)image->nameToClassHashTable) != -0x424e4d)
        ret = old_Class_FromName(image, namespaze, name);
    auto vec = BNM_classes_map[(DWORD)image];
    if (!ret && vec)
        for (auto BNM_class : *vec) {
            if (!strcmp(namespaze, BNM_class->namespaze) && !strcmp(name, BNM_class->name)){
                ret = BNM_class;
                break;
            }
        }
    return ret;
}
bool new_Class_Init(Il2CppClass *klass) {
    if (!klass)
        return false;
    if (((DWORD)klass->image->nameToClassHashTable) != -0x424e4d)
        return old_Class_Init(klass);
    return true;
}

void new_Image_GetTypes(const Il2CppImage* image, bool exportedOnly_UNUSED_IN_IL2CPP_SRC, TypeVector* target) {
    if (((DWORD)image->nameToClassHashTable) != -0x424e4d)
        old_Image_GetTypes(image, exportedOnly_UNUSED_IN_IL2CPP_SRC, target);
    auto vec = BNM_classes_map[(DWORD)image];
    if (vec)
        for (auto BNM_class : *vec) {
            target->push_back(BNM_class);
        }
}
#if __cplusplus >= 201703
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
        NameSapce = OBFUSCATE_BNM(namespaze); \
        BaseName = OBFUSCATE_BNM(base_name); \
        BaseNameSapce = OBFUSCATE_BNM(base_namespaze); \
        DllName = OBFUSCATE_BNM("ByNameModding"); \
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

#define NewStaticMethodInit(_type, _name, args, ...) \
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


// Add class to exist or to new dll. Write dll name without '.dll'!
#define NewClassWithDllInit(dll, namespaze, name, base_namespaze, base_name, base_size, type)\
private: \
struct _BNMClass : NewClass { \
    _BNMClass() { \
        Name = OBFUSCATE_BNM(#name); \
        NameSapce = OBFUSCATE_BNM(namespaze); \
        BaseName = OBFUSCATE_BNM(base_name); \
        BaseNameSapce = OBFUSCATE_BNM(base_namespaze); \
        DllName = OBFUSCATE_BNM(dll); \
        classType = type;\
        if (type == 0)\
            classType = Il2CppTypeEnum::IL2CPP_TYPE_VALUETYPE;\
        this->size = sizeof(Me_Type); \
        AddNewClass(this); \
    } \
}; \
static inline _BNMClass BNMClass = _BNMClass(); \
public: \
uint8_t _baseFields[base_size]; \
using Me_Type = name


void InitNewClasses(){
    if (!Clases4Add) return;
    for (auto klass : (*Clases4Add)){
        auto type = new Il2CppType();
        type->type = (Il2CppTypeEnum)klass->GetClassType();
        type->pinned = 0;
        type->byref = 0;
        type->num_mods = 0;
        Il2CppImage *curImg = makeOrGetImage(klass->GetDllName());
        curImg->typeCount++;
        Il2CppClass *parent = LoadClass(klass->GetBaseNameSapce(), klass->GetBaseName()).GetIl2CppClass();
        if (!parent)
            parent = GetType<Il2CppObject *>().ToIl2CppClass();
        std::vector<VirtualInvokeData> newVTable;
        std::vector<Il2CppRuntimeInterfaceOffsetPair> newInterOffsets;
        if (parent && parent->interfaceOffsets) {
            for (uint16_t nI = 0; nI < parent->interface_offsets_count; ++nI) {
                newInterOffsets.push_back(parent->interfaceOffsets[nI]);
            }
        }
        if (parent){
            for (uint16_t v = 0; v < parent->vtable_count; ++v) {
                newVTable.push_back(parent->vtable[v]);
            }
        }
        const MethodInfo **methods = NULL;
        if (klass->Methods4Add != 0 && !klass->Methods4Add->empty()) {
            methods = (const MethodInfo **) calloc(klass->Methods4Add->size(), sizeof(MethodInfo *));
            for (int m = 0; m < klass->Methods4Add->size(); m++) {
                auto method = (*klass->Methods4Add)[m];
                auto virtualMethod = method->virtualMethod;
                if (virtualMethod && !method->statik){
                    bool vInit = false;
#if UNITY_VER > 184
                    if (parent && !parent->initialized_and_no_error){
#else
                    if (parent && !parent->initialized){
#endif
                        Class$$Init(parent);
                    }
                    if (parent && parent->interfaceOffsets){
                        for (uint16_t mi = 0; mi < parent->interface_offsets_count; ++mi) {
                            auto &inter = parent->interfaceOffsets[mi];
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
                    if (parent && parent->interfaceOffsets && !vInit){
                        auto b = parent;
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
                    if (method->args_types && !method->args_types->empty() && ip < method->args_types->size())
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
        if (parent){
            klass->thizClass->parent = parent;
#if UNITY_VER > 182
            if (parent && !parent->initialized_and_no_error){
#else
            if (parent && !parent->initialized){
#endif
                Class$$Init(parent);
            }
            klass->thizClass->typeHierarchyDepth = parent->typeHierarchyDepth + 1;
            klass->thizClass->typeHierarchy = (Il2CppClass**)calloc(klass->thizClass->typeHierarchyDepth, sizeof(Il2CppClass*));
            memcpy(klass->thizClass->typeHierarchy, parent->typeHierarchy, parent->typeHierarchyDepth * sizeof(Il2CppClass*));
            klass->thizClass->typeHierarchy[klass->thizClass->typeHierarchyDepth - 1] = klass->thizClass;
        } else {
            klass->thizClass->typeHierarchyDepth = 1;
        }

        klass->thizClass->image = curImg;
        klass->thizClass->name = klass->GetName();
        klass->thizClass->namespaze = klass->GetNameSapce();
        klass->thizClass->byval_arg = klass->thizClass->this_arg =
#if UNITY_VER > 174
        *type;
#else
        type;
#endif
        type->data.dummy = klass->thizClass;
        klass->thizClass->flags = klass->thizClass->parent->flags & ~0x00000080; // TYPE_ATTRIBUTE_ABSTRACT
        klass->thizClass->element_class = klass->thizClass;
        klass->thizClass->castClass = klass->thizClass;
#if UNITY_VER > 174
        klass->thizClass->klass = klass->thizClass;
#else
        klass->thizClass->declaringType = 0;// = klass->thizClass;
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
        klass->thizClass->typeMetadataHandle = NULL;
#endif
#if UNITY_VER == 202
        klass->thizClass->valuetype = 1;
#endif

        klass->thizClass->token = -1;
        klass->thizClass->has_references = 0;
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
                    newField->token = newField->type->attrs | field->attributes; // FIELD_ATTRIBUTE_PUBLIC
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
                    newField->token = newField->type->attrs | field->attributes; // FIELD_ATTRIBUTE_PUBLIC and FIELD_ATTRIBUTE_STATIC
                    newField++;
                }
                klass->StaticFields4Add->clear();
                klass->thizClass->static_fields = (void *)klass->staticFieldsAdress;
                klass->thizClass->static_fields_size = klass->staticFieldOffset;
            }
            klass->thizClass->fields = fields;
        }
        BNM_classes_map[(DWORD)curImg]->push_back(klass->thizClass);
        BNM_classes.push_back(klass->thizClass);
        LOGIBNM(OBFUSCATE_BNM("[InitNewClasses] Added new class: [%s]::[%s] parent is [%s]::[%s] to [%s]"), klass->GetNameSapce(), klass->GetName(), klass->GetBaseNameSapce(), klass->GetBaseName(), klass->thizClass->image->name);
    }
    Clases4Add->clear();
}
#endif