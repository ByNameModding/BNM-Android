#include <jni.h>
#include "../ByNameModding/BNM.hpp"

void OnLoaded_Example_01();
void OnLoaded_Example_02();
void OnLoaded_Example_03();
void OnLoaded_Example_04();

//! У BNM много возможностей.
//! Самые основные описаны в примерах.
//! Остальные вы можете узнать в BNM.hpp.

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);

    // Загрузить BNM, найдя путь до libil2cpp.so
    BNM::TryForceLoadIl2CppByPath(env);

    BNM::AddOnLoadedEvent(OnLoaded_Example_01);
    BNM::AddOnLoadedEvent(OnLoaded_Example_02);
    BNM::AddOnLoadedEvent(OnLoaded_Example_03);
    BNM::AddOnLoadedEvent(OnLoaded_Example_04);

    return JNI_VERSION_1_6;
}