#pragma once
#include <list>
#include <map>

struct monoString {
    void *klass;
    void *monitor;
    int length;
    char chars[255];

    std::string get_string() {
        if (isNOT_Allocated(chars))
            return OBFUSCATE_BNM("ERROR");
        std::string out;
        for (int i = 0; i < length * 2; i++){
            if (chars[i])
                out += chars[i];
        }
        return out;
    }

    std::string get_string_old() {
        if (isNOT_Allocated(chars))
            return OBFUSCATE_BNM("ERROR");
        std::u16string u16_string(reinterpret_cast<const char16_t *>(chars));
        std::wstring wide_string(u16_string.begin(), u16_string.end());
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
        return convert.to_bytes(wide_string);
    }

    enum CreateMethod {
        IL2CPP,
        MONO,
        C_LIKE
    };

    const char *get_const_char() {
        return get_string().c_str();
    }

    char *getChars() {
        return chars;
    }

    operator std::string(){
        return get_string();
    }

    operator const char *(){
        return get_string().c_str();
    }

    int getLength() {
        return length;
    }

};

template<typename T>
struct monoArray {
    void *klass;
    void *monitor;
    void *bounds;
    int32_t max_length;
    T m_Items[65535];

    int32_t getLength() {
        return max_length;
    }

    T *getPointer() {
        return m_Items;
    }
    std::vector<T> toCPPlist(){
        std::vector<T> ret;
        for (int i = 0; i < max_length; i++){
            ret.push_back(m_Items[i]);
        }
        return ret;
    }
};
template<typename T>
int32_t GetArraySize(monoArray<T> *array, bool unityObj){
    int32_t out = 0;
    bool check;
    if (unityObj)
        check = [](auto obj){
            return IsNativeObjectAlive(*(void **)obj);
        };
    else
        check = [](auto obj){
            return (*(void **)obj) != 0;
        };
    for (int32_t i = 0; i < array->max_length;i++){
        if (check(array[i])) out++;
    }
    return out;
}
template<typename T>
struct monoList {
    void *klass;
    void *monitor;
    monoArray<T> *Items;
    int32_t size;
    int32_t version;
    void *syncRoot;

    T* getItems() {
        return Items->getPointer();
    }

    int32_t getSize() {
        return size;
    }

    int32_t getVersion() {
        return version;
    }

    std::vector<T> toCPPlist(){
        std::vector<T> ret;
        for (int i = 0; i < size; i++){
            ret.push_back(Items->m_Items[i]);
        }
        return ret;
    }

    void Add(T val) {
        Items->m_Items[size] = val;
        size++;
        version++;
    }
};
uint GetHashCode(uint val){
    return -1640531535 * (val >> 2);
}
typedef struct Il2CppObject Il2CppObject;
template<typename TKey, typename TValue>
struct monoDictionary {
    void *klass;
    void *monitor;
    monoArray<int *> *buckets;
    monoArray<void *> *entries;
    int32_t count;
    int32_t version;
    int32_t freeList;
    int32_t freeCount;
    Il2CppObject *comparer;
    monoArray<TKey> *keys;
    monoArray<TValue> *values;
    void *syncRoot;

    TKey* getKeys() {
        return keys->getPointer();
    }

    TValue* getValues() {
        return values->getPointer();
    }

    void *getNumKeys() {
        return keys->getLength();
    }

    void *getNumValues() {
        return values->getLength();
    }

    int getSize() {
        return count;
    }
};

enum Mono_Method_attributes{
    IMPL_CODE_TYPE_MASK       = 0x0003,
    IMPL_IL                   = 0x0000,
    IMPL_NATIVE               = 0x0001,
    IMPL_OPTIL                = 0x0002,
    IMPL_RUNTIME              = 0x0003,

    IMPL_MANAGED_MASK         = 0x0004,
    IMPL_UNMANAGED            = 0x0004,
    IMPL_MANAGED              = 0x0000,

    IMPL_FORWARD_REF          = 0x0010,
    IMPL_PRESERVE_SIG         = 0x0080,
    IMPL_INTERNAL_CALL        = 0x1000,
    IMPL_SYNCHRONIZED         = 0x0020,
    IMPL_NOINLINING           = 0x0008,
    IMPL_MAX_METHOD_IMPL_VAL  = 0xffff,

    MEMBER_ACCESS_MASK        = 0x0007,
    COMPILER_CONTROLLED       = 0x0000,
    PRIVATE                   = 0x0001,
    FAM_AND_ASSEM             = 0x0002,
    ASSEM                     = 0x0003,
    FAMILY                    = 0x0004,
    FAM_OR_ASSEM              = 0x0005,
    PUBLIC                    = 0x0006,

    STATIC                    = 0x0010,
    FINAL                     = 0x0020,
    VIRTUAL                   = 0x0040,
    HIDE_BY_SIG               = 0x0080,

    VTABLE_LAYOUT_MASK        = 0x0100,
    REUSE_SLOT                = 0x0000,
    NEW_SLOT                  = 0x0100,

    STRICT                    = 0x0200,
    ABSTRACT                  = 0x0400,
    SPECIAL_NAME              = 0x0800,

    PINVOKE_IMPL              = 0x2000,
    UNMANAGED_EXPORT          = 0x0008,
};