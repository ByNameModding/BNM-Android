#include <BNM/UserSettings/GlobalSettings.hpp>
#include <BNM/UserSettings/Il2CppMethodNames.hpp>
#include <BNM/Loading.hpp>
#include <BNM/Field.hpp>

#include "Internals.hpp"

using namespace BNM;

void Internal::Load() {

    // Загрузить BNM
    SetupBNM();
#ifdef BNM_CLASSES_MANAGEMENT

#ifdef BNM_COROUTINE
    BNM::Internal::SetupCoroutine();
#endif

    BNM::Internal::ClassesManagement::ProcessCustomClasses();

#ifdef BNM_COROUTINE
    BNM::Internal::LoadCoroutine();
#endif

#endif
    state = true;

    // Вызвать все события после загрузки il2cpp
    auto events = onIl2CppLoaded;
    for (auto event : events) event();
}

void *Internal::GetIl2CppMethod(const char *methodName) {
    if (!usersFinderMethod) goto DLFCN;
    
    return usersFinderMethod(methodName, usersFinderMethodData);
    
    DLFCN:;
    if (!il2cppLibraryHandle) return nullptr;
    return BNM_dlsym(il2cppLibraryHandle, methodName);
}

void Loading::AllowedLateInitHook() {
    Internal::lateInitAllowed = true;
}

bool CheckHandle(void *handle) {
    void *init = BNM_dlsym(handle, OBFUSCATE_BNM(BNM_IL2CPP_API_il2cpp_init));
    if (!init) return false;

    ::HOOK(init, Internal::BNM_il2cpp_init, Internal::old_BNM_il2cpp_init);

    if (Internal::lateInitAllowed) Internal::LateInit(BNM_dlsym(handle, OBFUSCATE_BNM(BNM_IL2CPP_API_il2cpp_class_from_il2cpp_type)));

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

    if (!env || Internal::il2cppLibraryHandle || Internal::state) return result;

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

    auto jDir = (jstring) env->GetObjectField(applicationInfo, env->GetFieldID(applicationInfoClass, isLibrariesExtracted ? OBFUSCATE_BNM("nativeLibraryDir") : OBFUSCATE_BNM("sourceDir"), OBFUSCATE_BNM("Ljava/lang/String;")));

    std::string file;
    auto cDir = std::string_view(env->GetStringUTFChars(jDir, nullptr));
    env->DeleteLocalRef(applicationInfo); env->DeleteLocalRef(applicationInfoClass);

    if (isLibrariesExtracted)
        // По пути до библиотеки /data/app/.../имя пакета-.../lib/архитектура/libil2cpp.so
        file = std::string(cDir) + OBFUSCATE_BNM("/libil2cpp.so");
    else
        // Из base.apk /data/app/.../имя пакета-.../base.apk!/lib/архитектура/libil2cpp.so
        file = std::string(cDir) + OBFUSCATE_BNM("!/lib/" CURRENT_ARCH "/libil2cpp.so");

    // Попробовать загрузить il2cpp по этому пути
    auto handle = BNM_dlopen(file.c_str(), RTLD_LAZY);
    if (!(result = CheckHandle(handle))) {
        BNM_LOG_ERR_IF(isLibrariesExtracted, "Не удалось загрузить libil2cpp.so по пути!");
    } else goto FINISH;
    if (isLibrariesExtracted) goto FINISH;
    file.clear();

    // Из split_config.архитектура.apk /data/app/.../имя пакета-.../split_config.архитектура.apk!/lib/архитектура/libil2cpp.so
    file = std::string(cDir).substr(0, cDir.length() - 8) + OBFUSCATE_BNM("split_config." CURRENT_ARCH ".apk!/lib/" CURRENT_ARCH "/libil2cpp.so");
    handle = BNM_dlopen(file.c_str(), RTLD_LAZY);
    if (!(result = CheckHandle(handle))) BNM_LOG_ERR("Не удалось загрузить libil2cpp.so по пути!");

    FINISH:
    env->ReleaseStringUTFChars(jDir, cDir.data()); env->DeleteLocalRef(jDir);
    return result;
}

bool Loading::TryLoadByDlfcnHandle(void *handle) {
    return CheckHandle(handle);
}

void Loading::SetMethodFinder(BNM::Loading::MethodFinder finderMethod, void *userData) {
    Internal::usersFinderMethod = finderMethod;
    Internal::usersFinderMethodData = userData;
}

