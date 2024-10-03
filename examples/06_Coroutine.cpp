#include <BNM/Coroutine.hpp>
#include <BNM/UnityStructures.hpp>
#include <BNM/ClassesManagement.hpp>
#include "BNM/Field.hpp"

#ifdef BNM_CLASSES_MANAGEMENT
#ifdef BNM_COROUTINE

BNM::UnityEngine::Object *thing{};

// An example of custom instructions for IEnumerator. For example, let's make an analog of WaitForSecondsRealtime, but using std::chrono
struct CustomYieldInstruction : BNM::IL2CPP::Il2CppObject {
    BNM_CustomClass(CustomYieldInstruction, BNM::CompileTimeClassBuilder(BNM_OBFUSCATE(""), BNM_OBFUSCATE("CustomYieldInstruction")).Build(), {}, {},
                    BNM::CompileTimeClassBuilder(BNM_OBFUSCATE("System.Collections"), BNM_OBFUSCATE("IEnumerator"), BNM_OBFUSCATE("mscorlib.dll")).Build());
    std::chrono::time_point<std::chrono::system_clock> waitUntilTime;
    void Finalize() { this->~CustomYieldInstruction(); }
    bool MoveNext() { return waitUntilTime > std::chrono::system_clock::now(); }
    void Reset() { waitUntilTime = {}; }
    Il2CppObject *Current() { return nullptr; }
    BNM_CustomMethod(Finalize, false, BNM::Defaults::Get<void>(), "Finalize");
    BNM_CustomMethod(MoveNext, false, BNM::Defaults::Get<bool>(), "MoveNext");
    BNM_CustomMethod(Reset, false, BNM::Defaults::Get<void>(), "Reset");
    BNM_CustomMethod(Current, false, BNM::Defaults::Get<BNM::IL2CPP::Il2CppObject *>(), "get_Current");

    void Setup(long long seconds) {
        Reset();
        waitUntilTime = std::chrono::system_clock::now() + std::chrono::seconds(seconds);
    }

    static BNM::IL2CPP::Il2CppObject *New(long long seconds) {
        auto instance = (CustomYieldInstruction *) BNM::Class(BNMCustomClass.myClass).CreateNewInstance();
        instance->Setup(seconds);
        return instance;
    }
};

// IEnumerator Example
BNM::Coroutine::IEnumerator IEnumeratorExample() {
    BNM_LOG_DEBUG("IEnumeratorExample 1");

    // The code is very similar to the IEnumerator code in C#, it's just that instead of yield return, you need to use co_yield
    co_yield BNM::Coroutine::WaitForEndOfFrame();
    BNM_LOG_DEBUG("IEnumeratorExample 2 (WaitForEndOfFrame)");

    co_yield BNM::Coroutine::WaitForFixedUpdate();
    BNM_LOG_DEBUG("IEnumeratorExample 3 (WaitForFixedUpdate)");

    co_yield BNM::Coroutine::WaitForSeconds(2.f);
    BNM_LOG_DEBUG("IEnumeratorExample 4 (WaitForSeconds)");

    co_yield BNM::Coroutine::WaitForSecondsRealtime(1.f);
    BNM_LOG_DEBUG("IEnumeratorExample 5 (WaitForSecondsRealtime)");

    co_yield CustomYieldInstruction::New(5);
    BNM_LOG_DEBUG("IEnumeratorExample 6 (CustomYieldInstruction)");

    // An analog of WaitWhile, but instead of il2cpp methods, it accepts C++ methods
    co_yield BNM::Coroutine::WaitWhile([]() -> bool {
        return thing->Alive();
    });
    BNM_LOG_DEBUG("'thing' мертва (dead) :_(");

    // Unlike C#, it is necessary to call co_return at the end of the code
    co_return;
}

BNM::Class GameObject;
BNM::Method<void> Destory;
BNM::Method<void *> StartCoroutine;
BNM::Field<void *> f;

// Suppose there is such a class in the game
struct EnumeratorTests : public BNM::UnityEngine::MonoBehaviour {
    BNM_CustomClass(EnumeratorTests, BNM::CompileTimeClassBuilder(nullptr, BNM_OBFUSCATE("EnumeratorTests")).Build(), {}, {});
    void Start() {
        BNM_CallCustomMethodOrigin(Start, this);

        // Creating an object for its subsequent destruction
        thing = (BNM::UnityEngine::Object *) GameObject.CreateNewObjectParameters();

        // You can get C# IEnumerator using .get() or any method of casting to IEnumerator *
        // BNM::Coroutine::IEnumerator *enumerator = IEnumeratorExample();
        // auto enumerator = (BNM::Coroutine::IEnumerator *) IEnumeratorExample();
        auto enumerator = IEnumeratorExample().get();

        auto r = StartCoroutine[this](enumerator);
        BNM_LOG_DEBUG("Invoked: %p", r);
    }

    inline static int frames = 0;
    void Update() {
        frames++;
        if (frames != 200) return;
        BNM_LOG_DEBUG("Killing 'thing'");
        Destory(thing);
    }

    BNM_CustomMethod(Start, false, BNM::Defaults::Get<void>(), "Start");
    BNM_CustomMethod(Update, false, BNM::Defaults::Get<void>(), "Update");
};

void OnLoaded_Example_06() {
    using namespace BNM;

    auto coreModule = BNM::Image(BNM_OBFUSCATE("UnityEngine.CoreModule.dll"));

    GameObject = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("GameObject"), coreModule);
    Destory = GameObject.GetParent().GetMethod(BNM_OBFUSCATE("Destroy"), 1);
    StartCoroutine = BNM::Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("MonoBehaviour"), coreModule).GetMethod(BNM_OBFUSCATE("StartCoroutine"), {BNM_OBFUSCATE("routine")});
}



#endif // BNM_COROUTINE
#endif // BNM_CLASSES_MANAGEMENT