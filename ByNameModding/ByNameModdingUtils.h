#pragma once
typedef unsigned long DWORD;

struct TypeFinder {
    bool byNameOnly;
    const char *name;
    const char *namespaze;
    bool withMethodName;
    const char *methodName;
    Il2CppType* ToIl2CppType();
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
bool new_Class_Init(Il2CppClass *klass);
static Il2CppAssembly* BNM_Assembly = nullptr;
static Il2CppImage * BNM_Image = nullptr;
static std::vector<Il2CppClass *> BNM_classes;
#if UNITY_VER <= 174
Il2CppImage*(*old_GetImageFromIndex)(ImageIndex index);
Il2CppImage* new_GetImageFromIndex(ImageIndex index){
    if (index == -0x424e4d) {
        return BNM_Image;
    }else{
        return old_GetImageFromIndex(index);
    }
}
#endif
void *getExternMethod(string str);
#define InitResolveFunc(x, y) *reinterpret_cast<void **>(&x) = getExternMethod(y)
auto InitFunc = [](auto&& method, auto ptr) {
    if (ptr != 0){
        *(void **)(&method) = (void *)(ptr);
    }
};
auto IsNativeObjectAlive = [](auto o) {
    return o != NULL && *(intptr_t *)((uint64_t)o + 0x8) != 0;
};

#define FieldBN(type, inst, nameSpacec, clazzz, fieldName) (LoadClass(OBFUSCATES_BNM(nameSpacec), OBFUSCATES_BNM(clazzz)).GetFieldByName<type>(OBFUSCATES_BNM(fieldName), inst))
#define FieldBNC(type, inst, nameSpacec, clazzz, fieldName) (LoadClass(nameSpacec, clazzz).GetFieldByName<type>(fieldName, inst))

char* str2char(std::string str) {
    int size = str.length();
    char* writable = new char[size];
    std::copy(str.begin(), str.end(), writable);
    writable[size] = '\0';
    return writable;
}
bool BNMil2cppLoaded(){
    void *mod = BNMdlopen(OBFUSCATE_BNM("libil2cpp.so"), RTLD_LAZY);
    bool ret = mod != 0;
    if (ret)
        BNMdlclose(mod);
    return ret;
}
void *get_il2cpp() {
    void *mod;
    do {
        mod = BNMdlopen(OBFUSCATE_BNM("libil2cpp.so"), RTLD_LAZY);
    } while (!mod);
    return mod;
}

#define DO_API(r, n, p) auto n = (r (*) p)BNMdlsym(get_il2cpp(), OBFUSCATE_BNM(#n))

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
DWORD FindNext_B_BL_offset(DWORD start, int index = 1) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
    int offset = 0;
    std::string curHex = readHexStrFromMem((const void*)start, 4);
    DWORD outOffset = 0;
    bool out = true;
    while (!(out = Decode_Branch_or_Call_Hex(curHex, start + offset, &outOffset)) || index != 1) {
        offset += 4;
        curHex = readHexStrFromMem((const void*)(start + offset), 4);
        if (out)
            index--;
    }
    return outOffset;
#elif defined(__i386__)
    int offset = 0;
    std::string curHex = readHexStrFromMem((const void*)start, 1);
    DWORD outOffset = 0;
    bool out = true;
    while (!(out = Is_x86_call_hex(curHex)) || index != 1) {
        offset += 1;
        curHex = readHexStrFromMem((const void*)(start + offset), 1);
        if (out)
            index--;
    }
    Decode_Branch_or_Call_Hex(readHexStrFromMem((const void*)(start + offset), 5), start + offset, &outOffset);
    return outOffset;
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
auto isAllocated = [](auto x) -> bool {
    static jmp_buf jump;
    static sighandler_t handler = [](int) { longjmp(jump, 1); };
    [[maybe_unused]] volatile char c;
    volatile bool ok = true;

    volatile sighandler_t old_handler = signal(SIGSEGV, handler);

    if (!setjmp (jump))
        c = *(char *) (x);
    else
        ok = false;
    signal(SIGSEGV, old_handler);
    return ok;
};
template<typename T>
T CheckObj(T obj) {
    if (!isAllocated(obj) || obj == NULL)
        return NULL;
    return obj;
}
bool loaded = false;

void InitIl2cppMethods(){
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
    int count = 1;
#elif defined(__i386__) || defined(__x86_64__)
    int count = 2;
#endif
    usleep(1000);
    //! il2cpp::vm::Class::FromIl2CppType HOOK
    if (!old_Class_FromIl2CppType){
        DWORD from_type_adr = (DWORD)BNMdlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_class_from_type"));
        Dl_info info;
        BNMdladdr((void*)from_type_adr, &info);
        DWORD startAddr = (DWORD)info.dli_fbase;
        from_type_adr = FindNext_B_BL_offset(from_type_adr, count);
        HOOK(from_type_adr, new_Class_FromIl2CppType, old_Class_FromIl2CppType);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::FromIl2CppType in lib: 0x%x"), (DWORD)from_type_adr - startAddr);
    }
    usleep(1000);
    //! il2cpp::vm::Class::FromName HOOK
    if (!old_Class_FromName){
        DWORD from_name_adr = (DWORD)BNMdlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_class_from_name"));
        Dl_info info;
        BNMdladdr((void*)from_name_adr, &info);
        DWORD startAddr = (DWORD)info.dli_fbase;
        from_name_adr = FindNext_B_BL_offset(FindNext_B_BL_offset(from_name_adr, count), count);
        HOOK(from_name_adr, new_Class_FromName, old_Class_FromName);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::FromName in lib: 0x%x"), (DWORD)from_name_adr - startAddr);
    }
    usleep(1000);
    //! il2cpp::vm::Class::Init HOOK
    if (!Class$$Init_t){
        DWORD Init_adr = (DWORD)BNMdlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_array_new_specific"));
        Dl_info info;
        BNMdladdr((void*)Init_adr, &info);
        DWORD startAddr = (DWORD)info.dli_fbase;
        Class$$Init_t = (bool (*)(Il2CppClass *))(FindNext_B_BL_offset(FindNext_B_BL_offset(Init_adr, count), count));
        HOOK(Class$$Init_t, new_Class_Init, old_Class_Init);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::Init in lib: 0x%x"), (DWORD)Class$$Init_t - startAddr);
    }
    usleep(1000);
    //! il2cpp::vm::Image::GetTypes HOOK
    if (!Image$$GetTypes_t){
        DO_API(const Il2CppImage*, il2cpp_get_corlib, ());
        DO_API(Il2CppClass*, il2cpp_class_from_name, (const Il2CppImage * image, const char* namespaze, const char *name));
        DO_API(const MethodInfo*, il2cpp_class_get_method_from_name, (Il2CppClass * klass, const char* name, int argsCount));
        auto corlib = il2cpp_get_corlib();
        Dl_info info;
        BNMdladdr((void*)il2cpp_get_corlib, &info);
        DWORD startAddr = (DWORD)info.dli_fbase;
        auto assemblyClass = il2cpp_class_from_name(corlib, OBFUSCATE_BNM("System.Reflection"), OBFUSCATE_BNM("Assembly"));
        DWORD GetTypesAdr = 0;
		for (int i = 0; i < assemblyClass->method_count; i++) {
            const MethodInfo *method = assemblyClass->methods[i];
            if (method && OBFUSCATES_BNM("GetTypes") == method->name && method->parameters_count == 1) {
                GetTypesAdr = (DWORD) method->methodPointer;
                break;
            }
        }
		const int sCount
#if UNITY_VER > 174
        = count + 1;
#else
        = count + 2;
#endif
        Image$$GetTypes_t = (void (*)(const Il2CppImage *, bool, TypeVector *))FindNext_B_BL_offset(FindNext_B_BL_offset(FindNext_B_BL_offset(GetTypesAdr, count), sCount), count);
        HOOK(Image$$GetTypes_t, new_Image_GetTypes, old_Image_GetTypes);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Image::GetTypes in lib: 0x%x"), (DWORD)Image$$GetTypes_t - startAddr);
    }
    usleep(1000);
#if UNITY_VER <= 174
    //! il2cpp::vm::MetadataCache::GetImageFromIndex HOOK
    if (!old_GetImageFromIndex){
        DWORD adr = (DWORD)BNMdlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_assembly_get_image"));
        Dl_info info;
        BNMdladdr((void*)adr, &info);
        DWORD startAddr = (DWORD)info.dli_fbase;
        DWORD AssemblyGetImageOffset = FindNext_B_BL_offset(adr, count);
        DWORD GetImageFromIndexOffset = FindNext_B_BL_offset(AssemblyGetImageOffset, count);
        HOOK(GetImageFromIndexOffset, new_GetImageFromIndex, old_GetImageFromIndex);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::GetImage in lib: 0x%x"), GetImageFromIndexOffset - startAddr);
    }
    usleep(1000);
#endif

    //! il2cpp::vm::Assembly::GetAllAssemblies GET
    if (!Assembly$$GetAllAssemblies_t){
        DO_API(const Il2CppImage*, il2cpp_get_corlib, ());
        DO_API(Il2CppClass*, il2cpp_class_from_name, (const Il2CppImage * image, const char* namespaze, const char *name));
        DO_API(const MethodInfo*, il2cpp_class_get_method_from_name, (Il2CppClass * klass, const char* name, int argsCount));
        auto corlib = il2cpp_get_corlib();
        Dl_info info;
        BNMdladdr((void*)il2cpp_get_corlib, &info);
        DWORD startAddr = (DWORD)info.dli_fbase;
        auto assemblyClass = il2cpp_class_from_name(corlib, OBFUSCATE_BNM("System"), OBFUSCATE_BNM("AppDomain"));
        const MethodInfo *getAssemb = 0;
        for (int i = 0; i < assemblyClass->method_count; i++) {
            const MethodInfo *method = assemblyClass->methods[i];
            if (method && OBFUSCATES_BNM("GetAssemblies") == method->name && method->parameters_count == 1) {
                getAssemb = method;
                break;
            }
        }
		if (getAssemb){
            DWORD GetTypesAdr = FindNext_B_BL_offset((DWORD) getAssemb->methodPointer, count);
            Assembly$$GetAllAssemblies_t = (AssemblyVector *(*)(void))(FindNext_B_BL_offset(GetTypesAdr, count+1));
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::GetAllAssemblies by AppDomain in lib: 0x%x"), (DWORD)Assembly$$GetAllAssemblies_t - startAddr);
        } else {
            DWORD adr = (DWORD)BNMdlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_domain_get_assemblies"));
            Assembly$$GetAllAssemblies_t = (AssemblyVector *(*)(void))(FindNext_B_BL_offset(adr, count));
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::GetAllAssemblies by domain in lib: 0x%x"), (DWORD)Assembly$$GetAllAssemblies_t - startAddr);
        }
    }
    usleep(1000);
}

