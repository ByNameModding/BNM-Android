#include <BNM/UserSettings/GlobalSettings.hpp>
#include <BNM/UserSettings/Il2CppMethodNames.hpp>
#include <BNM/Loading.hpp>
#include <BNM/Field.hpp>

#include <Internals.hpp>

using namespace BNM;

void Internal::Load() {
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
    std::shared_lock lock(loadingMutex);
#endif

    // Load BNM
    SetupBNM();

    BNM::Internal::LoadDefaults();

#ifdef BNM_CLASSES_MANAGEMENT

#ifdef BNM_COROUTINE
    BNM::Internal::SetupCoroutine();
#endif

    BNM::Internal::ClassesManagement::ProcessCustomClasses();

#ifdef BNM_COROUTINE
    BNM::Internal::LoadCoroutine();
#endif

#endif
    states.state = true;

    // Call all events after loading il2cpp
    auto events = onIl2CppLoaded;
    for (auto event : events) event();
}

void *Internal::GetIl2CppMethod(const char *methodName) {
    return currentFinderMethod(methodName, currentFinderData);
}

void Loading::AllowedLateInitHook() {
    Internal::states.lateInitAllowed = true;
}

static bool CheckHandle(void *handle) {
    void *init = BNM_dlsym(handle, BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_init));
    if (!init) return false;

    Internal::BNM_il2cpp_init_origin = ::HOOK(init, Internal::BNM_il2cpp_init, Internal::old_BNM_il2cpp_init);

    if (Internal::states.lateInitAllowed) Internal::LateInit(BNM_dlsym(handle, BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_class_from_il2cpp_type)));

    Internal::il2cppLibraryHandle = handle;
    return true;
}

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

bool Loading::TryLoadByJNI(JNIEnv *env, jobject context) {
    bool result = false;

    if (!env || Internal::il2cppLibraryHandle || Internal::states.state) return result;

    if (context == nullptr) {
        jclass activityThread = env->FindClass(BNM_OBFUSCATE_TMP("android/app/ActivityThread"));
        auto currentActivityThread = env->CallStaticObjectMethod(activityThread, env->GetStaticMethodID(activityThread, BNM_OBFUSCATE_TMP("currentActivityThread"), BNM_OBFUSCATE_TMP("()Landroid/app/ActivityThread;")));
        context = env->CallObjectMethod(currentActivityThread, env->GetMethodID(activityThread, BNM_OBFUSCATE_TMP("getApplication"), BNM_OBFUSCATE_TMP("()Landroid/app/Application;")));
        env->DeleteLocalRef(currentActivityThread);
    }

    auto applicationInfo = env->CallObjectMethod(context, env->GetMethodID(env->GetObjectClass(context), BNM_OBFUSCATE_TMP("getApplicationInfo"), BNM_OBFUSCATE_TMP("()Landroid/content/pm/ApplicationInfo;")));
    auto applicationInfoClass = env->GetObjectClass(applicationInfo);

    auto flags = env->GetIntField(applicationInfo, env->GetFieldID(applicationInfoClass, BNM_OBFUSCATE_TMP("flags"), BNM_OBFUSCATE_TMP("I")));
    bool isLibrariesExtracted = (flags & 0x10000000) == 0x10000000; // ApplicationInfo.FLAG_EXTRACT_NATIVE_LIBS

    auto jDir = (jstring) env->GetObjectField(applicationInfo, env->GetFieldID(applicationInfoClass, isLibrariesExtracted ? BNM_OBFUSCATE_TMP("nativeLibraryDir") : BNM_OBFUSCATE_TMP("sourceDir"), BNM_OBFUSCATE_TMP("Ljava/lang/String;")));

    std::string file;
    auto cDir = std::string_view(env->GetStringUTFChars(jDir, nullptr));
    env->DeleteLocalRef(applicationInfo); env->DeleteLocalRef(applicationInfoClass);

    if (isLibrariesExtracted)
        // Path to the library /data/app/.../package name-.../lib/architecture/libil2cpp.so
        file = std::string(cDir) + BNM_OBFUSCATE_TMP("/libil2cpp.so");
    else
        // From base.apk /data/app/.../package name-.../base.apk!/lib/architecture/libil2cpp.so
        file = std::string(cDir) + BNM_OBFUSCATE_TMP("!/lib/" CURRENT_ARCH "/libil2cpp.so");

    // Try to download il2cpp using this path
    auto handle = BNM_dlopen(file.c_str(), RTLD_LAZY);
    if (!(result = CheckHandle(handle))) {
        BNM_LOG_ERR_IF(isLibrariesExtracted, DBG_BNM_MSG_TryLoadByJNI_Fail);
    } else goto FINISH;
    if (isLibrariesExtracted) goto FINISH;
    file.clear();

    // From split_config.architecture.apk /data/app/.../package name-.../split_config.architecture.apk!/lib/architecture/libil2cpp.so
    file = std::string(cDir).substr(0, cDir.length() - 8) + BNM_OBFUSCATE_TMP("split_config." CURRENT_ARCH ".apk!/lib/" CURRENT_ARCH "/libil2cpp.so");
    handle = BNM_dlopen(file.c_str(), RTLD_LAZY);
    if (!(result = CheckHandle(handle))) BNM_LOG_ERR(DBG_BNM_MSG_TryLoadByJNI_Fail);

    FINISH:
    env->ReleaseStringUTFChars(jDir, cDir.data()); env->DeleteLocalRef(jDir);
    return result;
}

