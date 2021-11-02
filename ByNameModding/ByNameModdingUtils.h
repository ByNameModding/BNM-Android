#pragma once
typedef unsigned long DWORD;

struct TypeFinder {
    bool byNameOnly;
    const char *name;
    const char *namespaze;
    const Il2CppType* ToIl2CppType();
};
template<typename T>
constexpr TypeFinder GetType();
void InitIl2cppMethods();
typedef std::vector<const Il2CppAssembly*> AssemblyVector;
typedef std::vector<const Il2CppClass*> TypeVector;

AssemblyVector *(*Assembly$$GetAllAssemblies_t)();

AssemblyVector *Assembly$$GetAllAssemblies(){
    if (!Assembly$$GetAllAssemblies_t)
        InitIl2cppMethods();
    return Assembly$$GetAllAssemblies_t();
}

bool (*Class$$Init_t)(Il2CppClass *);

bool Class$$Init(Il2CppClass *cls){
    if (!Class$$Init_t)
        InitIl2cppMethods();
    return Class$$Init_t(cls);
}
void (*old_Image_GetTypes)(...);
void new_Image_GetTypes(const Il2CppImage* image, bool exportedOnly_UNUSED_IN_IL2CPP_SRC, TypeVector* target);

void (*Image$$GetTypes_t)(const Il2CppImage* image, bool exportedOnly_UNUSED_IN_IL2CPP_SRC, TypeVector* target);
void Image$$GetTypes(const Il2CppImage* image, bool exportedOnly_UNUSED_IN_IL2CPP_SRC, TypeVector* target){
    if (!Image$$GetTypes_t)
        InitIl2cppMethods();
    Image$$GetTypes_t(image, exportedOnly_UNUSED_IN_IL2CPP_SRC, target);
}
Il2CppClass* (*old_Class_FromIl2CppType)(const Il2CppType* type);
Il2CppClass* new_Class_FromIl2CppType(const Il2CppType* type);

Il2CppClass* (*old_Class_FromName)(const Il2CppImage* image, const char* namespaze, const char *name);
Il2CppClass* new_Class_FromName(const Il2CppImage* image, const char* namespaze, const char *name);

bool (*old_Class_Init)(Il2CppClass *klass);
bool new_Class_Init(Il2CppClass *klass) ;

#define InitResolveFunc(x, y) *reinterpret_cast<void **>(&x) = get_Method(y)
#define InitFunc(x, y) if (y != 0) *(void **)(&x) = (void *)(y)
#define FieldBN(type, inst, nameSpacec, clazzz, fieldName) (LoadClass(OBFUSCATES_BNM(nameSpacec), OBFUSCATES_BNM(clazzz)).GetFieldByName<type>(OBFUSCATES_BNM(fieldName), inst))
#define FieldBNC(type, inst, nameSpacec, clazzz, fieldName) (LoadClass(nameSpacec, clazzz).GetFieldByName<type>(fieldName, inst))


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

void segv([[maybe_unused]] int sig) {
    longjmp(jump, 1);
}

