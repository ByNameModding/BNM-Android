#include <jni.h>
#include "../ByNameModding/BNM.hpp"

void OnLoaded_Example_01();
void OnLoaded_Example_02();
void OnLoaded_Example_03();
void OnLoaded_Example_04();

//! BNM has many capabilities.
//! The most basic ones are described in the examples.
//! You can learn about the rest in BNM.hpp.

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);

    // Load BNM, finding the path to libil2cpp.so
    // When using TryForceLoadIl2CppByPath, it is advisable to uncomment BNM_DISABLE_AUTO_LOAD to slightly lighten the program
    BNM::TryForceLoadIl2CppByPath(env);

    BNM::AddOnLoadedEvent(OnLoaded_Example_01);
    BNM::AddOnLoadedEvent(OnLoaded_Example_02);
    BNM::AddOnLoadedEvent(OnLoaded_Example_03);
    BNM::AddOnLoadedEvent(OnLoaded_Example_04);

    return JNI_VERSION_1_6;
}
