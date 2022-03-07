#pragma once
#include <list>
#include <map>
#include <cstdlib>
#include <cmath>
#include "utf8.h"

namespace std {
    typedef std::basic_string<Il2CppChar> string16;
}

std::string Utf16ToUtf8(Il2CppChar* utf16String, size_t length) {
    std::string utf8String;
    utf8String.reserve(length);
    utf8::unchecked::utf16to8(utf16String, utf16String + length, std::back_inserter(utf8String));
    return utf8String;
}

std::string16 Utf8ToUtf16(const char* utf8String, size_t length) {
    std::string16 utf16String;
    if (utf8::is_valid(utf8String, utf8String + length)) {
        utf16String.reserve(length);
        utf8::unchecked::utf8to16(utf8String, utf8String + length, std::back_inserter(utf16String));
    }
    return utf16String;
}


struct monoString {
    Il2CppClass *klass;
    MonitorData *monitor;
    int length;
    Il2CppChar chars[0];

    std::string get_string() {
        if (!this)
            return OBFUSCATE_BNM("ERROR: monoString is null");
        if (!isAllocated(chars))
            return OBFUSCATE_BNM("ERROR: chars is null");
        return Utf16ToUtf8(chars, length);
    }
    
    const char *get_const_char() {
        return str2char(get_string());
    }

    const char *c_str() {
        return get_const_char();
    }

    std::string str() {
        return get_string();
    }

    std::string stro() {
        return get_string_old();
    }

    std::string get_string_old() {
        if (!this)
            return OBFUSCATE_BNM("ERROR: monoString is null");
        if (!isAllocated(chars))
            return OBFUSCATE_BNM("ERROR: chars is null");
        return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(std::wstring(chars, chars + length));
    }

    operator std::string() {
        return get_string();
    }
    
    operator const char *() {
        return get_const_char();
    }
    
    unsigned int getHash() {
        if (!this)
            return 0;
        Il2CppChar* p = chars;
        unsigned int h = 0;
        for (int i = 0; i < length; i++) {
            h = (h << 5) - h + *p; p++;
        }
        return h;
    }
    
    static monoString *Create(std::string str) {
        return Create(str2char(str));
    }
    
    static monoString *Create(const char *str) {
        const size_t length = strlen(str);
        const size_t utf16Size = sizeof(Il2CppChar) * length;
        monoString *ret = (monoString*)malloc(sizeof(monoString) + utf16Size);
        ret->length = length;
        std::string16 u16 = Utf8ToUtf16(str, ret->length);
        memcpy(ret->chars, &u16[0], utf16Size);
        u16.clear();
        return (monoString*)ret;
    }
    
    static monoString* Empty();
};

template<typename T>
struct monoArray {
    Il2CppClass *klass;
    MonitorData *monitor;
    Il2CppArrayBounds *bounds;
    int capacity;
    T m_Items[0];

    int getCapacity() {
        return capacity;
    }

    T *getPointer() {
        return m_Items;
    }
    
    template<typename V = T>
    std::vector<V> toCPPlist() {
        std::vector<V> ret;
        for (int i = 0; i < capacity; i++)
            ret.push_back(m_Items[i]);
        return std::move(ret);
    }

    void copyFrom(std::vector<T> vec) {
        copyFrom(vec.data(), vec.size());
    }
    
    void copyFrom(T *arr, int size) {
        if (size > capacity)
            Resize(size);
        memcpy(arr, m_Items, capacity * sizeof(T));
    }

    void copyTo(T *arr) {
        memcpy(arr, m_Items, sizeof(T) * capacity);
    }
    
    T operator[] (int index) {
        return m_Items[index];
    }

    void Resize(int newSize) {
        if (newSize <= capacity) return;
        T* newArr = new T[newSize];
        memcpy(newArr, m_Items, capacity * sizeof(T));
        capacity = newSize;
        delete [] m_Items;
        m_Items = newArr;
    }

    template<typename t>
    static monoArray<t> *Create(int size) {
        monoArray<t> *monoArr = (monoArray<t> *)malloc(sizeof(monoArray) + sizeof(t) * size);
        monoArr->max_length = size;
        return monoArr;
    }
    
    template<typename t>
    static monoArray<t> *Create(std::vector<t> vec) {
        return Create<t>(vec.data(), vec.size());
    }
    
    template<typename t>
    static monoArray<t> *Create(T *arr, int size) {
        monoArray<t> *monoArr = Create<t>(size);
        for (int i = 0; i < size; i++)
            monoArr->m_Items[i] = arr[i];
        return monoArr;
    }

};
template<typename T>
struct monoList {
    Il2CppClass *klass;
    MonitorData *monitor;
    monoArray<T> *items;
    int size;
    int version;

    T* getItems() {
        return items->getPointer();
    }

    int getSize() {
        return size;
    }

    int getVersion() {
        return version;
    }
    
    template<typename V = T>
    std::vector<V> toCPPlist() {
        std::vector<V> ret;
        for (int i = 0; i < size; i++)
            ret.push_back(getItems()[i]);
        return std::move(ret);
    }
    
    void Add(T val) {
        GrowIfNeeded(1);
        items->m_Items[size] = val;
        size++;
        version++;
    }
    
