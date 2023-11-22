#include <jni.h>
#include "../ByNameModding/BNM.hpp"

//! BNM has many capabilities.
//! The most basic ones are described in the examples.
//! You can learn about the rest in BNM.hpp.

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);

    // Load BNM, finding the path to libil2cpp.so
    // When using TryForceLoadIl2CppByPath, it is advisable to uncomment BNM_DISABLE_AUTO_LOAD to slightly lighten the program
    BNM::TryForceLoadIl2CppByPath(env);

    return JNI_VERSION_1_6;
}
