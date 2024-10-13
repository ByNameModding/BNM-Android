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

namespace BNM::Coroutine {
    // Similar to UnityEngine.YieldInstruction, but for BNM
    struct YieldInstruction {
        inline YieldInstruction(BNM::IL2CPP::Il2CppObject *object) : _object(object) {};
        BNM::IL2CPP::Il2CppObject *_object{};
    protected:
        inline YieldInstruction() = default;
        friend struct IEnumerator;
    };
    struct AsyncOperation : YieldInstruction {
        explicit AsyncOperation(intptr_t operation);
    };
    struct WaitForEndOfFrame : YieldInstruction {
        WaitForEndOfFrame();
    };
    struct WaitForFixedUpdate : YieldInstruction {
        WaitForFixedUpdate();
    };
    struct WaitForSeconds : YieldInstruction {
        explicit WaitForSeconds(float seconds);
    };
    struct WaitForSecondsRealtime : YieldInstruction {
        explicit WaitForSecondsRealtime(float seconds);
    };
    struct WaitUntil : YieldInstruction {
        explicit WaitUntil(const std::function<bool()> &function) ;
    };
    struct WaitWhile : YieldInstruction {
        explicit WaitWhile(const std::function<bool()> &function);
    };

    struct _IEnumeratorInit;
    struct IEnumerator : BNM::IL2CPP::Il2CppObject {
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

        struct IEnumerator *get();
        inline operator struct IEnumerator *() { return get(); }

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

#endif
