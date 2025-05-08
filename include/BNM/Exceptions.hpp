#pragma once

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "BasicMonoStructures.hpp"

#include <functional>
#include <string>

// NOLINTBEGIN
namespace BNM {

    /**
        @brief Wrapper of Il2CppException
    */
    struct Exception {
        /**
            @brief Create empty exception.
        */
        inline constexpr Exception() = default;

        /**
            @brief Create exception from il2cpp exception.
            @param exception Il2cpp exception
        */
        inline constexpr Exception(BNM::IL2CPP::Il2CppException *exception) : _data(exception) {}

        /**
            @brief Get class name of exception.
            @return Class name of exception
        */
        [[nodiscard]] inline std::string ClassName() const { return ((BNM::Structures::Mono::String *)_data->className)->str(); }

        /**
            @brief Get message of exception.
            @return Message of exception
        */
        [[nodiscard]] inline std::string Message() const { return ((BNM::Structures::Mono::String *)_data->message)->str(); }

        /**
            @brief Get data of exception.
            @return Data of exception
        */
        [[nodiscard]] inline IL2CPP::Il2CppObject *Data() const { return _data->_data; }

        /**
            @brief Get Il2CppException.
            @return Il2CppException
        */
        [[nodiscard]] inline IL2CPP::Il2CppException *GetException() const { return _data; }

        /**
            @brief Check if exception is valid.
            @return State of exception
        */
        [[nodiscard]] inline bool IsValid() const { return _data; }

        BNM::IL2CPP::Il2CppException *_data{};
    };

    /**
        @brief Helper function for catching il2cpp errors.
        @param func Function that can throw il2cpp exception
        @return Valid exception if it's caught, otherwise empty exception.
    */
    Exception TryInvoke(const std::function<void()> &func);

    /// @cond
    inline void _TryInvokeWrapper(const std::function<void()> &baseCode, const std::function<void(Exception)> &catchCode = {}) {
        if (auto ex = TryInvoke(baseCode); ex.IsValid() && catchCode) catchCode(ex);
    }
    /// @endcond
}
// NOLINTEND

/**
    @defgroup EXCEPTIONS_MACRO Exceptions macro

    Macros for handling il2cpp's exceptions.

    @{
*/

/**
    @brief Define <em>danger</em> code block.
    @hideinitializer

    Macro that allows to define a block of code to be tested for exception while it is being executed.
*/
#define BNM_try BNM::_TryInvokeWrapper([&]() {

/**
    @brief Define block to handle exceptions.
    @hideinitializer

    Macro that allows to define a block of code to be executed, if an exception occurs in the try macro block.

    @param ex BNM::Exception object name
*/
#define BNM_catch(ex) }, [&](BNM::Exception ex) {

/**
    @brief End BNM_try or BNM_try&BNM_catch blocks
    @hideinitializer

    Should be always written after BNM_try code block or after BNM_catch block.
*/
#define BNM_end_try });

/** @} */