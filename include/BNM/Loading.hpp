#pragma once

#include <jni.h>

#include "UserSettings/GlobalSettings.hpp"

/**
    @brief Namespace with all methods for loading BNM.
*/
namespace BNM::Loading {
    /**
        @brief Allow BNM to load at any game state.

        Allow BNM to hook il2cpp::vm::Class::FromIl2CppType to load itself if loading starts too late.
        Should be called before attempting to load BNM. (Before @ref TryLoadByJNI, @ref TryLoadByDlfcnHandle, @ref TryLoadByUsersFinder and @ref TrySetupByUsersFinder).

        @warning For this loading method Unhook is required. Otherwise, the game will be slowed down.
    */
    void AllowLateInitHook();

    /**
        @brief Try load BNM using JNI.

        Uses JNI and Context to find the full path to libil2cpp.so and then hook methods for loading BNM from the il2cpp thread.

        @param env Valid in current thread JNIEnv
        @param context Target app's android.content.Context

        @return True if library is found and hooks are setup.

        Example usage:
        @code{.cpp}
        JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
            // ...
            JNIEnv *env;
            vm->GetEnv((void **) &env, JNI_VERSION_1_6);
            // ...
            BNM::Loading::TryLoadByJNI(env);
            // ...
        }
        @endcode
    */
    bool TryLoadByJNI(JNIEnv *env, jobject context = nullptr);

    /**
        @brief Try load BNM using dlfcn handle.

        Uses the library handle to hook methods for loading BNM from the il2cpp thread.

        @return True if hooks are setup.

        Example usage:
        @code{.cpp}
        void method() {
            // ...
            auto handle = BNM_dlopen(path_to_libil2cpp, RTLD_LAZY);
            // ...
            BNM::Loading::TryLoadByDlfcnHandle(handle);
            // ...
        }
        @endcode
    */
    bool TryLoadByDlfcnHandle(void *handle);

    typedef void *(*MethodFinder)(const char *name, void *userData);

    /**
        @brief Set custom method finder.

        You can use it for setting custom method finder if dlfcn or JNI don't work in current context.

        @param finderMethod Pointer to custom method finder
        @param userData Data that will be passed with every custom method finder call
    */
    void SetMethodFinder(MethodFinder finderMethod, void *userData);

    /**
        @brief Try load BNM using custom method finder.

        Uses custom method finder to hook methods for loading BNM from the il2cpp thread.

        @return True if hooks are setup.
    */
    bool TryLoadByUsersFinder();

    /**
       @brief Try setup BNM using custom method finder.

       Uses custom method finder to load BNM from the current thread.

       @warning You cannot call the method until il2cpp is fully loaded. It will cause crash.
       @warning Highly recommended not to call it from thread other than il2cpp's, it can cause a lot of problems and crashes, especially if classes management is enabled.
   */
    void TrySetupByUsersFinder();

    /**
        @brief Add event that will be called from il2cpp thread when il2cpp and BNM will be fully loaded.
    */
    void AddOnLoadedEvent(void (*event)());

    /**
        @brief Remove all events.
    */
    void ClearOnLoadedEvents();
}