bool Loading::TryLoadByUsersFinder() {

    auto init = Internal::usersFinderMethod(OBFUSCATE_BNM(BNM_IL2CPP_API_il2cpp_init), Internal::usersFinderMethodData);
    if (!init) return false;

    ::HOOK(init, Internal::BNM_il2cpp_init, Internal::old_BNM_il2cpp_init);

    if (Internal::lateInitAllowed) Internal::LateInit(Internal::usersFinderMethod(OBFUSCATE_BNM(BNM_IL2CPP_API_il2cpp_class_from_il2cpp_type), Internal::usersFinderMethodData));

    return true;
}

void Loading::TrySetupByUsersFinder() {
    return Internal::Load();
}

namespace AssemblerUtils {
    // Перевернуть шестнадцатеричную строку (из 001122 в 221100)
    std::string ReverseHexString(const std::string &hex) {
        std::string out{};
        for (size_t i = 0; i < hex.length(); i += 2) out.insert(0, hex.substr(i, 2));
        return out;
    }

    // Конвертировать шестнадцатеричную строку в значение
    unsigned long long HexStr2Value(const std::string &hex) { return strtoull(hex.c_str(), nullptr, 16); }

#if defined(__ARM_ARCH_7A__)

    // Проверить, является ли ассемблер `bl ...` или `b ...`
    bool IsBranchHex(const std::string &hex) {
        BNM_PTR hexW = HexStr2Value(ReverseHexString(hex));
        return (hexW & 0x0A000000) == 0x0A000000;
    }

#elif defined(__aarch64__)

    // Проверить, является ли ассемблер `bl ...` или `b ...`
    bool IsBranchHex(const std::string &hex) {
        BNM_PTR hexW = HexStr2Value(ReverseHexString(hex));
        return (hexW & 0xFC000000) == 0x14000000 || (hexW & 0xFC000000) == 0x94000000;
    }

#elif defined(__i386__) || defined(__x86_64__)

    // Проверить, является ли ассемблер `call ...`
    bool IsCallHex(const std::string &hex) { return hex[0] == 'E' && hex[1] == '8'; }
#elif defined(__riscv)
#error "Он вышел для Android?"
#else
#error "BNM поддерживает только arm64, arm, x86 и x86_64"
#endif
    const char *hexChars = OBFUSCATE_BNM("0123456789ABCDEF");
    // Прочитать память, как шестнадцатеричную строку
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

    // Декодировать b или bl и получить адрес, по которому он переходит
    bool DecodeBranchOrCall(const std::string &hex, BNM_PTR offset, BNM_PTR &outOffset) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
        if (!IsBranchHex(hex)) return false;
#if defined(__aarch64__)
        uint8_t add = 0;
#else
        uint8_t add = 8;
#endif
        // Эта строка основана на коде capstone
        outOffset = ((int32_t)(((((HexStr2Value(ReverseHexString(hex))) & (((uint32_t)1 << 24) - 1) << 0) >> 0) << 2) << (32 - 26)) >> (32 - 26)) + offset + add;
#elif defined(__i386__) || defined(__x86_64__)
        if (!IsCallHex(hex)) return false;
        // Адрес + адрес из `call` + размер инструкции
        outOffset = offset + HexStr2Value(ReverseHexString(hex).substr(0, 8)) + 5;
#else
#error "BNM поддерживает только arm64, arm, x86 и x86_64"
        return false;
#endif
        return true;
    }

    // Идёт по памяти и пытается найти b-, bl- или call-инструкции
    // Потом получает адрес, по которому они переходят
    // index: 0 - первый, 1 - второй и т. д.
    BNM_PTR FindNextJump(BNM_PTR start, uint8_t index) {
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
#else
#error "BNM поддерживает только arm64, arm, x86 и x86_64"
        return 0;
#endif
    }
}

void Internal::LateInit(void *il2cpp_class_from_il2cpp_type_addr) {
    if (!il2cpp_class_from_il2cpp_type_addr) return;

#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
    const uint8_t count = 1;
#elif defined(__i386__) || defined(__x86_64__)
    // x86 имеет один доп. вызов в начеле
    const uint8_t count = 2;
#endif

    //! il2cpp::vm::Class::FromIl2CppType
    // Путь:
    // il2cpp_class_from_il2cpp_type ->
    // il2cpp::vm::Class::FromIl2CppType
    auto from_il2cpp_type = AssemblerUtils::FindNextJump((BNM_PTR) il2cpp_class_from_il2cpp_type_addr, count);

    Internal::BNM_il2cpp_class_from_system_type_origin = ::HOOK(from_il2cpp_type, Internal::BNM_il2cpp_class_from_system_type, Internal::old_BNM_il2cpp_class_from_system_type);
}

