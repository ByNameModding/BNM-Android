#include <BNM/UserSettings/GlobalSettings.hpp>

#include <BNM/ClassesManagement.hpp>
#include <BNM/BasicMonoStructures.hpp>
#include <BNM/UnityStructures.hpp>
#include <BNM/Field.hpp>
#include "BNM/ComplexMonoStructures.hpp"

#ifdef BNM_CLASSES_MANAGEMENT

using namespace BNM::Structures::Unity;
using namespace BNM::Structures::Mono;

// Let's add a class to the game
/*
    namespace BNM_Example_05 {
        public class BNM_ExampleObject : UnityEngine.MonoBehaviour, UnityEngine.IExposedPropertyTable {
            int Value;
            void Start();
            Object GetReferenceValue(PropertyName id, out bool idValid);
        };
    };
*/
//! BNM::IL2CPP::Il2CppObject is needed if the object inherits System.Object or nothing
//! BNM::UnityEngine::Object is needed if the object inherits UnityEngine.ScriptableObject
//! BNM::UnityEngine::MonoBehaviour is needed if the object inherits UnityEngine.MonoBehaviour
struct BNM_ExampleObject : public BNM::UnityEngine::MonoBehaviour {

    BNM_CustomClass(BNM_ExampleObject,
                    BNM::CompileTimeClassBuilder(BNM_OBFUSCATE("BNM_Example_05"), BNM_OBFUSCATE("BNM_ExampleObject")).Build(),
                    BNM::CompileTimeClassBuilder(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("MonoBehaviour"), BNM_OBFUSCATE("UnityEngine.CoreModule")).Build(),
                    // Here need specify base class, if you need to create inner class
                    {},
                    BNM::CompileTimeClassBuilder(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("IExposedPropertyTable")).Build(),
                    );

    // To set fields, for example `veryImportantValue`
    // Otherwise it will have garbage from memory
    void Constructor() {
        BNM::UnityEngine::MonoBehaviour tmp = *this;
        *this = BNM_ExampleObject();
        *((BNM::UnityEngine::MonoBehaviour *)this) = tmp;
    }

    int Value{};
    uintptr_t veryImportantValue{0x424E4D};
    void Start() {
        BNM_LOG_INFO("BNM_ExampleObject::Start! Верен ли veryImportantValue (Is veryImportantValue true): %d", veryImportantValue == 0x424E4D);
    }

    void *GetReferenceValue(int id, bool *isValid) {
        *isValid = false;
        BNM_LOG_INFO("BNM_ExampleObject::GetReferenceValue(%d)!", id);
        return nullptr;
    }

    // To override virtual methods or replace methods, the types must be exactly the same
    BNM_CustomMethod(GetReferenceValue, false, BNM::Defaults::Get<BNM::IL2CPP::Il2CppObject *>(), "GetReferenceValue",
                 BNM::CompileTimeClassBuilder(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("IExposedPropertyTable")).Build(),
                 BNM::Defaults::Get<bool>());

    BNM_CustomMethod(Start, false, BNM::Defaults::Get<void>(), "Start");

    BNM_CustomField(Value, BNM::Defaults::Get<int>(), "Value");

    BNM_CustomMethod(Constructor, false, BNM::Defaults::Get<void>(), ".ctor");
};

//! Let's look at an example from example 02

// This class was created to replace Start and use class' fields
struct Delegates :
        // Since we want to use the fields directly, we need to specify parent to match addresses of fields themselves.
        BNM::UnityEngine::MonoBehaviour {
    // Here we specify fields. It is not necessary to specify them before or after BNM_CustomClass.
    BNM::MulticastDelegate<int> *justDelegateDef;
    BNM::UnityEngine::UnityAction<int, int> *JustUnityAction;
    BNM::Structures::Mono::Action<int, int> *JustAction;
    BNM::UnityEngine::UnityEvent<int, int> *JustEvent;
    // If the field is not needed and you do not create this class in C++ using new or any other methods, you can omit all fields that come after the necessary ones.
    void *logClass;

    BNM_CustomClass(Delegates, BNM::CompileTimeClassBuilder(nullptr, BNM_OBFUSCATE("Delegates")).Build(), {}, {});
    void Start() {
        BNM_CallCustomMethodOrigin(Start, this);

        BNM_LOG_DEBUG("justDelegateDef: %p", justDelegateDef);
        BNM_LOG_DEBUG("JustUnityAction: %p", JustUnityAction);
        BNM_LOG_DEBUG("JustAction: %p", JustAction);
        BNM_LOG_DEBUG("JustEvent: %p", JustEvent);
        if (justDelegateDef) justDelegateDef->Invoke(10, 60);
        if (JustUnityAction) JustUnityAction->Invoke(70, 9);
        if (JustAction) JustAction->Invoke(30, 42);
        if (JustEvent) JustEvent->Invoke(7, 234);
    }
    // We specify all information about method
    BNM_CustomMethod(Start, false, BNM::Defaults::Get<void>(), BNM_OBFUSCATE("Start"));
    // Used to speed up the search for methods. Tells BNM not to compare the types of fields, but simply check their number.
    BNM_CustomMethodSkipTypeMatch(Start);

    // Specifies BNM to use method hook via Invoke. It doesn't have to be specified, it just makes it easier for BNM to work.
    BNM_CustomMethodMarkAsInvokeHook(Start);

    // Also BNM have:
    //! BNM_CustomMethodMarkAsBasicHook()
    // It works the same way as BNM_CustomMethodMarkAsInvokeHook, but says to use hook using hooking software.
};

void OnLoaded_Example_05() {
    using namespace BNM;

    //! auto BNM_ExampleObjectClass = LoadClass(BNM_OBFUSCATE("BNM_Example_03"), BNM_OBFUSCATE("BNM_ExampleObject"))
    // or
    Class BNM_ExampleObjectClass = BNM_ExampleObject::BNMCustomClass.myClass;
}

#endif