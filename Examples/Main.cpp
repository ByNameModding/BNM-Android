#include <jni.h>
#include "../ByNameModding/BNM.hpp"

//! У BNM много возможностей.
//! Самые основные описаны в примерах.
//! Остальные вы можете узнать в BNM.hpp.

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);

    // Загрузить BNM, найдя путь до libil2cpp.so
    // При использовании TryForceLoadIl2CppByPath, желательно раскоментировать BNM_DISABLE_AUTO_LOAD, чтобы слегка облегчить программу
    BNM::TryForceLoadIl2CppByPath(env);

    return JNI_VERSION_1_6;
}