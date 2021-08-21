#pragma once
using namespace std;
#include "macros.h"
typedef unsigned long DWORD;
#if defined(__ARM_ARCH_7A__) // armv7

#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>

DWORD MetadataRegistrationOffset = 0x0;
DWORD Il2CppRegistrationOffset = 0x0;

#elif defined(__i386__) //x86

#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>

DWORD MetadataRegistrationOffset = 0x0;
DWORD Il2CppRegistrationOffset = 0x0;

#elif defined(__aarch64__) //arm64-v8a

#include <And64InlineHook/And64InlineHook.hpp>

DWORD MetadataRegistrationOffset = 0x0;
DWORD Il2CppRegistrationOffset = 0x0;

#endif

auto HOOK = [](auto ptr, auto newMethod, auto&& oldBytes) {
    if (ptr != 0){
#if defined(__aarch64__)
        A64HookFunction((void *)ptr, (void *) newMethod, (void **) &oldBytes);
#else
        MSHookFunction((void *)ptr, (void *) newMethod, (void **) &oldBytes);
#endif
    }
};
#define InitResolveFunc(x, y) *(void **)(&x) = get_Method(y)
#define InitFunc(x, y) if (y != 0) *(void **)(&x) = (void *)(y)
#define FieldBN(type, inst, nameSpacec, clazzz, _new, fieldName, key) (LoadClass(OBFUSCATES_KEY_BNM(nameSpacec, key), OBFUSCATES_KEY_BNM(clazzz, key), _new).GetFieldByName<type>(OBFUSCATES_KEY_BNM(fieldName, key), inst))
#define FieldBNC(type, inst, nameSpacec, clazzz, _new, fieldName) (LoadClass(nameSpacec, clazzz, _new).GetFieldByName<type>(fieldName, inst))

void *s_Il2CppMetadataRegistration = 0;
void *s_Il2CppCodeRegistration = 0;

void *get_il2cpp() {
    void *mod = 0;
    while (!mod) {
        mod = dlopen(OBFUSCATE_BNM("libil2cpp.so"), RTLD_LAZY);
    }
    return mod;
}
bool IsNativeObjectAlive(void *o){
    return o != NULL && *(intptr_t *)((uint64_t)o + 0x8) != 0;
}
#define DO_API(r, n, p) auto n = (r (*) p)dlsym(get_il2cpp(), OBFUSCATE_BNM(#n))

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
    if (!setjmp (jump))
        c = *(char *) (x);
    else
        illegal = true;
    signal(SIGSEGV, SIG_DFL);
    return illegal;
}

const char *readFileTxt(const char *myFile) {
    FILE *file = fopen(myFile, OBFUSCATE_BNM("r"));
    if (!file) return OBFUSCATE_BNM("");
    std::vector<char> vec;
    while (!feof(file)) {
        char a;
        fread(&a, 1, 1, file);
        vec.push_back(a);
    }
    fclose(file);
    return std::string(vec.begin(), vec.end()).c_str();
}

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


