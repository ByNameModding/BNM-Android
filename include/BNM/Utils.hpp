#pragma once

#include <csetjmp>

#include <string_view>
#include <type_traits>

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"

namespace BNM {
#if __DOXYGEN__
#define NO_INLINE
#else
#define NO_INLINE __attribute__((noinline))
#endif

    /**
        @brief Macro function for checking pointer for null. Due to noinline attribute allows to check even `this` of objects.
    */
    template <typename T>
    NO_INLINE bool CheckForNull(T obj) { return (void *) obj; }

    /**
        @brief Macro function for checking if pointer points to valid address.
        @return True if address is valid.
    */
    template <typename T, typename = std::enable_if<std::is_pointer_v<T>>>
    inline bool IsAllocated(T x) {
#ifdef BNM_ALLOW_SAFE_IS_ALLOCATED
        if (!x) return false;

        static jmp_buf jump;
        static sighandler_t handler = [](int) { longjmp(jump, 1); }; // NOLINT
        char c;
        bool ok = true;
        sighandler_t old_handler = signal(SIGSEGV, handler);
        if (!setjmp (jump)) c = *(char *) x; else ok = false; // NOLINT
        (void)c;
        signal(SIGSEGV, old_handler);
        return ok;
#else
        return x != nullptr;
#endif
    }

    namespace Structures::Mono { struct String; }

    /**
        @brief Macro function for creating C# strings (BNM::Structures::Mono::String).
        @return New mono string object
    */
    Structures::Mono::String *CreateMonoString(const std::string_view &str);

    /**
        @brief Macro function for getting external methods (icall).
        @return Pointer to extern method if it's found, otherwise null.
    */
    void *GetExternMethod(const std::string_view &str);

    /**
        @brief Check if BNM and il2cpp are loaded.
        @return True if BNM and il2cpp are loaded.
    */
    bool IsLoaded();

    /**
        @brief Get handle of libil2cpp.so if it's used by BNM.
        @return Dlfcn handle of libil2cpp.so if it's used by BNM and BNM is loaded, otherwise null.
    */
    void *GetIl2CppLibraryHandle();

    /**
        @brief Unbox any object.
        @return Unboxed object of passed type
    */
    template<typename T>
    inline T UnboxObject(T obj) { return (T)(void *)(((char *)obj) + sizeof(BNM::IL2CPP::Il2CppObject)); }

#ifdef BNM_DEPRECATED
    template <typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    inline T CheckObj(T obj) {
        if (obj && IsAllocated(obj)) return obj;
        return nullptr;
    }

    template<typename MET_T, typename PTR_T>
    inline void InitFunc(MET_T& method, PTR_T ptr) {
        *(void **)&method = (void *)ptr;
    }
#endif

    /// @cond
    // For thread static adn const fields
    namespace PRIVATE_FieldUtils {
        void GetStaticValue(IL2CPP::FieldInfo *info, void *value);
        void SetStaticValue(IL2CPP::FieldInfo *info, void *value);
    }
    /// @endcond

    struct CompileTimeClass;
#ifdef BNM_DEBUG
    /**
        @brief BNM namespace with utils
    */
    namespace Utils {
        void *OffsetInLib(void *);
        void LogCompileTimeClass(const BNM::CompileTimeClass &compileTimeClass);
    }
#endif

    /**
        @brief Attach current thread to il2cpp VM.
        @return True if thread was attached.
    */
    bool AttachIl2Cpp();

    /**
        @brief Get current thread attached to il2cpp VM.
        @return IL2CPP::Il2CppThread if current tread is attached to VM, otherwise null.
    */
    IL2CPP::Il2CppThread *CurrentIl2CppThread();

    /**
        @brief Detach current thread from il2cpp VM.
    */
    void DetachIl2Cpp();

    /**
        @brief Allocates memory that is registered in il2cpp's GC.
        @return Allocated memory
    */
    void *Allocate(size_t size);

    /**
        @brief Frees memory that was allocated by il2cpp.
    */
    void Free(void *);

#if UNITY_VER >= 232
    /**
        @brief Unmarshals unity object.
        @return Unmarshaled unity object
    */
    inline BNM_INT_PTR UnmarshalUnityObject(BNM_INT_PTR gcHandlePtr) {
        if (!gcHandlePtr) return 0;
        return *(BNM_INT_PTR *)(uintptr_t)(gcHandlePtr & ~(BNM_INT_PTR)1);
    }
#endif

    /// @cond
    namespace PRIVATE_INTERNAL {
        template<typename T>
        inline T ReturnEmpty() { if constexpr (std::is_same_v<T, void>) return; else return {}; }

        inline IL2CPP::Il2CppClass *&GetMethodClass(IL2CPP::MethodInfo *methodInfo) {
#if UNITY_VER > 174
#define kls klass
#else
#define kls declaring_type
#endif
            return methodInfo->kls;
#undef kls
        }
    }
    /// @endcond

    /**
        @brief BNM's custom forward list implementation

        @warning Don't use it anywhere! Internal Only

        Almost same as std::forward_list, but works how BNM needs to.
        Has only pointer to last element, but elements are looped, so last element points to first one.
        It used internally, so it lacks a lot of C++ stuff like foreach.
    */
    template<typename T>
    struct ForwardList {
        struct Element {
            Element *next{};
            T value{};
        };

        Element *lastElement{};

        inline ForwardList() = default;

        inline ~ForwardList() { Clear(); }

        inline ForwardList(const ForwardList& other) : lastElement(nullptr) {
            if (other.IsEmpty()) return;

            auto currentOther = other.lastElement->next;
            do {
                Add(currentOther->value);
                currentOther = currentOther->next;
            } while (currentOther != other.lastElement->next);
        }

        inline ForwardList& operator=(const ForwardList& other) {
            if (this == &other) return *this;

            Clear();

            if (other.IsEmpty()) return *this;

            auto currentOther = other.lastElement->next;
            do {
                Add(currentOther->value);
                currentOther = currentOther->next;
            } while (currentOther != other.lastElement->next);

            return *this;
        }

        [[nodiscard]] inline bool IsEmpty() const { return !lastElement; }

        inline void Add(T value) {
            auto newElement = (Element *) BNM_malloc(sizeof(Element));
            newElement->next = nullptr;
            newElement->value = value;

            if (IsEmpty()) {
                newElement->next = newElement;
                lastElement = newElement;
            } else {
                newElement->next = lastElement->next;
                lastElement->next = newElement;
                lastElement = newElement;
            }
        }

        inline void Clear(void (*onElementFreed)(T value) = nullptr) {
            if (IsEmpty()) return;

            auto current = lastElement->next;
            lastElement->next = nullptr;

            while (current) {
                auto next = current->next;
                if (onElementFreed) onElementFreed(current->value);
                BNM_free(current);
                current = next;
            }

            lastElement = nullptr;
        }
    };
}