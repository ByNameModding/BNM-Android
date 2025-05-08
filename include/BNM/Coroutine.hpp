#pragma once

#include "UserSettings/GlobalSettings.hpp"

#if defined(BNM_CLASSES_MANAGEMENT) && defined(BNM_COROUTINE)

#include <functional>

#if __has_include(<coroutine>)
#include <coroutine>
#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
namespace std {
    template<typename T>
    using coroutine_handle = std::experimental::coroutine_handle<T>;
    using suspend_always = std::experimental::suspend_always;
    template <typename Ret, typename... Args>
    using coroutine_traits = std::experimental::coroutine_traits<Ret, Args...>;
}
#endif

#include "Il2CppHeaders.hpp"

// NOLINTBEGIN
/**
    @brief Namespace that has types to create Unity coroutines.
*/
namespace BNM::Coroutine {

    /**
        @brief Class similar to UnityEngine.YieldInstruction, but not it's implementation.

        It's used as base class for custom BNM based YieldInstruction.
    */
    struct YieldInstruction {
        inline YieldInstruction(BNM::IL2CPP::Il2CppObject *object) : _object(object) {};
        BNM::IL2CPP::Il2CppObject *_object{};
    protected:
        inline YieldInstruction() = default;
        friend struct IEnumerator;
    };

    /**
        @brief BNM's custom wrapper for UnityEngine.AsyncOperation.
    */
    struct AsyncOperation : YieldInstruction {
        explicit AsyncOperation(intptr_t operation);
    };

    /**
        @brief BNM's custom wrapper for UnityEngine.WaitForEndOfFrame.
    */
    struct WaitForEndOfFrame : YieldInstruction {
        WaitForEndOfFrame();
    };

    /**
        @brief BNM's custom wrapper for UnityEngine.WaitForFixedUpdate.
    */
    struct WaitForFixedUpdate : YieldInstruction {
        WaitForFixedUpdate();
    };

    /**
        @brief BNM's custom wrapper for UnityEngine.WaitForSeconds.
    */
    struct WaitForSeconds : YieldInstruction {
        explicit WaitForSeconds(float seconds);
    };

    /**
        @brief BNM's custom wrapper for UnityEngine.WaitForSecondsRealtime.
    */
    struct WaitForSecondsRealtime : YieldInstruction {
        explicit WaitForSecondsRealtime(float seconds);
    };

    /**
        @brief BNM's custom implementation of UnityEngine.WaitUntil.
    */
    struct WaitUntil : YieldInstruction {
        explicit WaitUntil(const std::function<bool()> &function) ;
    };

    /**
        @brief BNM's custom implementation of UnityEngine.WaitUntil.
    */
    struct WaitWhile : YieldInstruction {
        explicit WaitWhile(const std::function<bool()> &function);
    };

    struct _IEnumeratorInit;

    /**
        @brief Analog of C# IEnumerator that is based on C++20 coroutines to emulate work of Unity's coroutines.
    */
    struct IEnumerator : BNM::IL2CPP::Il2CppObject {

        /// @cond
        struct promise_type {
            Coroutine::YieldInstruction _currentValue{};
            inline IEnumerator get_return_object() { return IEnumerator(std::coroutine_handle<promise_type>::from_promise(*this)); }
            inline std::suspend_always initial_suspend() noexcept { return {}; }
            inline std::suspend_always final_suspend() noexcept { return {}; }
            inline void unhandled_exception() {}
            inline std::suspend_always await_transform() = delete;
            [[nodiscard]] inline Coroutine::YieldInstruction value() const noexcept { return _currentValue; }
            inline std::suspend_always yield_value(const Coroutine::YieldInstruction &val) { _currentValue = val; return {}; }
            inline void return_void() {}
        };
        /// @endcond

        /**
            @brief Create IEnumerator that can be passed to Unity.
        */
        struct IEnumerator *Get();

        /**
            @brief Alias of Get().
        */
        inline struct IEnumerator * operator()() { return Get(); }

        /**
            @brief Alias of Get().
        */
        inline operator struct IEnumerator *() { return Get(); }

    private:
        friend struct _IEnumeratorInit;
        void Finalize();
        bool MoveNext();
        void Reset();
        Il2CppObject *Current();
        friend struct IEnumerator;
        explicit IEnumerator(std::coroutine_handle<promise_type> handle) : BNM::IL2CPP::Il2CppObject(), _coroutine(handle) {}
        inline constexpr IEnumerator() : BNM::IL2CPP::Il2CppObject() {}
        Il2CppObject *_current{};
        std::coroutine_handle<promise_type> _coroutine{};
    };
}
// NOLINTEND

#endif