bool Loading::TryLoadByDlfcnHandle(void *handle) {
    return CheckHandle(handle);
}

void Loading::SetMethodFinder(BNM::Loading::MethodFinder finderMethod, void *userData) {
    Internal::currentFinderMethod = finderMethod;
    Internal::currentFinderData = userData;
}

bool Loading::TryLoadByUsersFinder() {

    auto init = Internal::currentFinderMethod(BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_init), Internal::currentFinderData);
    if (!init) return false;

    Internal::BNM_il2cpp_init_origin = ::HOOK(init, Internal::BNM_il2cpp_init, Internal::old_BNM_il2cpp_init);

    if (Internal::states.lateInitAllowed) Internal::LateInit(Internal::currentFinderMethod(BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_class_from_il2cpp_type), Internal::currentFinderData));

    return true;
}

void Loading::TrySetupByUsersFinder() {
    return Internal::Load();
}

namespace AssemblerUtils {
    // Reverse hexadecimal string (from 001122 to 221100)
    static std::string ReverseHexString(const std::string &hex) {
        std::string out{};
        for (size_t i = 0; i < hex.length(); i += 2) out.insert(0, hex.substr(i, 2));
        return out;
    }

    // Convert hexadecimal string to a value
    static BNM_PTR HexStr2Value(const std::string &hex) { return strtoull(hex.c_str(), nullptr, 16); }

#if defined(__ARM_ARCH_7A__)

    // Check if the assembly is `bl ...`or `b ...`
    static bool IsBranchHex(const std::string &hex) {
        BNM_PTR hexW = HexStr2Value(ReverseHexString(hex));
        return (hexW & 0x0A000000) == 0x0A000000;
    }

#elif defined(__aarch64__)

    // Check if the assembly is `bl ...`or `b ...`
    static bool IsBranchHex(const std::string &hex) {
        BNM_PTR hexW = HexStr2Value(ReverseHexString(hex));
        return (hexW & 0xFC000000) == 0x14000000 || (hexW & 0xFC000000) == 0x94000000;
    }

#elif defined(__i386__) || defined(__x86_64__)

    // Check if the assembly is `call ...`
    static bool IsCallHex(const std::string &hex) { return hex[0] == 'E' && hex[1] == '8'; }
#elif defined(__riscv)
#error "Is it released for Android?"
#else
#error "BNM only supports arm64, arm, x86 and x86_64"
#endif
    static const char *hexChars = BNM_OBFUSCATE_TMP("0123456789ABCDEF");
    // Прочитать память, как шестнадцатеричную строку
    // Read the memory as a hexadecimal string
    template<size_t len>
    static std::string ReadMemory(BNM_PTR address) {
        char temp[len]; memset(temp, 0, len);
        std::string ret{};
        if (memcpy(temp, (void *)address, len) == nullptr) return std::move(ret);
        ret.resize(len * 2, 0);
        auto buf = (char *)ret.data();
        for (size_t i = 0; i < len; ++i) {
            *buf++ = hexChars[temp[i] >> 4];
            *buf++ = hexChars[temp[i] & 0x0F];
        }
        return std::move(ret);
    }

