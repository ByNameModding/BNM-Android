#pragma once

#include <iostream>
#include <dlfcn.h>
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <sstream>
#include <vector>
#include <any>
#include <setjmp.h>
#include <android/log.h>

#define BNMTAG "ByNameModding"
#define LOGIBNM(...) ((void)__android_log_print(4,  BNMTAG, __VA_ARGS__))
typedef unsigned long DWORD;
#ifdef __arm__
DWORD MetadataRegistrationOffset = 0x0;
DWORD Il2CppRegistrationOffset = 0x0;

#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>

#define HOOK(offset, a, b) if (offset != 0) MSHookFunction((void *)offset, (void *) a, (void **) &b)
#elif defined(__i386__) //x86

DWORD MetadataRegistrationOffset = 0x0;
DWORD Il2CppRegistrationOffset = 0x0;

#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>

#define HOOK(offset, a, b) if (offset != 0) MSHookFunction((void *)offset, (void *) a, (void **) &b)
#elif defined(__aarch64__) //arm64-v8a

DWORD MetadataRegistrationOffset = 0x0;
DWORD Il2CppRegistrationOffset = 0x0;

#include <And64InlineHook/And64InlineHook.hpp>

#define HOOK(offset, a, b) if (offset != 0) A64HookFunction((void *)offset, (void *) a, (void **) &b)
#endif
#define InitResolveFunc(x, y) *reinterpret_cast<void **>(&x) = get_Method(y)
#define InitFunc(x, y) if (y != 0) *(void **)(&x) = (void *)(y)
#define FieldBN(myfield, type, inst, nameSpacec, clazzz, _new, fieldName, key) Field<type> myfield = LoadClass(OBFUSCATE_KEY(nameSpacec, key), OBFUSCATE_KEY(clazzz, key), _new).GetFieldByName<type>(OBFUSCATE_KEY(fieldName, key), inst)
#define FieldBNC(type, inst, nameSpacec, clazzz, _new, fieldName, key) LoadClass(OBFUSCATE_KEY(nameSpacec, key), OBFUSCATE_KEY(clazzz, key), _new).GetFieldByName<type>(OBFUSCATE_KEY(fieldName, key), inst)
void *s_Il2CppMetadataRegistration = 0;
void *s_Il2CppCodeRegistration = 0;

typedef Il2CppClass *(*class_from_name_t)(const Il2CppImage *assembly, const char *name_space,
                                          const char *name);

typedef MethodInfo *(*class_get_method_from_name_t)(Il2CppClass *klass, const char *name,
                                                    int paramcount);

typedef Il2CppDomain *(*domain_get_t)();

typedef const Il2CppAssembly **(*domain_get_assemblies_t)(const Il2CppDomain *domain, size_t *size);

typedef const Il2CppImage *(*assembly_get_image_t)(const Il2CppAssembly *assembly);

typedef const Il2CppAssembly *(*domain_assembly_open_t)(Il2CppDomain *domain, const char *name);

typedef FieldInfo *(*class_get_field_from_name_t)(Il2CppClass *klass, const char *name);

typedef const Il2CppType *(*class_get_type_t)(Il2CppClass *klass);

typedef Il2CppClass *(*class_from_type_t)(const Il2CppType *);

typedef Il2CppClass *(*class_get_interfaces_t)(Il2CppClass *klass, void **iter);

typedef Il2CppObject *(*type_get_object_t)(const Il2CppType *type);

typedef Il2CppObject *(*object_new_t)(Il2CppClass *klass);

typedef Il2CppObject *(*object_new_t)(Il2CppClass *klass);

typedef Il2CppObject *(*runtime_invoke_t)(const MethodInfo *method,
                                          void *obj, void **params, Il2CppException **exc);

void *get_il2cpp() {
    void *mod = 0;
    while (!mod) {
        mod = dlopen("libil2cpp.so", RTLD_LAZY);
    }
    return mod;
}

// Some modified version of this:
// https://stackoverflow.com/a/15340456
jmp_buf jump;

void segv(int sig) {
    longjmp(jump, 1);
}

