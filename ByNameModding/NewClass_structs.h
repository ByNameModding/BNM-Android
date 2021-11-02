#pragma once
const Il2CppType* TypeFinder::ToIl2CppType(){
    if (byNameOnly){
        return LoadClass(OBFUSCATE_BNM("System"), name).GetIl2CppType();
    } else {
        return LoadClass(namespaze, name).GetIl2CppType();
    }
}
// Get Il2Cpp Mono type name at compile time
template<typename T>
constexpr TypeFinder GetType(){
    if constexpr (std::is_same_v<T, void>){
        return {true, OBFUSCATE_BNM("Void")};
    } else if constexpr (std::is_same_v<T, bool>){
        return {true, OBFUSCATE_BNM("Boolean")};
    } else if constexpr (std::is_same_v<T, uint8_t>){
        return {true, OBFUSCATE_BNM("Byte")};
    } else if constexpr (std::is_same_v<T, int8_t>){
        return {true, OBFUSCATE_BNM("SByte")};
    } else if constexpr (std::is_same_v<T, int16_t>){
        return {true, OBFUSCATE_BNM("Int16")};
    } else if constexpr (std::is_same_v<T, uint16_t>){
        return {true, OBFUSCATE_BNM("UInt16")};
    } else if constexpr (std::is_same_v<T, int32_t>){
        return {true, OBFUSCATE_BNM("Int32")};
    } else if constexpr (std::is_same_v<T, uint32_t>){
        return {true, OBFUSCATE_BNM("UInt32")};
    } else if constexpr (std::is_same_v<T, intptr_t>){
        return {true, OBFUSCATE_BNM("IntPtr")};
    } else if constexpr (std::is_same_v<T, int64_t>){
        return {true, OBFUSCATE_BNM("Int64")};
    } else if constexpr (std::is_same_v<T, uint64_t>){
        return {true, OBFUSCATE_BNM("UInt64")};
    } else if constexpr (std::is_same_v<T, float>){
        return {true, OBFUSCATE_BNM("Single")};
    } else if constexpr (std::is_same_v<T, double>){
        return {true, OBFUSCATE_BNM("Double")};
    } else if constexpr (std::is_same_v<T, Il2CppString *> || std::is_same_v<T, monoString *>){
        return {true, OBFUSCATE_BNM("String")};
    } else if constexpr (std::is_same_v<T, Vector3> || std::is_same_v<T, IVector3>){
        return {false, OBFUSCATE_BNM("Vector3"), OBFUSCATE_BNM("UnityEngine")};
    } else if constexpr (std::is_same_v<T, Vector2> || std::is_same_v<T, IVector2>){
        return {false, OBFUSCATE_BNM("Vector2"), OBFUSCATE_BNM("UnityEngine")};
    } else if constexpr (std::is_same_v<T, Color>){
        return {false, OBFUSCATE_BNM("Color"), OBFUSCATE_BNM("UnityEngine")};
    } else if constexpr (std::is_same_v<T, Ray>){
        return {false, OBFUSCATE_BNM("Ray"), OBFUSCATE_BNM("UnityEngine")};
    } else if constexpr (std::is_same_v<T, RaycastHit>){
        return {false, OBFUSCATE_BNM("RaycastHit"), OBFUSCATE_BNM("UnityEngine")};
    } else {
        return {true, OBFUSCATE_BNM("Object")};
    }
}

struct NewMethod {
    MethodInfo *thizMethod;
    const char* Name;
    const char* GetName() {return Name;};
    const MethodInfo *virtualMethod;
    TypeFinder ret_type;
    std::vector<TypeFinder> *args_types;
    bool statik = false;
};