template<typename T>
T CheckObj(T obj) {
    if (isNOT_Allocated(obj) || obj == NULL)
        return NULL;
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
    if (out.find(std::string(OBFUSCATE_BNM("0x"))) != -1) {
        tmp = out.find(std::string(OBFUSCATE_BNM("0x")));
        out.replace(tmp, 2, std::string(OBFUSCATE_BNM("")));
    }
    for (int i = out.length() - 1; i >= 0; --i) {
        if (out[i] == ' ')
            out.erase(i, 1);
    }
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
        hex = std::string(OBFUSCATE_BNM("00")) + hex.substr(2);
    } else {
        hex = std::string(OBFUSCATE_BNM("FF")) + hex.substr(2);
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
    float version;
    int32_t ImagesCount;
    const Il2CppImageDefinition *ImageDefinitionTable;
    const Il2CppTypeDefinition *TypeDefinitionTable;
    const Il2CppFieldDefinition *FieldDefinitionTable;
    const Il2CppMethodDefinition *MethodDefinitionTable;
    const Il2CppParameterDefinition *ParameterDefinitionTable;
    Il2CppClass **TypeInfoDefinitionTable;

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
#if IL2CPP_VERSION < 242
        newMethod->methodPointer = GetMethodPointerFromIndex(methodDefinition->methodIndex);
        newMethod->invoker_method = GetMethodInvokerFromIndex(methodDefinition->invokerIndex);
#else
        newMethod->methodPointer = GetMethodPointer(typeInfo->image, methodDefinition->token);
        newMethod->invoker_method = GetMethodInvoker(typeInfo->image,
                                                     methodDefinition->token);
#endif
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

    const TypeDefinitionIndex GetIndexForTypeDefinition(const Il2CppClass *typeDefinition) {
        const Il2CppTypeDefinition *typeDefinitions = (const Il2CppTypeDefinition *) (
                (const char *) file + header->typeDefinitionsOffset);
        ptrdiff_t index = typeDefinition->typeDefinition - typeDefinitions;
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
        const Il2CppTypeDefinition *typeDefinition = (const Il2CppTypeDefinition *) (
                (const char *) file + header->typeDefinitionsOffset) + index;
        //! This is more safer and this should work on all versions of unity
        DO_API(Il2CppClass*, il2cpp_class_from_il2cpp_type, (const Il2CppType * type));
        return il2cpp_class_from_il2cpp_type(
                GetIl2CppTypeFromIndex(typeDefinition->byvalTypeIndex));
    }

    const Il2CppParameterDefinition *GetParameterDefinitionFromIndex(ParameterIndex index) {
        const Il2CppParameterDefinition *parameters = (const Il2CppParameterDefinition *) (
                (const char *) file + header->parametersOffset);
        return parameters + index;
    }

#if IL2CPP_VERSION < 242
    InvokerMethod GetMethodInvokerFromIndex(MethodIndex index)
    {
        if (index == kMethodIndexInvalid)
            return NULL;
        return Il2CppRegistration->invokerPointers[index];
    }
    Il2CppMethodPointer GetMethodPointerFromIndex(MethodIndex index)
    {
        if (index == kMethodIndexInvalid)
            return NULL;
        return Il2CppRegistration->methodPointers[index];
    }
#else

    InvokerMethod GetMethodInvoker(const Il2CppImage *image, uint32_t token) {
        uint32_t rid = token & 0x00FFFFFF;
        if (rid == 0)
            return NULL;

        int32_t index = image->codeGenModule->invokerIndices[rid - 1];

        if (index == kMethodIndexInvalid)
            return NULL;
        return Il2CppRegistration->invokerPointers[index];
    }

    Il2CppMethodPointer GetMethodPointer(const Il2CppImage *image, uint32_t token) {
        uint32_t rid = token & 0x00FFFFFF;
        if (rid == 0)
            return NULL;
        return image->codeGenModule->methodPointers[rid - 1];
    }

#endif

    const Il2CppMethodDefinition *GetMethodDefinitionFromIndex(MethodIndex index) {
        const Il2CppMethodDefinition *methods = (const Il2CppMethodDefinition *) (
                (const char *) file + header->methodsOffset);
        return methods + index;
    }

};


// For finder
bool isfinder;

void *(*old_regget)(void *codeRegistration, void *metadataRegistration,
                    void *codeGenOptions);