    // Decode b or bl and get the address it goes to
    static bool DecodeBranchOrCall(const std::string &hex, BNM_PTR offset, BNM_PTR &outOffset) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
        if (!IsBranchHex(hex)) return false;
#if defined(__aarch64__)
        uint8_t add = 0;
#else
        uint8_t add = 8;
#endif
        // This line is based on the capstone code
        outOffset = ((int32_t)(((((HexStr2Value(ReverseHexString(hex))) & (((uint32_t)1 << 24) - 1) << 0) >> 0) << 2) << (32 - 26)) >> (32 - 26)) + offset + add;
#elif defined(__i386__) || defined(__x86_64__)
        if (!IsCallHex(hex)) return false;
        // Address + address from the `call` + size of the instruction
        outOffset = offset + HexStr2Value(ReverseHexString(hex).substr(0, 8)) + 5;
#else
#error "BNM only supports arm64, arm, x86 and x86_64"
        return false;
#endif
        return true;
    }

    // Goes through memory and tries to find b-, bl- or call instructions
    // Then gets the address they go to
    // index: 1 is the first, 2 is the second, etc.
    static BNM_PTR FindNextJump(BNM_PTR start, uint8_t index) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
        BNM_PTR offset = 0;
        std::string curHex = ReadMemory<4>(start);
        BNM_PTR outOffset = 0;
        bool out;
        while (!(out = DecodeBranchOrCall(curHex, start + offset, outOffset)) || index != 1) {
            offset += 4;
            curHex = ReadMemory<4>(start + offset);
            if (out) index--;
        }
        return outOffset;
#elif defined(__i386__) || defined(__x86_64__)
        BNM_PTR offset = 0;
        std::string curHex = ReadMemory<1>(start);
        BNM_PTR outOffset = 0;
        bool out;
        while (!(out = IsCallHex(curHex)) || index != 1) {
            offset += 1;
            curHex = ReadMemory<1>(start + offset);
            if (out) index--;
        }
        DecodeBranchOrCall(ReadMemory<5>(start + offset), start + offset, outOffset);
        return outOffset;
#else
#error "BNM only supports arm64, arm, x86 and x86_64"
        return 0;
#endif
    }
}

void Internal::LateInit(void *il2cpp_class_from_il2cpp_type_addr) {
    if (!il2cpp_class_from_il2cpp_type_addr) return;

#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
    const uint8_t count = 1;
#elif defined(__i386__) || defined(__x86_64__)
    // x86 has one add-on call at start
    const uint8_t count = 2;
#endif

    //! il2cpp::vm::Class::FromIl2CppType
    // Путь (Path):
    // il2cpp_class_from_il2cpp_type ->
    // il2cpp::vm::Class::FromIl2CppType
    auto from_il2cpp_type = AssemblerUtils::FindNextJump((BNM_PTR) il2cpp_class_from_il2cpp_type_addr, count);

    Internal::BNM_il2cpp_class_from_system_type_origin = ::HOOK(from_il2cpp_type, Internal::BNM_il2cpp_class_from_system_type, Internal::old_BNM_il2cpp_class_from_system_type);
}

static void EmptyMethod() {}

#ifdef BNM_DEBUG
static void *OffsetInLib(void *offsetInMemory) {
    if (offsetInMemory == nullptr) return nullptr;
    Dl_info info; BNM_dladdr(offsetInMemory, &info);
    return (void *) ((BNM_PTR) offsetInMemory - (BNM_PTR) info.dli_fbase);
}

void *Utils::OffsetInLib(void *offsetInMemory) {
    return ::OffsetInLib(offsetInMemory);
}
#endif