template<typename T>
bool isNOT_Allocated(T x) {
    volatile char c;
    bool illegal = false;
    signal(SIGSEGV, segv);
    if (!setjmp(jump))
        c = *(char *) (x);
    else
        illegal = true;
    signal(SIGSEGV, SIG_DFL);
    return illegal;
}

template<typename T>
T CheckObj(T obj) {
    if (isNOT_Allocated(obj) || obj == NULL) {
        return NULL;
    }
    return obj;
}

std::string revhexstr(std::string hex) {
    std::string out;
    for (unsigned int i = 0; i < hex.length(); i += 2) out = hex.substr(i, 2) + out;
    return out;
}

std::string fixhex(std::string str) {
    std::string out = str;
    std::string::size_type tmp;
    if (out.find(std::string("0x")) != -1) {
        tmp = out.find(std::string("0x"));
        out.replace(tmp, 2, std::string(""));
    }
    out.erase(std::remove(out.begin(), out.end(), ' '), out.end());
    return out;
}

std::string DWORD2HexStr(DWORD hex) {
    stringstream stream;
    stream << std::hex << hex;
    return stream.str();
}

DWORD HexStr2DWORD(std::string hex) {
    return strtoull(hex.c_str(), NULL, 16);
}

DWORD getOffsetFromB_Hex(std::string hex, DWORD offset, bool idk = false) {
    std::string strOffset = DWORD2HexStr(offset);
    hex = revhexstr(fixhex(hex));
    if (strOffset.length() < 7 && !idk) {
        hex = std::string("00") + hex.substr(2);
    } else {
        hex = std::string("FF") + hex.substr(2);
    }
    DWORD hexdw = (HexStr2DWORD(hex) << 2) + 8; // get offset from fixed opcode "b #offset"
    if (DWORD2HexStr(hexdw).length() > strOffset.length()) {
        return HexStr2DWORD(DWORD2HexStr(hexdw + offset).substr(
                DWORD2HexStr(hexdw).length() - strOffset.length()));
    }
    return hexdw + offset;
}

struct Metadata {
    void *file;
    const Il2CppGlobalMetadataHeader *header;
    const Il2CppMetadataRegistration *registartion;
    const Il2CppCodeRegistration *Il2CppRegistration;
    uint32_t version;
    int32_t ImagesCount;
    const Il2CppImageDefinition *ImageDefinitionTable;
    const Il2CppTypeDefinition *TypeDefinitionTable;
    const Il2CppFieldDefinition *FieldDefinitionTable;
    const Il2CppMethodDefinition *MethodDefinitionTable;
    const Il2CppParameterDefinition *ParameterDefinitionTable;
    Il2CppImage *ImagesTable;
    Il2CppClass **TypeInfoDefinitionTable;
    Il2CppClass **TypeInfoTable;

    template<typename T>
    static T MetadataOffset(void *metadata, size_t sectionOffset, size_t itemIndex) {
        return reinterpret_cast<T>(reinterpret_cast<uint8_t *>(metadata) + sectionOffset) +
               itemIndex;
    }

    const Il2CppType *GetIl2CppTypeFromIndex(TypeIndex index) {
        return CheckObj(registartion->types[index]);
    }

    const Il2CppImageDefinition *getImageDefinitionByIndex(ImageIndex index) {
        const Il2CppImageDefinition *imageDef = (const Il2CppImageDefinition *) (
                ImageDefinitionTable + index);
        return imageDef;
    }

    const Il2CppTypeDefinition *getTypeDefinitionByIndex(TypeDefinitionIndex index) {
        const Il2CppTypeDefinition *typeDef = (const Il2CppTypeDefinition *) (TypeDefinitionTable +
                                                                              index);
        return typeDef;
    }

    const Il2CppFieldDefinition *getFieldDefinitionByIndex(FieldIndex index) {
        const Il2CppFieldDefinition *fieldDef = (const Il2CppFieldDefinition *) (
                FieldDefinitionTable + index);
        return fieldDef;
    }

    const Il2CppMethodDefinition *getMethodDefinitionByIndex(MethodIndex index) {
        const Il2CppMethodDefinition *methodDefinition = (const Il2CppMethodDefinition *) (
                MethodDefinitionTable + index);
        return methodDefinition;
    }