template<typename T>
bool isNOT_Allocated(T x) {
    [[maybe_unused]] volatile char c;
    bool illegal = false;

    volatile auto old_sig = signal(SIGSEGV, segv);

    if (!setjmp (jump))
        c = *(char *) (x);
    else
        illegal = true;
    signal(SIGSEGV, old_sig);
    return illegal;
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

DWORD HexStr2DWORD(std::string hex) {
    return strtoull(hex.c_str(), NULL, 16);
}

bool Is_B_BL_Hex_arm64(std::string hex) {
    DWORD hexw = HexStr2DWORD(revhexstr(fixhex(hex)));
    return (hexw & 0xFC000000) == 0x14000000 || (hexw & 0xFC000000) == 0x94000000;
}

bool Is_B_BL_Hex(std::string hex) {
    DWORD hexw = HexStr2DWORD(revhexstr(fixhex(hex)));
    return ((hexw & 0xFC000000) == 0x14000000 || (hexw & 0xFC000000) == 0x94000000) || (hexw & 0x0A000000) == 0x0A000000;
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

bool Is_x86_call_hex(std::string hex) {
    return hex.substr(0, 2) == OBFUSCATES_BNM("E8");
}
/*
 * Branch decoding based on
 * https://github.com/aquynh/capstone/
*/
bool Decode_Branch_or_Call_Hex(std::string hex, DWORD offset, DWORD *outOffset) {
    bool arm = false;
    if (!(arm = Is_B_BL_Hex(hex)) && !Is_x86_call_hex(hex)) return false;
    if (arm) {
        DWORD insn = HexStr2DWORD(revhexstr(fixhex(hex)));
        DWORD imm = ((insn & (((uint32_t)1 << 24) - 1) << 0) >> 0) << 2;
        DWORD SignExtendedImm = (int32_t)(imm << (32 - 26)) >> (32 - 26);
        *outOffset = SignExtendedImm + offset + (Is_B_BL_Hex_arm64(hex) ? 0 : 8);
    } else {
        *outOffset = offset + HexStr2DWORD(revhexstr(fixhex(hex)).substr(0, 8)) + 5;
    }
    return true;
}
DWORD FindNext_B_BL_offset(DWORD start, DWORD libstart, int index = 1) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
    int offset = 0;
    std::string curHex = readHexStrFromMem((const void*)start, 4);
    DWORD outOffset = 0;
    DWORD start_inlib = start - libstart;
    bool out = true;
    while (!(out = Decode_Branch_or_Call_Hex(curHex, start_inlib, &outOffset)) || index != 1) {
        offset += 4;
        curHex = readHexStrFromMem((const void*)(start + offset), 4);
        start_inlib += 4;
        if (out)
            index--;
    }
    return outOffset + libstart;
#elif defined(__i386__)
    int offset = 0;
    std::string curHex = readHexStrFromMem((const void*)start, 1);
    DWORD outOffset = 0;
    DWORD start_inlib = start - libstart;
    bool out = true;
    while (!(out = Is_x86_call_hex(curHex)) || index != 1) {
        offset += 1;
        curHex = readHexStrFromMem((const void*)(start + offset), 1);
        start_inlib += 1;
        if (out)
            index--;
    }
    Decode_Branch_or_Call_Hex(readHexStrFromMem((const void*)(start + offset), 5), start_inlib, &outOffset);
    return outOffset + libstart;
#endif
}

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

static Il2CppThread *CurThread;
void BNM_Attach(){
    if (CurThread) return;
    DO_API(Il2CppDomain*, il2cpp_domain_get, ());
    DO_API(Il2CppThread*, il2cpp_thread_attach, (Il2CppDomain * domain));
    CurThread = il2cpp_thread_attach(il2cpp_domain_get());
}
void BNM_DeAttach(){
    if (!CurThread) return;
    DO_API(Il2CppDomain*, il2cpp_domain_get, ());
    DO_API(void, il2cpp_thread_detach, (Il2CppThread * thread));
    il2cpp_thread_detach(CurThread);
    CurThread = nullptr;
}
void InitIl2cppMethods(){
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
    int count = 1;
#elif defined(__i386__)
    int count = 2;
#endif
    LibInfo libInfo = GetLibInfo(OBFUSCATE_BNM("libil2cpp.so"));

    //! il2cpp::vm::Class::FromIl2CppType HOOK
    if (!old_Class_FromIl2CppType){
        DWORD from_type_adr = (DWORD)dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_class_from_type"));
        from_type_adr = FindNext_B_BL_offset(from_type_adr, libInfo.startAddr, count);
        HOOK(from_type_adr, new_Class_FromIl2CppType, old_Class_FromIl2CppType);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::FromIl2CppType in lib: 0x%x"), (DWORD)from_type_adr - libInfo.startAddr);
    }

    //! il2cpp::vm::Class::FromName HOOK
    if (!old_Class_FromName){
        DWORD from_name_adr = (DWORD)dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_class_from_name"));
        from_name_adr = FindNext_B_BL_offset(FindNext_B_BL_offset(from_name_adr, libInfo.startAddr, count), libInfo.startAddr, count);
        HOOK(from_name_adr, new_Class_FromName, old_Class_FromName);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::FromName in lib: 0x%x"), (DWORD)from_name_adr - libInfo.startAddr);
    }

    //! il2cpp::vm::Class::Init HOOK
    if (!Class$$Init_t){
        DWORD Init_adr = (DWORD)dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_array_new_specific"));
        Class$$Init_t = (bool (*)(Il2CppClass *))(FindNext_B_BL_offset(FindNext_B_BL_offset(Init_adr, libInfo.startAddr, count), libInfo.startAddr, count));
        HOOK(Class$$Init_t, new_Class_Init, old_Class_Init);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::Init in lib: 0x%x"), (DWORD)Class$$Init_t - libInfo.startAddr);
    }

    //! il2cpp::vm::Image::GetTypes HOOK
    if (!Image$$GetTypes_t){
        DO_API(const Il2CppImage*, il2cpp_get_corlib, ());
        DO_API(Il2CppClass*, il2cpp_class_from_name, (const Il2CppImage * image, const char* namespaze, const char *name));
        DO_API(const MethodInfo*, il2cpp_class_get_method_from_name, (Il2CppClass * klass, const char* name, int argsCount));
        auto corlib = il2cpp_get_corlib();
        auto assemblyClass = il2cpp_class_from_name(corlib, OBFUSCATE_BNM("System.Reflection"), OBFUSCATE_BNM("Assembly"));
        DWORD GetTypesAdr = (DWORD) il2cpp_class_get_method_from_name(assemblyClass, OBFUSCATE_BNM("GetTypes"), 1)->methodPointer;
        Image$$GetTypes_t = (void (*)(const Il2CppImage *, bool, TypeVector *))FindNext_B_BL_offset(FindNext_B_BL_offset(FindNext_B_BL_offset(GetTypesAdr, libInfo.startAddr, count), libInfo.startAddr, count+1), libInfo.startAddr, count);
        HOOK(Image$$GetTypes_t, new_Image_GetTypes, old_Image_GetTypes);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Image::GetTypes in lib: 0x%x"), (DWORD)Image$$GetTypes - libInfo.startAddr);
    }

    //! il2cpp::vm::Assembly::GetAllAssemblies GET
    if (!Assembly$$GetAllAssemblies_t){
        DWORD adr = (DWORD)dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_domain_get_assemblies"));
        Assembly$$GetAllAssemblies_t = (AssemblyVector *(*)(void))(FindNext_B_BL_offset(adr, libInfo.startAddr, 1));
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::GetAllAssemblies in lib: 0x%x"), (DWORD)Assembly$$GetAllAssemblies_t - libInfo.startAddr);
    }
}