void Internal::SetupBNM() {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
    const uint8_t count = 1;
#elif defined(__i386__) || defined(__x86_64__)
    // x86 has one add-on call at start
    const uint8_t count = 2;
#endif

    //! il2cpp::vm::Class::Init
    // Path:
    // il2cpp_array_new_specific ->
    // il2cpp::vm::Array::NewSpecific ->
    // il2cpp::vm::Class::Init
    Class$$Init = (decltype(Class$$Init)) AssemblerUtils::FindNextJump(AssemblerUtils::FindNextJump((BNM_PTR) GetIl2CppMethod(BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_array_new_specific)), count), count);
    BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_Class_Init, OffsetInLib((void *)Class$$Init));


#define INIT_IL2CPP_API(name) il2cppMethods.name = (decltype(il2cppMethods.name)) GetIl2CppMethod(BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_##name))

    INIT_IL2CPP_API(il2cpp_image_get_class);
    INIT_IL2CPP_API(il2cpp_get_corlib);
    INIT_IL2CPP_API(il2cpp_class_from_name);
    INIT_IL2CPP_API(il2cpp_assembly_get_image);
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
    INIT_IL2CPP_API(il2cpp_runtime_invoke);
    INIT_IL2CPP_API(il2cpp_domain_get);
    INIT_IL2CPP_API(il2cpp_thread_current);
    INIT_IL2CPP_API(il2cpp_thread_attach);
    INIT_IL2CPP_API(il2cpp_thread_detach);

#undef INIT_IL2CPP_API

    //! il2cpp::vm::Image::GetTypes
    if (il2cppMethods.il2cpp_image_get_class == nullptr) {
        auto assemblyClass = il2cppMethods.il2cpp_class_from_name(il2cppMethods.il2cpp_get_corlib(), BNM_OBFUSCATE_TMP("System.Reflection"), BNM_OBFUSCATE_TMP("Assembly"));
        BNM_PTR GetTypesAdr = Class(assemblyClass).GetMethod(BNM_OBFUSCATE_TMP("GetTypes"), 1).GetOffset();

#if UNITY_VER >= 211
        const int sCount = count;
#elif UNITY_VER > 174
        const int sCount = count + 1;
#else
        const int sCount = count + 2;
#endif
        // Path:
        // System.Reflection.Assembly.GetTypes(bool) ->
        // il2cpp::icalls::mscorlib::System::Reflection::Assembly::GetTypes ->
        // il2cpp::icalls::mscorlib::System::Module::InternalGetTypes ->
        // il2cpp::vm::Image::GetTypes
        orig_Image$$GetTypes = (decltype(orig_Image$$GetTypes)) AssemblerUtils::FindNextJump(AssemblerUtils::FindNextJump(AssemblerUtils::FindNextJump(GetTypesAdr, count), sCount), count);

        BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_Image_GetTypes, OffsetInLib((void *)orig_Image$$GetTypes));
    } else BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_image_get_class_exists);

#ifdef BNM_CLASSES_MANAGEMENT

    //! il2cpp::vm::Class::FromIl2CppType
    // Path:
    // il2cpp_class_from_type ->
    // il2cpp::vm::Class::FromIl2CppType
    auto from_type_adr = AssemblerUtils::FindNextJump((BNM_PTR) GetIl2CppMethod(BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_class_from_type)), count);
    ::HOOK(from_type_adr, ClassesManagement::Class$$FromIl2CppType, ClassesManagement::old_Class$$FromIl2CppType);
    BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_Class_FromIl2CppType, OffsetInLib((void *)from_type_adr));


    //! il2cpp::vm::Type::GetClassOrElementClass
    // Path:
    // il2cpp_type_get_class_or_element_class ->
    // il2cpp::vm::Type::GetClassOrElementClass
    auto type_get_class_adr = AssemblerUtils::FindNextJump((BNM_PTR) GetIl2CppMethod(BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_type_get_class_or_element_class)), count);
    ::HOOK(type_get_class_adr, ClassesManagement::Type$$GetClassOrElementClass, ClassesManagement::old_Type$$GetClassOrElementClass);
    BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_Type_GetClassOrElementClass, OffsetInLib((void *)type_get_class_adr));

    //! il2cpp::vm::Image::ClassFromName
    // Path:
    // il2cpp_class_from_name ->
    // il2cpp::vm::Class::FromName ->
    // il2cpp::vm::Image::ClassFromName
    auto from_name_adr = AssemblerUtils::FindNextJump(AssemblerUtils::FindNextJump((BNM_PTR) il2cppMethods.il2cpp_class_from_name, count), count);
    ::HOOK(from_name_adr, ClassesManagement::Class$$FromName, ClassesManagement::old_Class$$FromName);
    BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_Image_FromName, OffsetInLib((void *)from_name_adr));
