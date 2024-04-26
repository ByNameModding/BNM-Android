#include <BNM/Coroutine.hpp>

#ifdef BNM_CLASSES_MANAGEMENT
#ifdef BNM_COROUTINE

#include "Internals.hpp"

namespace Classes {
    BNM::Class AsyncOperation, WaitForEndOfFrame, WaitForFixedUpdate, WaitForSeconds, WaitForSecondsRealtime;
}

namespace IEnumeratorData {
    BNM::MANAGEMENT_STRUCTURES::CustomClass customClass;
    BNM::MANAGEMENT_STRUCTURES::CustomMethod customMethod_Finalize, customMethod_MoveNext, customMethod_Reset, customMethod_Current;
}

namespace CustomWaitData {
    BNM::MANAGEMENT_STRUCTURES::CustomClass customClass;
    BNM::MANAGEMENT_STRUCTURES::CustomMethod customMethod_Finalize, customMethod_MoveNext, customMethod_Reset, customMethod_Current;
}

struct CustomWait : BNM::IL2CPP::Il2CppObject {
    std::function<bool()> _func{};
    bool _isUntil = false;
    void Finalize() { this->~CustomWait(); }
    bool MoveNext() { return _isUntil == !_func(); }
    void Reset() {}
    Il2CppObject *Current() { return nullptr; }
};

struct BNM::Coroutine::_IEnumeratorInit {
    inline static void _Init() {
        using namespace IEnumeratorData;
        customClass = {};
        customClass._size = sizeof(IEnumerator);
        customClass._targetType = BNM::CompileTimeClassBuilder().Class(OBFUSCATE_BNM("IEnumerator"), OBFUSCATE_BNM("BNM.Coroutine")).Build();
        customClass._baseType = {};
        customClass._owner = {};
        customClass._interfaces = { BNM::CompileTimeClassBuilder().Class(OBFUSCATE_BNM("IEnumerator"), OBFUSCATE_BNM("System.Collections"), OBFUSCATE_BNM("mscorlib.dll")).Build() };
        BNM::MANAGEMENT_STRUCTURES::AddClass(&customClass);
        {
            constexpr auto p = &IEnumerator::Finalize; customMethod_Finalize = {};
            customMethod_Finalize._address = *(void **) &p;
            customMethod_Finalize._invoker = (void *) &BNM::MANAGEMENT_STRUCTURES::GetMethodInvoker<false, decltype(&IEnumerator::Finalize)>::Invoke;
            customMethod_Finalize._name = OBFUSCATE_BNM("Finalize");
            customMethod_Finalize._returnType = BNM::GetType<void>();
            customMethod_Finalize._isStatic = false;
            customMethod_Finalize._parameterTypes = {};
            customClass._methods.push_back(&customMethod_Finalize);
        }
        {
            constexpr auto p = &IEnumerator::MoveNext; customMethod_MoveNext = {};
            customMethod_MoveNext._address = *(void **) &p;
            customMethod_MoveNext._invoker = (void *) &BNM::MANAGEMENT_STRUCTURES::GetMethodInvoker<false, decltype(&IEnumerator::MoveNext)>::Invoke;
            customMethod_MoveNext._name = OBFUSCATE_BNM("MoveNext");
            customMethod_MoveNext._returnType = BNM::GetType<bool>();
            customMethod_MoveNext._isStatic = false;
            customMethod_MoveNext._parameterTypes = {};
            customClass._methods.push_back(&customMethod_MoveNext);
        }
        {
            constexpr auto p = &IEnumerator::Reset; customMethod_Reset = {};
            customMethod_Reset._address = *(void **) &p;
            customMethod_Reset._invoker = (void *) &BNM::MANAGEMENT_STRUCTURES::GetMethodInvoker<false, decltype(&IEnumerator::Reset)>::Invoke;
            customMethod_Reset._name = OBFUSCATE_BNM("Reset");
            customMethod_Reset._returnType = BNM::GetType<void>();
            customMethod_Reset._isStatic = false;
            customMethod_Reset._parameterTypes = {};
            customClass._methods.push_back(&customMethod_Reset);
        }
        {
            constexpr auto p = &IEnumerator::Current; customMethod_Current = {};
            customMethod_Current._address = *(void **) &p;
            customMethod_Current._invoker = (void *) &BNM::MANAGEMENT_STRUCTURES::GetMethodInvoker<false, decltype(&IEnumerator::Current)>::Invoke;
            customMethod_Current._name = OBFUSCATE_BNM("get_Current");
            customMethod_Current._returnType = BNM::GetType<BNM::IL2CPP::Il2CppObject *>();
            customClass._methods.push_back(&customMethod_Current);
        }
    }
};