    const Il2CppParameterDefinition *getParameterDefinitionByIndex(ParameterIndex index) {
        const Il2CppParameterDefinition *parameterDefinition = (const Il2CppParameterDefinition *) (
                ParameterDefinitionTable + index);
        return parameterDefinition;
    }

    Il2CppClass *GetTypeInfoFromTypeDefinitionIndex(TypeDefinitionIndex index) {
        if (index == kTypeIndexInvalid)
            return NULL;

        if (!TypeInfoDefinitionTable[index]) {
            Il2CppClass *klass = FromTypeDefinition(index);
            TypeInfoDefinitionTable[index] = klass;
        }

        return TypeInfoDefinitionTable[index];
    }

    const char *GetStringFromIndex(StringIndex index) {
        const char *strings = (const char *) ((uint64_t) file + header->stringOffset) + index;
        return strings;
    }


    MethodInfo *GetMethodInfoFromIndex(MethodIndex index) {
        const Il2CppMethodDefinition *methodDefinition = getMethodDefinitionByIndex(index);
        Il2CppClass *typeInfo = GetTypeInfoFromTypeDefinitionIndex(methodDefinition->declaringType);
        MethodInfo *newMethod = new MethodInfo();
        newMethod->name = GetStringFromIndex(methodDefinition->nameIndex);
        newMethod->methodPointer = GetMethodPointer(typeInfo->image, methodDefinition->token);
        newMethod->invoker_method = GetMethodInvoker(typeInfo->image,
                                                     methodDefinition->token);
        newMethod->klass = typeInfo;
        newMethod->return_type = GetIl2CppTypeFromIndex(methodDefinition->returnType);
        ParameterInfo *parameters = CheckObj((ParameterInfo *) calloc(
                methodDefinition->parameterCount, sizeof(ParameterInfo)));
        ParameterInfo *newParameter = parameters;
        for (uint16_t paramIndex = 0;
             paramIndex < methodDefinition->parameterCount; ++paramIndex) {
            const Il2CppParameterDefinition *parameterDefinition = CheckObj(
                    getParameterDefinitionByIndex(
                            (methodDefinition->parameterStart + paramIndex)));
            if (!isNOT_Allocated(parameterDefinition) && parameterDefinition != NULL) {
                newParameter->name = CheckObj(
                        GetStringFromIndex(parameterDefinition->nameIndex));
                newParameter->position = paramIndex;
                newParameter->token = CheckObj(parameterDefinition->token);
                newParameter->parameter_type = CheckObj(GetIl2CppTypeFromIndex(
                        parameterDefinition->typeIndex));
                newParameter++;
            }
        }
        newMethod->parameters = parameters;

        newMethod->flags = methodDefinition->flags;
        newMethod->iflags = methodDefinition->iflags;
        newMethod->slot = methodDefinition->slot;
        newMethod->parameters_count = static_cast<const uint8_t>(methodDefinition->parameterCount);
        newMethod->is_inflated = false;
        newMethod->token = methodDefinition->token;
        newMethod->methodDefinition = methodDefinition;
        if (newMethod->genericContainer)
            newMethod->is_generic = true;

        return newMethod;
    }