// For finder
void *
regget(void *codeRegistration, void *metadataRegistration, void *codeGenOptions) {
    if (isfinder) {
        s_Il2CppCodeRegistration = codeRegistration;
        s_Il2CppMetadataRegistration = metadataRegistration;
        return 0;
    } else {
        return old_regget(codeRegistration, metadataRegistration, codeGenOptions);
    }
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
    FILE *fp = fopen(OBFUSCATE_BNM("/proc/self/maps"), OBFUSCATE_BNM("rt"));
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, libraryName)) {
                LibInfo tmpMap;
                char tmpPathname[400] = {0};
                sscanf(line, OBFUSCATE_BNM("%llx-%llx %*s %*ld %*s %*d %s"),
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
        sprintf(&buffer[i * 2], OBFUSCATE_BNM("%02X"), (unsigned char) temp[i]);
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
        sprintf(curmem, OBFUSCATE_BNM("%02X"), *(char *) pCur);
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


struct MetaDataUtils {
    static void *LoadMetadataDat() {
        pid_t pid = getpid();
        char pagacke_path[255] = {0};
        sprintf(pagacke_path, OBFUSCATE_BNM("/proc/%d/cmdline"), pid);
        const char *current_pakage_name = readFileTxt(pagacke_path);
        std::string dat_path =
                std::string(OBFUSCATE_BNM("/sdcard/Android/data/")) + current_pakage_name +
                std::string(OBFUSCATE_BNM("/files/il2cpp/Metadata/global-metadata.dat"));
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
        if (header->version == 24)
            if (header->stringLiteralOffset == 264)
                data->version = 24.2f;
            else
                for (int i = 0; i < header->imagesCount; i++)
                    if (((const Il2CppTypeDefinition *) ((uint64_t) metadata +
                                                         header->typeDefinitionsOffset) +
                         i)->token != 1)
                        data->version = 24.1f;
        data->file = metadata;
        data->header = header;
        data->registartion = registration;
        data->Il2CppRegistration = (const Il2CppCodeRegistration *) s_Il2CppCodeRegistration;
        data->ImagesCount = header->imagesCount / sizeof(Il2CppImageDefinition);
        data->TypeInfoDefinitionTable = (Il2CppClass **) calloc(
                header->typeDefinitionsCount / sizeof(Il2CppTypeDefinition), sizeof(Il2CppClass *));
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
        return data;
    }

    static const char *getFirstPattern() {
#if defined(__ARM_ARCH_7A__)
        return OBFUSCATE_BNM(
                "14 ?? 9F E5 14 ?? 9F E5 14 ?? 9F E5 ?? ?? ?? ?? ?? ?? 8F E0 ?? ?? ?? ?? ?? ?? ?? E?");
#else
        return OBFUSCATE_BNM("NONE");
#endif
    }

    static void *getRegister() {
        LibInfo libInfo = GetLibInfo(OBFUSCATE_BNM("libil2cpp.so"));
        if (s_Il2CppMetadataRegistration) return s_Il2CppMetadataRegistration;
        if (MetadataRegistrationOffset != 0x0) {
            s_Il2CppMetadataRegistration = (void *) (libInfo.startAddr +
                                                     MetadataRegistrationOffset);
            s_Il2CppCodeRegistration = (void *) (libInfo.startAddr + Il2CppRegistrationOffset);
        } else {
            DWORD Method2Call_Offset = findPattern(getFirstPattern(), libInfo.startAddr,
                                                   libInfo.size) - libInfo.startAddr;
            DWORD BOffset = Method2Call_Offset + 24;
            DWORD fB = getOffsetFromB_Hex(
                    readHexStrFromMem((const void *) (BOffset + libInfo.startAddr), 4), BOffset);
            DWORD Method2Hook_offset = getOffsetFromB_Hex(
                    readHexStrFromMem((const void *) (fB + libInfo.startAddr), 4), fB);
            if (Method2Hook_offset == 0 || BOffset == 0 || fB == 0) return 0;
            HOOK((libInfo.startAddr + Method2Hook_offset), regget, old_regget);
            void *(*Il2CppCodegenRegistration)(void *);
            InitFunc(Il2CppCodegenRegistration, (Method2Call_Offset + libInfo.startAddr));
            isfinder = true;
            Il2CppCodegenRegistration(0);
            isfinder = false;
        }
        return s_Il2CppMetadataRegistration;
    }
};

DWORD abs(DWORD val) {
    if (val < 0)
        return -val;
    return val;
}

template<typename T>
static T UnBoxObject(T obj) {
    void *val = (void *) (((char *) obj) + sizeof(Il2CppObject));
    return *(T *) val;
}