void EmptyMethod() {}

#ifdef BNM_DEBUG
void *OffsetInLib(void *offsetInMemory) {
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
    // x86 имеет один доп. вызов в начеле
    const uint8_t count = 2;
#endif

    //! il2cpp::vm::Class::Init
    // Путь:
    // il2cpp_array_new_specific ->
    // il2cpp::vm::Array::NewSpecific ->
    // il2cpp::vm::Class::Init
    Class$$Init = (decltype(Class$$Init)) AssemblerUtils::FindNextJump(AssemblerUtils::FindNextJump((BNM_PTR) BNM_dlsym(il2cppLibraryHandle, OBFUSCATE_BNM("il2cpp_array_new_specific")), count), count);
    BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::Class::Init в библиотеке: %p.", OffsetInLib((void *)Class$$Init));


#define INIT_IL2CPP_API(name) il2cppMethods.name = (decltype(il2cppMethods.name)) GetIl2CppMethod(BNM_IL2CPP_API_##name)

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

#undef INIT_IL2CPP_API
    
    //! il2cpp::vm::Image::GetTypes
    if (il2cppMethods.il2cpp_image_get_class == nullptr) {
        auto assemblyClass = il2cppMethods.il2cpp_class_from_name(il2cppMethods.il2cpp_get_corlib(), OBFUSCATE_BNM("System.Reflection"), OBFUSCATE_BNM("Assembly"));
        BNM_PTR GetTypesAdr = Class(assemblyClass).GetMethod(OBFUSCATE_BNM("GetTypes"), 1).GetOffset();

#if UNITY_VER >= 211
        const int sCount = count;
#elif UNITY_VER > 174
        const int sCount = count + 1;
#else
        const int sCount = count + 2;
#endif
        // Путь:
        // System.Reflection.Assembly.GetTypes(bool) ->
        // il2cpp::icalls::mscorlib::System::Reflection::Assembly::GetTypes ->
        // il2cpp::icalls::mscorlib::System::Module::InternalGetTypes ->
        // il2cpp::vm::Image::GetTypes
        orig_Image$$GetTypes = (decltype(orig_Image$$GetTypes)) AssemblerUtils::FindNextJump(AssemblerUtils::FindNextJump(AssemblerUtils::FindNextJump(GetTypesAdr, count), sCount), count);

        BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::Image::GetTypes в библиотеке: %p.", OffsetInLib((void *)orig_Image$$GetTypes));
    } else BNM_LOG_DEBUG("[SetupBNM] в коде есть il2cpp_image_get_class. BNM будет использовать его.");

#ifdef BNM_CLASSES_MANAGEMENT

    //! il2cpp::vm::Class::FromIl2CppType
    // Путь:
    // il2cpp_class_from_type ->
    // il2cpp::vm::Class::FromIl2CppType
    auto from_type_adr = AssemblerUtils::FindNextJump((BNM_PTR) BNM_dlsym(il2cppLibraryHandle, OBFUSCATE_BNM("il2cpp_class_from_type")), count);
    ::HOOK(from_type_adr, ClassesManagement::Class$$FromIl2CppType, ClassesManagement::old_Class$$FromIl2CppType);
    BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::Class::FromIl2CppType в библиотеке: %p.", OffsetInLib((void *)from_type_adr));


    //! il2cpp::vm::Type::GetClassOrElementClass
    // Путь:
    // il2cpp_type_get_class_or_element_class ->
    // il2cpp::vm::Type::GetClassOrElementClass
    auto type_get_class_adr = AssemblerUtils::FindNextJump((BNM_PTR) BNM_dlsym(il2cppLibraryHandle, OBFUSCATE_BNM("il2cpp_type_get_class_or_element_class")), count);
    ::HOOK(type_get_class_adr, ClassesManagement::Type$$GetClassOrElementClass, ClassesManagement::old_Type$$GetClassOrElementClass);
    BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::Type::GetClassOrElementClass в библиотеке: %p.", OffsetInLib((void *)type_get_class_adr));

    //! il2cpp::vm::Image::ClassFromName
    // Путь:
    // il2cpp_class_from_name ->
    // il2cpp::vm::Class::FromName ->
    // il2cpp::vm::Image::ClassFromName
    auto from_name_adr = AssemblerUtils::FindNextJump(AssemblerUtils::FindNextJump((BNM_PTR) il2cppMethods.il2cpp_class_from_name, count), count);
    ::HOOK(from_name_adr, ClassesManagement::Class$$FromName, ClassesManagement::old_Class$$FromName);
    BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::Image::FromName в библиотеке: %p.", OffsetInLib((void *)from_name_adr));
#if UNITY_VER <= 174

    //! il2cpp::vm::MetadataCache::GetImageFromIndex
    // Путь:
    // il2cpp_assembly_get_image ->
    // il2cpp::vm::Assembly::GetImage ->
    // il2cpp::vm::MetadataCache::GetImageFromIndex
    auto GetImageFromIndexOffset = AssemblerUtils::FindNextJump(AssemblerUtils::FindNextJump((BNM_PTR) il2cppMethods.il2cpp_assembly_get_image, count), count);
    ::HOOK(GetImageFromIndexOffset, ClassesManagement::new_GetImageFromIndex, ClassesManagement::old_GetImageFromIndex);
    BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::MetadataCache::GetImageFromIndex в библиотеке: %p.", OffsetInLib((void *)GetImageFromIndexOffset));

    //! il2cpp::vm::Assembly::Load
    // Путь:
    // il2cpp_domain_assembly_open ->
    // il2cpp::vm::Assembly::Load
    BNM_PTR AssemblyLoadOffset = AssemblerUtils::FindNextJump((BNM_PTR) BNM_dlsym(il2cppLibraryHandle, OBFUSCATE_BNM("il2cpp_domain_assembly_open")), count);
    ::HOOK(AssemblyLoadOffset, ClassesManagement::Assembly$$Load, nullptr);
    BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::Assembly::Load в библиотеке: %p.", OffsetInLib((void *)AssemblyLoadOffset));

#endif
#endif

    //! il2cpp::vm::Assembly::GetAllAssemblies
#ifdef BNM_USE_APPDOMAIN
    auto assemblyClass = il2cppMethods.il2cpp_class_from_name(il2cppMethods.il2cpp_get_corlib(), OBFUSCATE_BNM("System"), OBFUSCATE_BNM("AppDomain"));
    auto getAssembly = Class(assemblyClass).GetMethod(OBFUSCATE_BNM("GetAssemblies"), 1);
    if (getAssembly) {
        const int sCount
#if !defined(__aarch64__) && UNITY_VER >= 211
            = count;
#else
            = count + 1;
#endif
        // Путь:
        // System.AppDomain.GetAssemblies(bool) ->
        // il2cpp::icalls::mscorlib::System::AppDomain::GetAssemblies ->
        // il2cpp::vm::Assembly::GetAllAssemblies
        BNM_PTR GetAssembliesAdr = AssemblerUtils::FindNextJump(getAssembly.GetOffset(), count);
        Assembly$$GetAllAssemblies = (std::vector<IL2CPP::Il2CppAssembly *> *(*)())(AssemblerUtils::FindNextJump(GetAssembliesAdr, sCount));
        BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::Assembly::GetAllAssemblies через AppDomain в библиотеке: %p.", OffsetInLib((void *)Assembly$$GetAllAssemblies));
    } else {
#endif
    // Путь:
    // il2cpp_domain_get_assemblies ->
    // il2cpp::vm::Assembly::GetAllAssemblies
    auto adr = (BNM_PTR) BNM_dlsym(il2cppLibraryHandle, OBFUSCATE_BNM("il2cpp_domain_get_assemblies"));
    Assembly$$GetAllAssemblies = (std::vector<IL2CPP::Il2CppAssembly *> *(*)())(AssemblerUtils::FindNextJump(adr, count));
    BNM_LOG_DEBUG("[SetupBNM] il2cpp::vm::Assembly::GetAllAssemblies через domain в библиотеке: %p.", OffsetInLib((void *)Assembly$$GetAllAssemblies));
    
#ifdef BNM_USE_APPDOMAIN
    }
#endif

    auto mscorlib = il2cppMethods.il2cpp_get_corlib();

    // Получить MakeGenericMethod_impl. В зависимости от версии Unity, он может быть в разных классах.
    auto runtimeMethodInfoClassPtr = TryGetClassInImage(mscorlib, OBFUSCATE_BNM("System.Reflection"), OBFUSCATE_BNM("RuntimeMethodInfo"));
    if (runtimeMethodInfoClassPtr) {
        Internal::Class$$Init(runtimeMethodInfoClassPtr);
        vmData.RuntimeMethodInfo$$MakeGenericMethod_impl = BNM::MethodBase(IterateMethods(runtimeMethodInfoClassPtr, [](const MethodBase &methodBase) {
            return !strcmp(methodBase._data->name, OBFUSCATE_BNM("MakeGenericMethod_impl"));
        }));
    }
    if (!vmData.RuntimeMethodInfo$$MakeGenericMethod_impl)
        vmData.RuntimeMethodInfo$$MakeGenericMethod_impl = Class(OBFUSCATE_BNM("System.Reflection"), OBFUSCATE_BNM("MonoMethod"), mscorlib).GetMethod(OBFUSCATE_BNM("MakeGenericMethod_impl"));

    auto runtimeTypeClass = Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("RuntimeType"), mscorlib);
    auto stringClass = Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("String"), mscorlib);
    auto interlockedClass = Class(OBFUSCATE_BNM("System.Threading"), OBFUSCATE_BNM("Interlocked"), mscorlib);
    auto objectClass = Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Object"), mscorlib);
    for (uint16_t slot = 0; slot < objectClass._data->vtable_count; slot++) {
        const BNM::IL2CPP::MethodInfo* vMethod = objectClass._data->vtable[slot].method;
        if (strcmp(vMethod->name, OBFUSCATE_BNM("Finalize")) != 0) continue;
        finalizerSlot = slot;
        break;
    }

    auto UnityEngineCoreModule = Image(OBFUSCATE_BNM("UnityEngine.CoreModule.dll"));

    vmData.Object = objectClass;
    vmData.UnityEngine$$Object = Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Object"), UnityEngineCoreModule);
    vmData.Type$$GetType = Class(OBFUSCATE_BNM("System"), OBFUSCATE_BNM("Type"), mscorlib).GetMethod(OBFUSCATE_BNM("GetType"), 1);
    vmData.Interlocked$$CompareExchange = interlockedClass.GetMethod(OBFUSCATE_BNM("CompareExchange"), {objectClass, objectClass, objectClass});
    vmData.RuntimeType$$MakeGenericType = runtimeTypeClass.GetMethod(OBFUSCATE_BNM("MakeGenericType"), 2);
    vmData.RuntimeType$$MakePointerType = runtimeTypeClass.GetMethod(OBFUSCATE_BNM("MakePointerType"), 1);
    vmData.RuntimeType$$make_byref_type = runtimeTypeClass.GetMethod(OBFUSCATE_BNM("make_byref_type"), 0);
    vmData.String$$Empty = stringClass.GetField(OBFUSCATE_BNM("Empty")).cast<Structures::Mono::String *>().GetPointer();

    auto listClass = Class(OBFUSCATE_BNM("System.Collections.Generic"), OBFUSCATE_BNM("List`1"));
    auto cls = listClass._data;
    auto size = sizeof(IL2CPP::Il2CppClass) + cls->vtable_count * sizeof(IL2CPP::VirtualInvokeData);
    listClass._data = (IL2CPP::Il2CppClass *) malloc(size);
    memcpy(listClass._data, cls, size);
    listClass._data->has_finalize = 0;
    listClass._data->instance_size = sizeof(Structures::Mono::List<void*>);

    // Обход создания статического поля _emptyArray, потому что его не может существовать
    listClass._data->has_cctor = 0;
    listClass._data->cctor_started = 0;
#if UNITY_VER >= 212
    listClass._data->cctor_finished_or_no_cctor = 1;
#else
    listClass._data->cctor_finished = 1;
#endif

    auto constructor = listClass.GetMethod(Internal::constructorName, 0)._data;

    auto newMethods = (IL2CPP::MethodInfo **) malloc(sizeof(IL2CPP::MethodInfo *) * listClass._data->method_count);
    memcpy(newMethods, listClass._data->methods, sizeof(IL2CPP::MethodInfo *) * listClass._data->method_count);

    auto newConstructor = (IL2CPP::MethodInfo *) malloc(sizeof(IL2CPP::MethodInfo));
    memcpy(newConstructor, constructor, sizeof(IL2CPP::MethodInfo));
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