    const Il2CppImage *GetImageForTypeDefinitionIndex(TypeDefinitionIndex index) {
        const Il2CppImageDefinition *imagesDefinitions = (const Il2CppImageDefinition *) (
                (const char *) file + header->imagesOffset);
        for (int32_t imageIndex = 0; imageIndex < ImagesCount; imageIndex++) {
            const Il2CppImageDefinition *imageDefinition = imagesDefinitions + imageIndex;
            if (index >= imageDefinition->typeStart &&
                static_cast<uint32_t>(index) <
                (imageDefinition->typeStart + imageDefinition->typeCount)) {
                Il2CppImage *image = ImagesTable + imageIndex;
                image->name = GetStringFromIndex(imageDefinition->nameIndex);
                const size_t pos = std::string(image->name).rfind('.');
                if (pos == std::string::npos)
                    image->nameNoExt = image->name;
                image->nameNoExt = std::string(image->name).substr(0, pos).c_str();
                image->typeStart = imageDefinition->typeStart;
                image->typeCount = imageDefinition->typeCount;
                image->exportedTypeStart = imageDefinition->exportedTypeStart;
                image->exportedTypeCount = imageDefinition->exportedTypeCount;
                image->entryPointIndex = imageDefinition->entryPointIndex;
                image->token = imageDefinition->token;
                image->customAttributeStart = imageDefinition->customAttributeStart;
                image->customAttributeCount = imageDefinition->customAttributeCount;
                for (uint32_t codeGenModuleIndex = 0; codeGenModuleIndex <
                                                      Il2CppRegistration->codeGenModulesCount; ++codeGenModuleIndex) {
                    const Il2CppCodeGenModule *il2CppCodeGenModule = Il2CppRegistration->codeGenModules[codeGenModuleIndex];
                    if (CheckObj(il2CppCodeGenModule->moduleName) &&
                        strcmp(image->name, il2CppCodeGenModule->moduleName) == 0) {
                        image->codeGenModule = Il2CppRegistration->codeGenModules[codeGenModuleIndex];
                        break;
                    }
                }
                image->dynamic = false;
                return image;
            }
        }
        return NULL;
    }

    const TypeDefinitionIndex GetIndexForTypeDefinition(const Il2CppClass *typeDefinition) {
        const Il2CppTypeDefinition *typeDefinitions = (const Il2CppTypeDefinition *) (
                (const char *) file + header->typeDefinitionsOffset);
        ptrdiff_t index = typeDefinition->typeDefinition - typeDefinitions;;
        return static_cast<TypeDefinitionIndex>(index);
    }

    FieldInfo *GetFieldInfoFromIndexAndClass(FieldIndex index, Il2CppClass *klass) {
        const Il2CppFieldDefinition *fieldDefinition = getFieldDefinitionByIndex(index);
        FieldInfo *newField = new FieldInfo();
        newField->type = GetIl2CppTypeFromIndex(fieldDefinition->typeIndex);
        newField->name = GetStringFromIndex(fieldDefinition->nameIndex);
        newField->parent = klass;
        newField->offset = registartion->fieldOffsets[GetIndexForTypeDefinition(klass)][index -
                                                                                        klass->typeDefinition->fieldStart];
        newField->token = fieldDefinition->token;
        return newField;
    }