#if UNITY_VER <= 174

    //! il2cpp::vm::MetadataCache::GetImageFromIndex
    // Path:
    // il2cpp_assembly_get_image ->
    // il2cpp::vm::Assembly::GetImage ->
    // il2cpp::vm::MetadataCache::GetImageFromIndex
    auto GetImageFromIndexOffset = AssemblerUtils::FindNextJump(AssemblerUtils::FindNextJump((BNM_PTR) il2cppMethods.il2cpp_assembly_get_image, count), count);
    ::HOOK(GetImageFromIndexOffset, ClassesManagement::new_GetImageFromIndex, ClassesManagement::old_GetImageFromIndex);
    BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_MetadataCache_GetImageFromIndex, OffsetInLib((void *)GetImageFromIndexOffset));

    //! il2cpp::vm::Assembly::Load
    // Path:
    // il2cpp_domain_assembly_open ->
    // il2cpp::vm::Assembly::Load
    BNM_PTR AssemblyLoadOffset = AssemblerUtils::FindNextJump((BNM_PTR) BNM_dlsym(il2cppLibraryHandle, BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_domain_assembly_open)), count);
    ::HOOK(AssemblyLoadOffset, ClassesManagement::Assembly$$Load, nullptr);
    BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_Assembly_Load, OffsetInLib((void *)AssemblyLoadOffset));