struct NewField {
    const char* Name;
    const char* GetName() {return Name;};
    FieldInfo *thizField;
    int32_t offset;
    int32_t size;
    unsigned int attributes : 16;
    TypeFinder MYtype;
    int32_t cppOffset;
};
struct NewClass {
    size_t size;
    Il2CppClass *thizClass;
    const char* NameSapce;
    const char* Name;
    const char* BaseNameSapce;
    const char* BaseName;
    const char* GetNameSapce() {return NameSapce;};
    const char* GetName() {return Name;};
    const char* GetBaseNameSapce() {return BaseNameSapce;};
    const char* GetBaseName() {return BaseName;};
    const char* GetDllName() {return OBFUSCATE_BNM("ByNameModding");};
    std::vector<NewMethod *> *Methods4Add;
    std::vector<NewField *> *Fields4Add;
    std::vector<NewField *> *StaticFields4Add;
    std::vector<Il2CppClass *> Interfaces;
    size_t staticFieldOffset = 0x0;
    size_t staticFieldsAdress;
    void AddNewField(NewField *field, bool statik = false){
        if (!statik) {
            if (!Fields4Add)
                Fields4Add = new std::vector<NewField *>();
            Fields4Add->push_back(field);
        } else {
            if (staticFieldOffset == 0x0){
                staticFieldsAdress = field->cppOffset;
            }
            if (!StaticFields4Add)
                StaticFields4Add = new std::vector<NewField *>();
            field->offset = staticFieldOffset;
            staticFieldOffset += field->size;
            StaticFields4Add->push_back(field);
        }

    }
    void AddNewMethod(NewMethod *method) {
        if (!Methods4Add)
            Methods4Add = new std::vector<NewMethod *>();
        Methods4Add->push_back(method);
    }
};
template<typename T> struct type {};
template<typename>
struct GetNewMethodCalls {};
template<typename RetT, typename T, typename ...ArgsT>
struct GetNewMethodCalls<RetT(T::*)(ArgsT...)> {
    template<typename Q>
    static inline Q UnpackArg(void* arg, type<Q>) {
        if constexpr (std::is_pointer_v<Q>) {
            return reinterpret_cast<Q>(arg);
        } else {
            return *reinterpret_cast<Q *>(arg);
        }
    }
public:
    template<RetT(T::* member)(ArgsT...)>
    static inline RetT get(T* self, ArgsT ...args) {
        return (self->*member)(args...);
    }
    template<std::size_t ...As>
    static void* InvokeMethod(RetT(*func)(T*, ArgsT...), T* instance, void** args, std::index_sequence<As...>) {
        if constexpr (std::is_same_v<RetT, void>) {
            func(instance, UnpackArg(args[As], type<ArgsT>{})...);
            return nullptr;
        } else {
            return (void*)func(instance, UnpackArg(args[As], type<ArgsT>{})...);
        }
    }
    static void* invoke(Il2CppMethodPointer ptr, const MethodInfo* m, void* obj, void** args) {
        auto func = (RetT(*)(T*, ArgsT...))(ptr);
        auto instance = (T*)(obj);
        std::index_sequence<sizeof...(ArgsT)> a;
        auto seq = std::make_index_sequence<sizeof...(ArgsT)>();
        return InvokeMethod(func, instance, args, seq);
    }
};
template<typename>
struct GetNewStaticMethodCalls {};
template<typename RetT, typename ...ArgsT>
struct GetNewStaticMethodCalls<RetT(*)(ArgsT...)> {
    template<typename T>
    static inline T UnpackArg(void* arg) {
        if constexpr (std::is_pointer_v<T>) {
            return (T)(arg);
        } else {
            return *(T*)(arg);
        }
    }
public:
    template<std::size_t ...As>
    static void* InvokeMethod(RetT(*func)(ArgsT...), void** args, std::index_sequence<As...>) {
        if constexpr (std::is_same_v<RetT, void>) {
            func(UnpackArg<ArgsT>(args[As])...);
            return nullptr;
        } else {
            return func(UnpackArg<ArgsT>(args[As])...);
        }
    }
    static void* invoke(Il2CppMethodPointer ptr, const MethodInfo* m, void* obj, void** args) {
        auto func = (RetT(*)(ArgsT...))(ptr);
        std::index_sequence<sizeof...(ArgsT)> a;
        auto seq = std::make_index_sequence<sizeof...(ArgsT)>();
        return InvokeMethod(func, args, seq);
    }
};