    Il2CppClass *FromTypeDefinition(TypeDefinitionIndex index) {
        const Il2CppTypeDefinition *typeDefinitions = (const Il2CppTypeDefinition *) (
                (const char *) file + header->typeDefinitionsOffset);
        const Il2CppTypeDefinition *typeDefinition = typeDefinitions + index;
        const Il2CppTypeDefinitionSizes *typeDefinitionSizes = registartion->typeDefinitionsSizes[index];
        Il2CppClass *typeInfo = (Il2CppClass *) calloc(1, sizeof(Il2CppClass) +
                                                          (sizeof(VirtualInvokeData) *
                                                           typeDefinition->vtable_count));
        typeInfo->klass = typeInfo;
        typeInfo->image = GetImageForTypeDefinitionIndex(index);
        typeInfo->name = GetStringFromIndex(typeDefinition->nameIndex);
        typeInfo->namespaze = GetStringFromIndex(typeDefinition->namespaceIndex);
        typeInfo->byval_arg = *GetIl2CppTypeFromIndex(typeDefinition->byvalTypeIndex);
        typeInfo->this_arg = *GetIl2CppTypeFromIndex(typeDefinition->byrefTypeIndex);
        typeInfo->typeDefinition = typeDefinition;
        typeInfo->genericContainerIndex = typeDefinition->genericContainerIndex;
        typeInfo->instance_size = typeDefinitionSizes->instance_size;
        typeInfo->actualSize = typeDefinitionSizes->instance_size;
        typeInfo->native_size = typeDefinitionSizes->native_size;
        typeInfo->static_fields_size = typeDefinitionSizes->static_fields_size;
        typeInfo->thread_static_fields_size = typeDefinitionSizes->thread_static_fields_size;
        typeInfo->thread_static_fields_offset = -1;
        typeInfo->flags = typeDefinition->flags;
        typeInfo->valuetype = (typeDefinition->bitfield >> (1 - 1)) & 0x1;
        typeInfo->enumtype = (typeDefinition->bitfield >> (2 - 1)) & 0x1;
        typeInfo->is_generic = typeDefinition->genericContainerIndex != -1;
        typeInfo->has_finalize = (typeDefinition->bitfield >> (3 - 1)) & 0x1;
        typeInfo->has_cctor = (typeDefinition->bitfield >> (4 - 1)) & 0x1;
        typeInfo->is_blittable = (typeDefinition->bitfield >> (5 - 1)) & 0x1;
        typeInfo->is_import_or_windows_runtime = (typeDefinition->bitfield >> (6 - 1)) & 0x1;
        typeInfo->packingSize = ConvertPackingSizeEnumToValue(
                static_cast<PackingSize>((typeDefinition->bitfield >> (7 - 1)) & 0xF));
        typeInfo->method_count = typeDefinition->method_count;
        typeInfo->methods = (const MethodInfo **) calloc(typeInfo->method_count,
                                                         sizeof(MethodInfo * ));
        typeInfo->property_count = typeDefinition->property_count;
        typeInfo->field_count = typeDefinition->field_count;
        typeInfo->event_count = typeDefinition->event_count;
        typeInfo->nested_type_count = typeDefinition->nested_type_count;
        typeInfo->vtable_count = typeDefinition->vtable_count;
        typeInfo->interfaces_count = typeDefinition->interfaces_count;
        typeInfo->interface_offsets_count = typeDefinition->interface_offsets_count;
        typeInfo->token = typeDefinition->token;

        auto class_from_il2cpp_type = (class_from_type_t) dlsym(get_il2cpp(),
                                                                "il2cpp_class_from_il2cpp_type");
        if (typeDefinition->parentIndex != kTypeIndexInvalid)
            typeInfo->parent = class_from_il2cpp_type(
                    GetIl2CppTypeFromIndex(typeDefinition->parentIndex));

        if (typeDefinition->declaringTypeIndex != kTypeIndexInvalid)
            typeInfo->declaringType = class_from_il2cpp_type(
                    GetIl2CppTypeFromIndex(typeDefinition->declaringTypeIndex));

        typeInfo->castClass = typeInfo->element_class = typeInfo;
        if (typeInfo->enumtype)
            typeInfo->castClass = typeInfo->element_class = class_from_il2cpp_type(
                    GetIl2CppTypeFromIndex(typeDefinition->elementTypeIndex));

        return typeInfo;
    }


    static uint8_t ConvertPackingSizeEnumToValue(PackingSize packingSize) {
        switch (packingSize) {
            case Zero:
                return 0;
            case One:
                return 1;
            case Two:
                return 2;
            case Four:
                return 4;
            case Eight:
                return 8;
            case Sixteen:
                return 16;
            case ThirtyTwo:
                return 32;
            case SixtyFour:
                return 64;
            case OneHundredTwentyEight:
                return 128;
            default:
                assert(0 && "Invalid packing size!");
                return 0;
        }
    }

    InvokerMethod GetMethodInvoker(const Il2CppImage *image, uint32_t token) {
        uint32_t rid = token & 0x00FFFFFF;
        if (rid == 0)
            return NULL;

        int32_t index = image->codeGenModule->invokerIndices[rid - 1];

        if (index == kMethodIndexInvalid)
            return NULL;
        return Il2CppRegistration->invokerPointers[index];
    }

    const Il2CppMethodDefinition *GetMethodDefinitionFromIndex(MethodIndex index) {
        const Il2CppMethodDefinition *methods = (const Il2CppMethodDefinition *) (
                (const char *) file + header->methodsOffset);
        return methods + index;
    }

    Il2CppMethodPointer GetMethodPointer(const Il2CppImage *image, uint32_t token) {
        uint32_t rid = token & 0x00FFFFFF;
        if (rid == 0)
            return NULL;
        return image->codeGenModule->methodPointers[rid - 1];
    }
};