    int IndexOf(T val) {
        for (int i = 0; i < size; i++) {
            if (items->m_Items[i] == val)
                return i;
        }
        return -1;
    }
    
    void RemoveAt(int index) {
        if (index != -1) {
            Shift(index, -1);
            version++;
        }

    }
    
    void Remove(T val) {
        RemoveAt(IndexOf(val));
    }

    T operator[] (int index) {
        return items->m_Items[index];
    }
    
    void Resize(int newSize) {
        items->Resize(newSize);
    }
private:
    void GrowIfNeeded(int n) {
        if (size + n > items->max_length) {
            Resize(size + n);
        }
    }

    void Shift(int start, int delta) {
        if (delta < 0)
            start -= delta;
        if (start < size)
            memcpy(items + start + delta, items + start, size - start);
        size += delta;
        if (delta < 0)
            memset(items + size + delta, 0, -delta * sizeof(T));
    }
};

namespace NET4x {
    template<typename TKey, typename TValue>
    struct monoDictionary {
        struct Entry {
            int hashCode, next;
            TKey key;
            TValue value;
        };
        Il2CppClass *klass;
        MonitorData *monitor;
        monoArray<int> *buckets;
        monoArray<Entry> *entries;
        int count;
        int version;
        int freeList;
        int freeCount;
        void *comparer;
        monoArray<TKey> *keys;
        monoArray<TValue> *values;
        void *syncRoot;

        std::map<TKey, TValue> toMap() {
            std::map<TKey, TValue> ret;
            auto lst = entries->template toCPPlist();
            for (auto enter : lst)
                ret.insert(std::make_pair(enter.key, enter.value));
            return std::move(ret);
        }

        std::vector<TKey> getKeys() {
            std::vector<TKey> ret;
            auto lst = entries->template toCPPlist();
            for (auto enter : lst)
                ret.push_back(enter.key);
            return std::move(ret);
        }

        std::vector<TValue> getValues() {
            std::vector<TValue> ret;
            auto lst = entries->template toCPPlist();
            for (auto enter : lst)
                ret.push_back(enter.value);
            return std::move(ret);
        }

        int getSize() {
            return count;
        }

        int getVersion() {
            return version;
        }

        bool TryGet(TKey key, TValue &value);
        void Add(TKey key, TValue value);
        void Insert(TKey key, TValue value);
        bool Remove(TKey key);
        bool ContainsKey(TKey key);
        bool ContainsValue(TValue value);

        TValue Get(TKey key) {
            TValue ret;
            if (TryGet(key, ret))
                return ret;
            return {};
        }

        TValue operator [](TKey key) {
            return Get(key);
        }
    };
}

namespace NET35 {
    template<typename TKey, typename TValue>
    struct monoDictionary {
        struct Link { int HashCode, Next; };
        Il2CppClass *klass;
        MonitorData *monitor;
        monoArray<int> *table;
        monoArray<Link> *linkSlots;
        monoArray<TKey> *keys;
        monoArray<TValue> *values;
        int touchedSlots;
        int emptySlot;
        int count;
        int threshold;
        void *hcp;
        void *serialization_info;
        int generation;

        std::map<TKey, TValue> toMap() {
            std::map<TKey, TValue> ret;
            for (int i = 0; i < touchedSlots; i++)
                if (((*linkSlots)[i].HashCode & -2147483648) != 0)
                    ret.insert(std::make_pair((*keys)[i], (*values)[i]));
            return std::move(ret);
        }

        std::vector<TKey> getKeys() {
            std::vector<TKey> ret;
            for (int i = 0; i < touchedSlots; i++)
                if (((*linkSlots)[i].HashCode & -2147483648) != 0)
                    ret.push_back((*keys)[i]);
            return std::move(ret);
        }

        std::vector<TValue> getValues() {
            std::vector<TValue> ret;
            for (int i = 0; i < touchedSlots; i++)
                if (((*linkSlots)[i].HashCode & -2147483648) != 0)
                    ret.push_back((*values)[i]);
            return std::move(ret);
        }

        int getSize() {
            return count;
        }

        int getVersion() {
            return generation;
        }

        TValue Get(TKey key) {
            return ((NET4x::monoDictionary<TKey, TValue> *)this)->Get(key);
        }

        bool TryGet(TKey key, TValue &value) {
            return ((NET4x::monoDictionary<TKey, TValue> *)this)->TryGet(key, value);
        }

        void Add(TKey key, TValue value) {
            return ((NET4x::monoDictionary<TKey, TValue> *)this)->Add(key, value);
        }

        bool Remove(TKey key) {
            return ((NET4x::monoDictionary<TKey, TValue> *)this)->Remove(key);
        }

        bool ContainsKey(TKey key) {
            return ((NET4x::monoDictionary<TKey, TValue> *)this)->ContainsKey(key);
        }

        bool ContainsValue(TValue value) {
            return ((NET4x::monoDictionary<TKey, TValue> *)this)->ContainsValue(value);
        }

        TValue operator [](TKey key) {
            return Get(key);

        }
    };
}


enum Mono_Method_attributes {
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