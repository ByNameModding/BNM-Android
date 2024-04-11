#include <jni.h>
#include <BNM/Loading.hpp>

void OnLoaded_Example_01();
void OnLoaded_Example_02();
void OnLoaded_Example_03();
void OnLoaded_Example_04();
void OnLoaded_Example_05();

//! У BNM много возможностей.
//! Самые основные описаны в примерах.
//! Остальные вы можете узнать в BNM.hpp.

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);

    // Загрузить BNM, найдя путь до libil2cpp.so
    BNM::Loading::TryLoadByJNI(env);

    BNM::Loading::AddOnLoadedEvent(OnLoaded_Example_01);
    BNM::Loading::AddOnLoadedEvent(OnLoaded_Example_02);
    BNM::Loading::AddOnLoadedEvent(OnLoaded_Example_03);
    BNM::Loading::AddOnLoadedEvent(OnLoaded_Example_04);
    BNM::Loading::AddOnLoadedEvent(OnLoaded_Example_05);

    return JNI_VERSION_1_6;
}