//https://github.com/4ch12dy/XB1nLib/blob/master/XB1nLib.c#L29
void *MapFile2Mem(const char *file_path) {
    int fd;
    struct stat st;
    size_t len_file;
    void *p;
    if ((fd = open(file_path, O_RDWR | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH)) < 0) return NULL;
    if ((fstat(fd, &st)) < 0) return NULL;
    len_file = st.st_size;
    if ((p = mmap(NULL, len_file, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
        return NULL;
    close(fd);
    return p;
}

void *(*old_regget)(void *codeRegistration, void *metadataRegistration,
                    void *codeGenOptions);

void *
regget(void *codeRegistration, void *metadataRegistration, void *codeGenOptions) {
    s_Il2CppCodeRegistration = codeRegistration;
    s_Il2CppMetadataRegistration = metadataRegistration;
    return old_regget(codeRegistration, metadataRegistration, codeGenOptions);
}

struct LibInfo {
    uintptr_t startAddr;
    uintptr_t endAddr;
    intptr_t size;
    std::string path;
};

LibInfo GetLibInfo(const char *libraryName) {
    LibInfo retMap = {};
    char line[512] = {0};
    FILE *fp = fopen("/proc/self/maps", "rt");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, libraryName)) {
                LibInfo tmpMap;
                char tmpPathname[400] = {0};
                sscanf(line, "%llx-%llx %*s %*ld %*s %*d %s",
                       (long long unsigned *) &tmpMap.startAddr,
                       (long long unsigned *) &tmpMap.endAddr,
                       tmpPathname);
                if (retMap.startAddr == 0)
                    retMap.startAddr = tmpMap.startAddr;
                retMap.endAddr = tmpMap.endAddr;
                retMap.size = retMap.endAddr - retMap.startAddr;
                if (retMap.path.empty())
                    retMap.path = tmpPathname;
            }
        }
        fclose(fp);
    }
    return retMap;
}

std::string readHexStrFromMem(const void *addr, size_t len) {
    char temp[len];
    memset(temp, 0, len);
    const size_t bufferLen = len * 2 + 1;
    char buffer[bufferLen];
    memset(buffer, 0, bufferLen);
    std::string ret;
    if (memcpy(temp, addr, len) == NULL)
        return ret;
    for (int i = 0; i < len; i++) {
        sprintf(&buffer[i * 2], "%02X", (unsigned char) temp[i]);
    }
    ret += buffer;
    return ret;
}

DWORD findPattern(const char *pattern, DWORD address, DWORD len) {
    std::string fixedpar = fixhex(std::string(pattern));
    const char *pat = fixedpar.c_str();
    DWORD match = 0;
    for (DWORD pCur = address; pCur < address + len; pCur++) {
        if (!pat[0]) return match;
        char curmem[3];
        sprintf(curmem, "%02X", *(char *) pCur);
        if ((pat[0] == '\?' && pat[1] == '\?') || (pat[0] == '\?' && pat[1] == curmem[1]) ||
            (pat[0] == curmem[0] && pat[1] == '\?') ||
            (pat[0] == curmem[0] && pat[1] == curmem[1])) {
            if (!match) match = pCur;
            if (!pat[2]) return match;
            pat += 2;
        } else {
            if (match) pCur = match;
            pat = fixedpar.c_str();
            match = 0;
        }
    }
    return 0;
}

const char *readFileTxt(const char *myFile) {
    FILE *file = fopen(myFile, "r");
    if (!file) return "";
    std::vector<char> vec;
    while (!feof(file)) {
        char a;
        fread(&a, 1, 1, file);
        vec.push_back(a);
    }
    fclose(file);
    return std::string(vec.begin(), vec.end()).c_str();
}

struct MetaDataUtils {
    static void *LoadMetadataDat() {
        pid_t pid = getpid();
        char pagacke_path[255] = {0};
        sprintf(pagacke_path, "/proc/%d/cmdline", pid);
        const char *current_pakage_name = readFileTxt(pagacke_path);
        std::string dat_path =
                std::string("/sdcard/Android/data/") + current_pakage_name +
                std::string("/files/il2cpp/Metadata/global-metadata.dat");
        void *dat = MapFile2Mem(dat_path.c_str());
        if (dat) {
            return dat;
        }
        return NULL;
    }

    static const Il2CppGlobalMetadataHeader *LoadMetadataDatHeader() {
        return (const Il2CppGlobalMetadataHeader *) LoadMetadataDat();
    }