void BNM::Internal::SetupCoroutine() {
    Coroutine::_IEnumeratorInit::_Init();

    using namespace CustomWaitData;
    customClass = {};
    customClass._size = sizeof(CustomWait);
    customClass._targetType = BNM::CompileTimeClassBuilder().Class(OBFUSCATE_BNM("CustomWait"), OBFUSCATE_BNM("BNM.Coroutine")).Build();
    customClass._baseType = {};
    customClass._owner = {};
    customClass._interfaces = { BNM::CompileTimeClassBuilder().Class(OBFUSCATE_BNM("IEnumerator"), OBFUSCATE_BNM("System.Collections"), OBFUSCATE_BNM("mscorlib.dll")).Build() };
    BNM::MANAGEMENT_STRUCTURES::AddClass(&customClass);
    {
        constexpr auto p = &CustomWait::Finalize; customMethod_Finalize = {};
        customMethod_Finalize._address = *(void **) &p;
        customMethod_Finalize._invoker = (void *) &BNM::MANAGEMENT_STRUCTURES::GetMethodInvoker<false, decltype(&CustomWait::Finalize)>::Invoke;
        customMethod_Finalize._name = OBFUSCATE_BNM("Finalize");
        customMethod_Finalize._returnType = BNM::GetType<void>();
        customMethod_Finalize._isStatic = false;
        customMethod_Finalize._parameterTypes = {};
        customClass._methods.push_back(&customMethod_Finalize);
    }
    {
        constexpr auto p = &CustomWait::MoveNext; customMethod_MoveNext = {};
        customMethod_MoveNext._address = *(void **) &p;
        customMethod_MoveNext._invoker = (void *) &BNM::MANAGEMENT_STRUCTURES::GetMethodInvoker<false, decltype(&CustomWait::MoveNext)>::Invoke;
        customMethod_MoveNext._name = OBFUSCATE_BNM("MoveNext");
        customMethod_MoveNext._returnType = BNM::GetType<bool>();
        customMethod_MoveNext._isStatic = false;
        customMethod_MoveNext._parameterTypes = {};
        customClass._methods.push_back(&customMethod_MoveNext);
    }
    {
        constexpr auto p = &CustomWait::Reset; customMethod_Reset = {};
        customMethod_Reset._address = *(void **) &p;
        customMethod_Reset._invoker = (void *) &BNM::MANAGEMENT_STRUCTURES::GetMethodInvoker<false, decltype(&CustomWait::Reset)>::Invoke;
        customMethod_Reset._name = OBFUSCATE_BNM("Reset");
        customMethod_Reset._returnType = BNM::GetType<void>();
        customMethod_Reset._isStatic = false;
        customMethod_Reset._parameterTypes = {};
        customClass._methods.push_back(&customMethod_Reset);
    }
    {
        constexpr auto p = &CustomWait::Current; customMethod_Current = {};
        customMethod_Current._address = *(void **) &p;
        customMethod_Current._invoker = (void *) &BNM::MANAGEMENT_STRUCTURES::GetMethodInvoker<false, decltype(&CustomWait::Current)>::Invoke;
        customMethod_Current._name = OBFUSCATE_BNM("get_Current");
        customMethod_Current._returnType = BNM::GetType<BNM::IL2CPP::Il2CppObject *>();
        customClass._methods.push_back(&customMethod_Current);
    }
}
void BNM::Internal::LoadCoroutine() {
    using namespace Classes;

    auto image = BNM::Image(OBFUSCATE_BNM("UnityEngine.CoreModule.dll"));
    auto _namespace = OBFUSCATE_BNM("UnityEngine");

    AsyncOperation = BNM::Class(_namespace, OBFUSCATE_BNM("AsyncOperation"), image);
    WaitForEndOfFrame = BNM::Class(_namespace, OBFUSCATE_BNM("WaitForEndOfFrame"), image);
    WaitForFixedUpdate = BNM::Class(_namespace, OBFUSCATE_BNM("WaitForFixedUpdate"),image);
    WaitForSeconds = BNM::Class(_namespace, OBFUSCATE_BNM("WaitForSeconds"), image);
    WaitForSecondsRealtime = BNM::Class(_namespace, OBFUSCATE_BNM("WaitForSecondsRealtime"), image);
}

void BNM::Coroutine::IEnumerator::Finalize() {
    _coroutine.destroy();
    this->~IEnumerator();
}

bool BNM::Coroutine::IEnumerator::MoveNext()  {
    // Запустить coroutine
    _coroutine.resume();

    // Проверить, завершён ли coroutine
    if (_coroutine.done()) return false;

    // Получить объект
    _current = _coroutine.promise().value()._object;
    return true;
}

BNM::Coroutine::IEnumerator *BNM::Coroutine::IEnumerator::get() {
    // Создание IEnumerator для il2cpp
    auto inst = (BNM::Coroutine::IEnumerator *) BNM::Class(IEnumeratorData::customClass.myClass).CreateNewInstance();
    inst->_current = nullptr;
    inst->_coroutine = nullptr;
    // Меняем coroutineHandle местами, чтобы избежать очистки
    std::swap(this->_coroutine, inst->_coroutine);
    return inst;
}

void BNM::Coroutine::IEnumerator::Reset() {}
BNM::IL2CPP::Il2CppObject *BNM::Coroutine::IEnumerator::Current() { return _current; }


BNM::Coroutine::AsyncOperation::AsyncOperation(intptr_t operation) { _object = Classes::AsyncOperation.CreateNewObjectParameters(operation); }
BNM::Coroutine::WaitForEndOfFrame::WaitForEndOfFrame() { _object = Classes::WaitForEndOfFrame.CreateNewInstance(); }
BNM::Coroutine::WaitForFixedUpdate::WaitForFixedUpdate() { _object = Classes::WaitForFixedUpdate.CreateNewInstance(); }
BNM::Coroutine::WaitForSeconds::WaitForSeconds(float seconds) { _object = Classes::WaitForSeconds.CreateNewObjectParameters(seconds); }
BNM::Coroutine::WaitForSecondsRealtime::WaitForSecondsRealtime(float seconds) { _object = Classes::WaitForSecondsRealtime.CreateNewObjectParameters(seconds); }

BNM::Coroutine::WaitUntil::WaitUntil(const std::function<bool()> &function) {
    auto obj = (CustomWait *) BNM::Class(CustomWaitData::customClass.myClass).CreateNewInstance();
    obj->_func = function; obj->_isUntil = true; _object = obj;
}

BNM::Coroutine::WaitWhile::WaitWhile(const std::function<bool()> &function) {
    auto obj = (CustomWait *) BNM::Class(CustomWaitData::customClass.myClass).CreateNewInstance();
    obj->_func = function; obj->_isUntil = false; _object = obj;
}

#endif
#endif