#endif
#endif

    //! il2cpp::vm::Assembly::GetAllAssemblies
    // Path:
    // il2cpp_domain_get_assemblies ->
    // il2cpp::vm::Assembly::GetAllAssemblies
    auto adr = (BNM_PTR) GetIl2CppMethod(BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_domain_get_assemblies));
    Assembly$$GetAllAssemblies = (std::vector<IL2CPP::Il2CppAssembly *> *(*)())(AssemblerUtils::FindNextJump(adr, count));
    BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_Assembly_GetAllAssemblies, OffsetInLib((void *)Assembly$$GetAllAssemblies));

    auto mscorlib = il2cppMethods.il2cpp_get_corlib();

    // Get MakeGenericMethod_impl. Depending on the version of Unity, it may be in different classes.
    auto runtimeMethodInfoClassPtr = TryGetClassInImage(mscorlib, BNM_OBFUSCATE_TMP("System.Reflection"), BNM_OBFUSCATE_TMP("RuntimeMethodInfo"));
    if (runtimeMethodInfoClassPtr) {
        Internal::Class$$Init(runtimeMethodInfoClassPtr);
        vmData.RuntimeMethodInfo$$MakeGenericMethod_impl = BNM::MethodBase(IterateMethods(runtimeMethodInfoClassPtr, [methodName = BNM_OBFUSCATE_TMP("MakeGenericMethod_impl")](const MethodBase &methodBase) {
            return !strcmp(methodBase._data->name, methodName);
        }));
    }
    if (!vmData.RuntimeMethodInfo$$MakeGenericMethod_impl.Initialized())
        vmData.RuntimeMethodInfo$$MakeGenericMethod_impl = Class(BNM_OBFUSCATE_TMP("System.Reflection"), BNM_OBFUSCATE_TMP("MonoMethod"), mscorlib).GetMethod(BNM_OBFUSCATE_TMP("MakeGenericMethod_impl"));

    auto runtimeTypeClass = Class(BNM_OBFUSCATE_TMP("System"), BNM_OBFUSCATE_TMP("RuntimeType"), mscorlib);
    auto stringClass = Class(BNM_OBFUSCATE_TMP("System"), BNM_OBFUSCATE_TMP("String"), mscorlib);
    auto interlockedClass = Class(BNM_OBFUSCATE_TMP("System.Threading"), BNM_OBFUSCATE_TMP("Interlocked"), mscorlib);
    auto objectClass = Class(BNM_OBFUSCATE_TMP("System"), BNM_OBFUSCATE_TMP("Object"), mscorlib);
    for (uint16_t slot = 0; slot < objectClass._data->vtable_count; slot++) {
        const BNM::IL2CPP::MethodInfo* vMethod = objectClass._data->vtable[slot].method;
        if (strcmp(vMethod->name, BNM_OBFUSCATE_TMP("Finalize")) != 0) continue;
        finalizerSlot = slot;
        break;
    }

    auto UnityEngineCoreModule = Image(BNM_OBFUSCATE_TMP("UnityEngine.CoreModule.dll"));

    vmData.Object = objectClass;
    vmData.UnityEngine$$Object = Class(BNM_OBFUSCATE_TMP("UnityEngine"), BNM_OBFUSCATE_TMP("Object"), UnityEngineCoreModule);
    vmData.Type$$GetType = Class(BNM_OBFUSCATE_TMP("System"), BNM_OBFUSCATE_TMP("Type"), mscorlib).GetMethod(BNM_OBFUSCATE_TMP("GetType"), 1);
    vmData.Interlocked$$CompareExchange = interlockedClass.GetMethod(BNM_OBFUSCATE_TMP("CompareExchange"), {objectClass, objectClass, objectClass});
    vmData.RuntimeType$$MakeGenericType = runtimeTypeClass.GetMethod(BNM_OBFUSCATE_TMP("MakeGenericType"), 2);
    vmData.RuntimeType$$MakePointerType = runtimeTypeClass.GetMethod(BNM_OBFUSCATE_TMP("MakePointerType"), 1);
    vmData.RuntimeType$$make_byref_type = runtimeTypeClass.GetMethod(BNM_OBFUSCATE_TMP("make_byref_type"), 0);
    vmData.String$$Empty = stringClass.GetField(BNM_OBFUSCATE_TMP("Empty")).cast<Structures::Mono::String *>().GetPointer();

    auto listClass = vmData.System$$List = Class(BNM_OBFUSCATE_TMP("System.Collections.Generic"), BNM_OBFUSCATE_TMP("List`1"));
    auto cls = listClass._data;
    auto size = sizeof(IL2CPP::Il2CppClass) + cls->vtable_count * sizeof(IL2CPP::VirtualInvokeData);
    listClass._data = (IL2CPP::Il2CppClass *) BNM_malloc(size);
    memcpy(listClass._data, cls, size);
    listClass._data->has_finalize = 0;
    listClass._data->instance_size = sizeof(Structures::Mono::List<void*>);

    // Bypassing the creation of a static _emptyArray field because it cannot exist
    listClass._data->has_cctor = 0;
    listClass._data->cctor_started = 0;
#if UNITY_VER >= 212
    listClass._data->cctor_finished_or_no_cctor = 1;
#else
    listClass._data->cctor_finished = 1;
#endif

    auto constructor = listClass.GetMethod(Internal::constructorName, 0)._data;

    auto newMethods = (IL2CPP::MethodInfo **) BNM_malloc(sizeof(IL2CPP::MethodInfo *) * listClass._data->method_count);
    memcpy(newMethods, listClass._data->methods, sizeof(IL2CPP::MethodInfo *) * listClass._data->method_count);

    auto newConstructor = (IL2CPP::MethodInfo *) BNM_malloc(sizeof(IL2CPP::MethodInfo));
    *newConstructor = *constructor;
    newConstructor->methodPointer = (decltype(newConstructor->methodPointer)) EmptyMethod;
    newConstructor->invoker_method = (decltype(newConstructor->invoker_method)) EmptyMethod;

    for (uint16_t i = 0; i < listClass._data->method_count; ++i) {
        if (listClass._data->methods[i] == constructor) {
            newMethods[i] = newConstructor;
            continue;
        }
        newMethods[i] = (IL2CPP::MethodInfo *) listClass._data->methods[i];
    }
    listClass._data->methods = (const IL2CPP::MethodInfo **) newMethods;
    customListTemplateClass = listClass;
}

void Loading::AddOnLoadedEvent(void (*event)()) {
    if (event) Internal::onIl2CppLoaded.push_back(event);
}

void Loading::ClearOnLoadedEvents() {
    Internal::onIl2CppLoaded.clear();
}