#pragma once
typedef unsigned long DWORD;
class LoadClass;
struct TypeFinder {
    bool byNameOnly;
    const char *name;
    const char *namespaze;
    bool withMethodName;
    const char *methodName;
    LoadClass ToLC();
    Il2CppType* ToIl2CppType();
    Il2CppClass* ToIl2CppClass();
    operator Il2CppType*(){
        return ToIl2CppType();
    }
    operator Il2CppClass*(){
        return ToIl2CppClass();
    }
    operator LoadClass();
};

template<typename T>
constexpr TypeFinder GetType();
void InitIl2cppMethods();
void InitNewClasses();
typedef std::vector<Il2CppAssembly*> AssemblyVector;
typedef std::vector<Il2CppClass*> TypeVector;

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
static std::vector<Il2CppClass *> BNM_classes;
static std::map<DWORD, std::vector<Il2CppClass *> *> BNM_classes_map;
void *get_il2cpp();
#define DO_API(r, n, p) auto n = (r (*) p)dlsym(get_il2cpp(), OBFUSCATE_BNM(#n))

// Need due Image and Assembly in 2017.x- has index instead of pointer to Image and Assembly
#if UNITY_VER <= 174
Il2CppImage*(*old_GetImageFromIndex)(ImageIndex index);
Il2CppImage* new_GetImageFromIndex(ImageIndex index){
    if (index < 0){
        for (auto [img, clases] : BNM_classes_map){
            if (((Il2CppImage*)img)->assemblyIndex == index)
                return (Il2CppImage *)img;
        }
    } else {
        return old_GetImageFromIndex(index);
    }
    return 0;
}
// All requests redirected to BNM and they are processed by it
Il2CppAssembly* new_Assembly_Load(const char *name){
    DO_API(Il2CppImage*, il2cpp_assembly_get_image, (const Il2CppAssembly * assembly));
    for (auto asmb : *Assembly$$GetAllAssemblies()){
        auto img = il2cpp_assembly_get_image(asmb);
        if (img->name == std::string(name) || img->nameNoExt == std::string(name)){
            return asmb;
        }
    }
    return 0;
}
#endif
void *getExternMethod(string str);
#define InitResolveFunc(x, y) *reinterpret_cast<void **>(&x) = getExternMethod(y)
auto InitFunc = [](auto&& method, auto ptr) {
    if (ptr != 0){
        *(void **)(&method) = (void *)(ptr);
    }
};
auto BNM_IsUnityObjectAlive = [](auto o) {
    return o != NULL && *(intptr_t *)((uint64_t)o + 0x8) != 0;
};
auto BNM_IsSameUnityObject = [](auto o1, auto o2) {
    return *(intptr_t *)((uint64_t)o1 + 0x8) == *(intptr_t *)((uint64_t)o2 + 0x8);
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

static bool BNM_LibLoaded = false;
static void *BNM_dlLib;

bool BNMil2cppLoaded(){
    return BNM_LibLoaded;
}
void *get_il2cpp() {
    return BNM_dlLib;
}

void (*old_BNM_il2cpp_init)(const char*);
void BNM_il2cpp_init(const char* domain_name){
    old_BNM_il2cpp_init(domain_name);
    InitIl2cppMethods();
    InitNewClasses();
    BNM_LibLoaded = true;
}

DWORD FindNext_B_BL_offset(DWORD start, int index);
void *(*old_BNM_dlopen)(const char *, int);
__attribute__((constructor))
void PrepareBNM() {
    std::thread([](){
        do {
            BNM_dlLib = dlopen(OBFUSCATE_BNM("libil2cpp.so"), RTLD_LAZY);
            if (BNM_dlLib){
                void *init = dlsym(BNM_dlLib, OBFUSCATE_BNM("il2cpp_init"));
                if (init){
                    HOOK(init, BNM_il2cpp_init, old_BNM_il2cpp_init);
                    break;
                }
                dlclose(BNM_dlLib);
            }

        } while (true);
    }).detach();
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
    bool out;
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
    bool out;
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

template<typename Ret, typename ... Args>
Ret BetterCall(MethodInfo *method, void *instance, Args... args) {
    return ((Ret (*)(void *, Args..., MethodInfo *))method->methodPointer)(instance, args..., method);
}

template<typename Ret, typename ... Args>
Ret BetterStaticCall(MethodInfo *method, Args... args) {
#if UNITY_VER > 174
    return ((Ret (*)(Args..., MethodInfo *))method->methodPointer)(args..., method);
#else
    return ((Ret (*)(void *, Args..., MethodInfo *))method->methodPointer)(nullptr, args..., method);
#endif
}

template<typename T>
static T UnBoxObject(T obj) {
    void *val = (void *) (((char *) obj) + sizeof(Il2CppObject));
    return *(T *) val;
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
    //! il2cpp::vm::Class::FromIl2CppType HOOK
    if (!old_Class_FromIl2CppType){
        DWORD from_type_adr = (DWORD)dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_class_from_type"));
        Dl_info info;
        dladdr((void*)from_type_adr, &info);
        DWORD startAddr = (DWORD)info.dli_fbase;
        from_type_adr = FindNext_B_BL_offset(from_type_adr, count);
        HOOK(from_type_adr, new_Class_FromIl2CppType, old_Class_FromIl2CppType);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::FromIl2CppType in lib: 0x%x"), (DWORD)from_type_adr - startAddr);
    }
    //! il2cpp::vm::Class::FromName HOOK
    if (!old_Class_FromName){
        DWORD from_name_adr = (DWORD)dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_class_from_name"));
        Dl_info info;
        dladdr((void*)from_name_adr, &info);
        DWORD startAddr = (DWORD)info.dli_fbase;
        from_name_adr = FindNext_B_BL_offset(FindNext_B_BL_offset(from_name_adr, count), count);
        HOOK(from_name_adr, new_Class_FromName, old_Class_FromName);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::FromName in lib: 0x%x"), (DWORD)from_name_adr - startAddr);
    }
    //! il2cpp::vm::Class::Init HOOK
    if (!Class$$Init_t){
        DWORD Init_adr = (DWORD)dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_array_new_specific"));
        Dl_info info;
        dladdr((void*)Init_adr, &info);
        DWORD startAddr = (DWORD)info.dli_fbase;
        Class$$Init_t = (bool (*)(Il2CppClass *))(FindNext_B_BL_offset(FindNext_B_BL_offset(Init_adr, count), count));
        HOOK(Class$$Init_t, new_Class_Init, old_Class_Init);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Class::Init in lib: 0x%x"), (DWORD)Class$$Init_t - startAddr);
    }
    //! il2cpp::vm::Image::GetTypes HOOK
    if (!Image$$GetTypes_t){
        DO_API(const Il2CppImage*, il2cpp_get_corlib, ());
        DO_API(Il2CppClass*, il2cpp_class_from_name, (const Il2CppImage * image, const char* namespaze, const char *name));
        DO_API(const MethodInfo*, il2cpp_class_get_method_from_name, (Il2CppClass * klass, const char* name, int argsCount));
        auto corlib = il2cpp_get_corlib();
        Dl_info info;
        dladdr((void*)il2cpp_get_corlib, &info);
        DWORD startAddr = (DWORD)info.dli_fbase;
        DWORD GetTypesAdr = 0;
        auto assemblyClass = il2cpp_class_from_name(corlib, OBFUSCATE_BNM("System.Reflection"), OBFUSCATE_BNM("Assembly"));
        Class$$Init(assemblyClass);
        for (int i = 0; i < assemblyClass->method_count; i++) {
            const MethodInfo *method = assemblyClass->methods[i];
            if (!CheckObj(method)) continue;
            if (!strcmp(OBFUSCATE_BNM("GetTypes"), method->name) && method->parameters_count == 1) {
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
#if UNITY_VER <= 174
    //! il2cpp::vm::MetadataCache::GetImageFromIndex HOOK
    if (!old_GetImageFromIndex){
        DWORD adr = (DWORD)dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_assembly_get_image"));
        Dl_info info;
        dladdr((void*)adr, &info);
        DWORD startAddr = (DWORD)info.dli_fbase;
        DWORD AssemblyGetImageOffset = FindNext_B_BL_offset(adr, count);
        DWORD GetImageFromIndexOffset = FindNext_B_BL_offset(AssemblyGetImageOffset, count);
        HOOK(GetImageFromIndexOffset, new_GetImageFromIndex, old_GetImageFromIndex);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::MetadataCache::GetImageFromIndex in lib: 0x%x"), GetImageFromIndexOffset - startAddr);
    }
    static void *old_AssemblyLoad;
    //! il2cpp::vm::Assembly::Load HOOK
    if (!old_AssemblyLoad){
        DWORD adr = (DWORD)dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_domain_assembly_open"));
        Dl_info info;
        dladdr((void*)adr, &info);
        DWORD startAddr = (DWORD)info.dli_fbase;
        DWORD AssemblyLoadOffset = FindNext_B_BL_offset(adr, count);
        HOOK(AssemblyLoadOffset, new_Assembly_Load, old_AssemblyLoad);
        LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::Load in lib: 0x%x"), AssemblyLoadOffset - startAddr);
    }
#endif
    //! il2cpp::vm::Assembly::GetAllAssemblies GET
    if (!Assembly$$GetAllAssemblies_t){
        DO_API(const Il2CppImage*, il2cpp_get_corlib, ());
        DO_API(Il2CppClass*, il2cpp_class_from_name, (const Il2CppImage * image, const char* namespaze, const char *name));
        DO_API(const MethodInfo*, il2cpp_class_get_method_from_name, (Il2CppClass * klass, const char* name, int argsCount));
        auto corlib = il2cpp_get_corlib();
        Dl_info info;
        dladdr((void*)il2cpp_get_corlib, &info);
        DWORD startAddr = (DWORD)info.dli_fbase;
        auto assemblyClass = il2cpp_class_from_name(corlib, OBFUSCATE_BNM("System"), OBFUSCATE_BNM("AppDomain"));
        Class$$Init(assemblyClass);
        const MethodInfo *getAssemb = 0;
        for (int i = 0; i < assemblyClass->method_count; i++) {
            const MethodInfo *method = assemblyClass->methods[i];
            if (!CheckObj(method)) continue;
            if (!strcmp(OBFUSCATE_BNM("GetAssemblies"), method->name) && method->parameters_count == 1) {
                getAssemb = method;
                break;
            }
        }
        if (getAssemb){
            DWORD GetTypesAdr = FindNext_B_BL_offset((DWORD) getAssemb->methodPointer, count);
            Assembly$$GetAllAssemblies_t = (AssemblyVector *(*)(void))(FindNext_B_BL_offset(GetTypesAdr, count+1));
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::GetAllAssemblies by AppDomain in lib: 0x%x"), (DWORD)Assembly$$GetAllAssemblies_t - startAddr);
        } else {
            DWORD adr = (DWORD)dlsym(get_il2cpp(), OBFUSCATE_BNM("il2cpp_domain_get_assemblies"));
            Assembly$$GetAllAssemblies_t = (AssemblyVector *(*)(void))(FindNext_B_BL_offset(adr, count));
            LOGDBNM(OBFUSCATE_BNM("[InitIl2cppMethods] il2cpp::vm::Assembly::GetAllAssemblies by domain in lib: 0x%x"), (DWORD)Assembly$$GetAllAssemblies_t - startAddr);
        }
    }
}

