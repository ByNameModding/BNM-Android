#pragma once

#include <jni.h>

#include "UserSettings/GlobalSettings.hpp"

namespace BNM::Loading {
    // Replace il2cpp::vm::Class::FromIl2CppType to load BNM if the download occurred too late.
    // !!!UNHOOK is required!!! Otherwise, the game will slow down.
    // Call BEFORE attempting to load BNM!
    void AllowedLateInitHook();

    // Use JNI and Context to find the full path to libil2cpp.so , and then replace the methods for loading BNM from the il2cpp stream
    bool TryLoadByJNI(JNIEnv *env, jobject context = nullptr);

    // Use the library handle to substitute methods for loading BNM from the il2cpp stream
    bool TryLoadByDlfcnHandle(void *handle);

    typedef void *(*MethodFinder)(const char *name, void *userData);
    // Set the data and method to search for il2cpp methods
    void SetMethodFinder(MethodFinder finderMethod, void *userData);

    // Use a custom method to hook methods for loading BNM from an il2cpp thread
    bool TryLoadByUsersFinder();

    // Use a custom method to load BNM from this stream
    // YOU CANNOT call the method until il2cpp is fully loaded!
    // ATTENTION, when using ClassesManagement, there may be problems (crashes) due to asynchronous data recording!
    void TrySetupByUsersFinder();

    void AddOnLoadedEvent(void (*event)());
    void ClearOnLoadedEvents();
}