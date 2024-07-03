#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "BasicMonoStructures.hpp"

#include <functional>
#include <string>

namespace BNM {
    struct Exception {
        inline constexpr Exception() = default;
        inline constexpr Exception(BNM::IL2CPP::Il2CppException *_data) : _data(_data) {}

        [[nodiscard]] inline std::string ClassName() { return ((BNM::Structures::Mono::String *)_data->className)->str(); }
        [[nodiscard]] inline std::string Message() { return ((BNM::Structures::Mono::String *)_data->message)->str(); }
        [[nodiscard]] inline IL2CPP::Il2CppObject *Data() { return _data->_data; }
        [[nodiscard]] inline IL2CPP::Il2CppException *GetException() { return _data; }

        [[nodiscard]] inline bool IsValid() { return _data; }

        BNM::IL2CPP::Il2CppException *_data{};
    };

    Exception TryInvoke(const std::function<void()> &func);
    inline void _TryInvokeWrapper(const std::function<void()> &baseCode, const std::function<void(Exception)> &catchCode = {}) {
        if (auto ex = TryInvoke(baseCode); ex.IsValid() && catchCode) catchCode(ex);
    }
}

#define BNM_try BNM::_TryInvokeWrapper([&]() {
#define BNM_catch(ex) }, [&](BNM::Exception ex) {
#define BNM_end_try });