    static Metadata *CrateMetadata() {
        auto *data = new Metadata();
        auto *metadata = LoadMetadataDat();
        if (!metadata) return NULL;
        auto *header = (const Il2CppGlobalMetadataHeader *) metadata;
        auto *registration = (const Il2CppMetadataRegistration *) getRegister();
        if (!registration) return NULL;
        data->version = header->version;
        data->file = metadata;
        data->header = header;
        data->registartion = registration;
        data->Il2CppRegistration = (const Il2CppCodeRegistration *) s_Il2CppCodeRegistration;
        data->ImagesCount = header->imagesCount / sizeof(Il2CppImageDefinition);
        data->TypeInfoDefinitionTable = (Il2CppClass **) calloc(
                header->typeDefinitionsCount / sizeof(Il2CppTypeDefinition),
                sizeof(Il2CppClass * ));
        data->TypeInfoTable = (Il2CppClass **) calloc(registration->typesCount,
                                                      sizeof(Il2CppClass * ));
        data->ImageDefinitionTable = (const Il2CppImageDefinition *) ((uint64_t) metadata +
                                                                      header->imagesOffset);
        data->TypeDefinitionTable = (const Il2CppTypeDefinition *) ((uint64_t) metadata +
                                                                    header->typeDefinitionsOffset);
        data->FieldDefinitionTable = (const Il2CppFieldDefinition *) ((uint64_t) metadata +
                                                                      header->fieldsOffset);
        data->MethodDefinitionTable = (const Il2CppMethodDefinition *) ((uint64_t) metadata +
                                                                        header->methodsOffset);
        data->ParameterDefinitionTable = (const Il2CppParameterDefinition *) ((uint64_t) metadata +
                                                                              header->parametersOffset);
        data->ImagesTable = (Il2CppImage *) calloc(
                header->imagesCount / sizeof(Il2CppImageDefinition), sizeof(Il2CppImage));
        return data;
    }

    static const char *getFirstPattern() {
#ifdef __arm__
        return "14 ?? 9F E5 14 ?? 9F E5 14 ?? 9F E5 ?? ?? ?? ?? ?? ?? 8F E0 ?? ?? ?? ?? ?? ?? ?? E?";
#else
        return "NONE";
#endif
    }

    static void *getRegister() {
        LibInfo libInfo = GetLibInfo("libil2cpp.so");
        if (MetadataRegistrationOffset != 0x0) {
            s_Il2CppMetadataRegistration = (void *) (libInfo.startAddr + MetadataRegistrationOffset);
            s_Il2CppCodeRegistration = (void *) (libInfo.startAddr + Il2CppRegistrationOffset);
        } else {
            if (s_Il2CppMetadataRegistration) return s_Il2CppMetadataRegistration;

            DWORD Method2Call_Offset = findPattern(getFirstPattern(), libInfo.startAddr,
                                                        libInfo.size, 0) - libInfo.startAddr;
            DWORD BOffset = Method2Call_Offset + 24;
            DWORD fB = getOffsetFromB_Hex(
                    readHexStrFromMem((const void *) (BOffset + libInfo.startAddr), 4), BOffset);
            DWORD Method2Hook_offset = getOffsetFromB_Hex(
                    readHexStrFromMem((const void *) (fB + libInfo.startAddr), 4), fB);
            if (Method2Hook_offset == 0 || BOffset == 0 || fB == 0) return 0;
            HOOK((libInfo.startAddr + Method2Hook_offset), regget, old_regget);
            void *(*Il2CppCodegenRegistration)(void *);
            InitFunc(Il2CppCodegenRegistration, (Method2Call_Offset + libInfo.startAddr));
            Il2CppCodegenRegistration(0);
        }
        return s_Il2CppMetadataRegistration;
    }
};

Il2CppReflectionType *GetMonoTypeFromIl2CppClass(Il2CppClass *klass) {
    auto type_get_object = (type_get_object_t) dlsym(get_il2cpp(), "il2cpp_type_get_object");
    return (Il2CppReflectionType *) type_get_object(&klass->byval_arg);
}

DWORD abs(DWORD val) {
    if (val < 0)
        return -val;
    return val;
}
