#pragma once
#include <list>
#include <map>
#include <cstdlib>
#include "monoConvert.h"


struct monoString {
    unsigned char data[sizeof(void *) * 2]; /*void *klass; and void *monitor;*/
    int length;
    monoChar chars[0];

    std::string get_string() {
        return get_const_char();
    }
    const char *get_const_char() {
        if (!this || !isAllocated(chars))
            return OBFUSCATE_BNM("ERROR");
        return monoConvert::utf16_to_utf8(chars, length);
    }
    std::string get_string_old() {
        if (!this || !isAllocated(chars))
            return OBFUSCATE_BNM("ERROR");
        try {
            return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(std::wstring(chars, chars + length));
        } catch(std::exception& e) {
            return OBFUSCATE_BNM("ERROR");
        }
    }
    std::string get_string_old_unsafe() {
        return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(std::wstring(chars, chars + length));
    }
    operator std::string(){
        return get_string();
    }
    operator const char *(){
        return get_const_char();
    }
    unsigned int getHash() {
        if (!this)
            return 0;
        monoChar* p = chars;
        unsigned int h = 0;
        for (int i = 0; i < length; i++)
        {
            h = (h << 5) - h + *p;
            p++;
        }
        return h;
    }
    static monoString *Create(std::string str){
        return Create(str2char(str));
    }
    static monoString *Create(const char *str){
        const size_t length = strlen(str);
        const size_t utf16Size = sizeof(monoChar) * length;
        monoString *ret = (monoString*)malloc(sizeof(monoString) + utf16Size);
        ret->length = length;
        memcpy(ret->chars, monoConvert::utf8_to_utf16(str, ret->length), utf16Size);
        return (monoString*)ret;
    }
    static monoString* Empty();
};

template<typename T>
struct monoArray {
    Il2CppObject obj;
    Il2CppArrayBounds *bounds;
    int32_t max_length;
    T m_Items[0];

    int32_t getLength() {
        return max_length;
    }

    T *getPointer() {
        return m_Items;
    }
    template<typename V = T>
    std::vector<V> toCPPlist(){
        std::vector<V> ret;
        for (int i = 0; i < max_length; i++){
            ret.push_back(m_Items[i]);
        }
        return ret;
    }
    bool copyFrom(std::vector<T> vec){
        return set(vec.data(), vec.size());
    }
    bool copyFrom(T *arr, int size){
        if (size > max_length) return false;
        memset((void *)m_Items, 0, sizeof(T) * max_length);
        for (int i = 0; i < size; i++)
            m_Items[i] = arr[i];
        return true;
    }
    T operator[] (int index) {
        return m_Items[index];
    }

};
template<typename T>
struct monoList {
    Il2CppObject obj;
    monoArray<T> *Items;
    int32_t size;
    int32_t version;

    T* getItems() {
        return Items->getPointer();
    }

    int32_t getSize() {
        return size;
    }

    int32_t getVersion() {
        return version;
    }
    template<typename V = T>
    std::vector<V> toCPPlist(){
        return Items->toCPPlist<V>();
    }

    void Add(T val) {
        Items->m_Items[size] = val;
        size++;
        version++;
    }
    T operator[] (int index) {
        return Items->m_Items[index];
    }
};

template<typename TKey, typename TValue>
struct monoDictionary {
    Il2CppObject obj;
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