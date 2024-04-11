#pragma once

#include <jni.h>

#include "UserSettings/GlobalSettings.hpp"

namespace BNM::Loading {
    // Подменить il2cpp::vm::Class::FromIl2CppType чтобы загрузить BNM, если загрузка произошла слишком поздно.
    // !!!Требуется UNHOOK!!! Иначе игра замедлится.
    // Вызывать ДО попытки загрузить BNM!
    void AllowedLateInitHook();

    // Использовать JNI и Context, чтобы найти полный путь до libil2cpp.so, а после подменить методы для загрузки BNM из потока il2cpp
    bool TryLoadByJNI(JNIEnv *env, jobject context = nullptr);

    // Использовать дескриптор библиотеки, чтобы подменить методы для загрузки BNM из потока il2cpp
    bool TryLoadByDlfcnHandle(void *handle);

    typedef void *(*MethodFinder)(const char *name, void *userData);
    // Установить данные и метод для поиска il2cpp методов
    void SetMethodFinder(MethodFinder finderMethod, void *userData);

    // Использовать пользовательский метод, чтобы подменить методы для загрузки BNM из потока il2cpp
    bool TryLoadByUsersFinder();

    // Использовать пользовательский метод, чтобы загрузить BNM из данного потока
    // НЕЛЬЗЯ вызывать метод до полной загрузки il2cpp!
    // ВНИМАНИЕ, при использовании ClassesManagement, могут быть проблемы (вылеты) из-за асинхронной записи данных!
    void TrySetupByUsersFinder();

    void AddOnLoadedEvent(void (*event)());
    void ClearOnLoadedEvents();
}