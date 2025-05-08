#pragma once

#include <type_traits>

#include "Field.hpp"
#include "Method.hpp"
#include "Property.hpp"
#include "Event.hpp"

/**
    @brief Namespace that holds operators which may help with methods, events, fields and properties.
*/
namespace BNM::Operators {
    template<typename P, typename T, typename = std::enable_if<std::is_pointer_v<P>>>
    inline Utils::DataIterator<T> operator ->*(const P pointer, Field<T> &field) {
        field[(IL2CPP::Il2CppObject *)pointer];
        return field.GetPointer();
    }
    template<typename P, typename T, typename = std::enable_if<std::is_pointer_v<P>>>
    inline Method<T> &operator ->*(const P pointer, Method<T> &method) {
        method[(IL2CPP::Il2CppObject *)pointer];
        return method;
    }
    template<typename P, typename T, typename = std::enable_if<std::is_pointer_v<P>>>
    inline Property<T> &operator ->*(const P pointer, Property<T> &property) {
        property[(IL2CPP::Il2CppObject *)pointer];
        return property;
    }
    template<typename P, typename T, typename = std::enable_if<std::is_pointer_v<P>>>
    inline Event<T> &operator ->*(const P pointer, Event<T> &event) {
        event[(IL2CPP::Il2CppObject *)pointer];
        return event;
    }
    template<typename P, typename T, typename = std::enable_if<std::is_pointer_v<P>>>
    inline Field<T> &operator >>(const P pointer, Field<T> &field) {
        field[(IL2CPP::Il2CppObject *)pointer];
        return field;
    }
    template<typename P, typename T, typename = std::enable_if<std::is_pointer_v<P>>>
    inline Method<T> &operator >>(const P pointer, Method<T> &method) {
        method[(IL2CPP::Il2CppObject *)pointer];
        return method;
    }
    template<typename P, typename T, typename = std::enable_if<std::is_pointer_v<P>>>
    inline Property<T> &operator >>(const P pointer, Property<T> &property) {
        property[(IL2CPP::Il2CppObject *)pointer];
        return property;
    }
    template<typename P, typename T, typename = std::enable_if<std::is_pointer_v<P>>>
    inline Event<T> &operator >>(const P pointer, Event<T> &event) {
        event[(IL2CPP::Il2CppObject *)pointer];
        return event;
    }
}