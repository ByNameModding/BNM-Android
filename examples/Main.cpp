#include <jni.h>
#include <BNM/Loading.hpp>

void OnLoaded_Example_01();
void OnLoaded_Example_02();
void OnLoaded_Example_03();
void OnLoaded_Example_04();
void OnLoaded_Example_05();
void OnLoaded_Example_06();
void OnLoaded_Example_08();
void Example_07();

//! BNM has many features.
//! The most basic ones are described in the examples.
//! You can find out the rest in best example/documentation ever - source code.

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);


    // Load BNM by finding the path to libil2cpp.so
    BNM::Loading::TryLoadByJNI(env);

    // Or load using KittyMemory (as an example)
    // Example_07();

    BNM::Loading::AddOnLoadedEvent(OnLoaded_Example_01);
    BNM::Loading::AddOnLoadedEvent(OnLoaded_Example_02);
    BNM::Loading::AddOnLoadedEvent(OnLoaded_Example_03);
    BNM::Loading::AddOnLoadedEvent(OnLoaded_Example_04);
    BNM::Loading::AddOnLoadedEvent(OnLoaded_Example_05);
    BNM::Loading::AddOnLoadedEvent(OnLoaded_Example_06);
    BNM::Loading::AddOnLoadedEvent(OnLoaded_Example_08);

    return JNI_VERSION_1_6;
}