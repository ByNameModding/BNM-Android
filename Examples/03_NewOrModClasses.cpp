#include "../ByNameModding/BNM.hpp"

#if !BNM_DISABLE_NEW_CLASSES

using namespace BNM::Structures::Unity;
using namespace BNM::Structures::Mono;

// Add a class to the game
/*
    namespace BNM_Example_03 {
        public class BNM_ExampleObject : UnityEngine.MonoBehaviour, UnityEngine.IExposedPropertyTable {
            int Value;
            void Start();
            Object GetReferenceValue(PropertyName id, out bool idValid);
        };
    };
*/
//! BNM::IL2CPP::Il2CppObject is needed if the object inherits from System.Object or nothing
//! BNM::UnityEngine::Object is needed if the object inherits from UnityEngine.MonoBehaviour or UnityEngine.ScriptableObject
struct BNM_ExampleObject : public BNM::UnityEngine::Object {
    BNM_NewClassInit("BNM_Example_03", BNM_ExampleObject, {
        // Code to find the parent class
        //! BNM::LoadClass takes the dll name as the 3rd argument (can include '.dll' or not)
        return BNM::LoadClass(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("MonoBehaviour"), OBFUSCATE_BNM("UnityEngine.CoreModule"));
    },
    // Interfaces
    BNM::GetType(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("IExposedPropertyTable"))
    );

    int Value;
    void Start() {
        BNM_LOG_INFO("BNM_ExampleObject::Start!");
    }

    void* GetReferenceValue(int id, bool* isValid) {
        *isValid = false;
        BNM_LOG_INFO("BNM_ExampleObject::GetReferenceValue(%d)!", id);
        return nullptr;
    }

    // To override virtual methods, argument types must match exactly
    BNM_NewMethodInit(BNM::GetType<BNM::IL2CPP::Il2CppObject*>(), GetReferenceValue, 2,
                      BNM::GetType(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("IExposedPropertyTable")),
                      BNM::GetType<bool>());

    BNM_NewMethodInit(BNM::GetType<void>(), Start, 0);
    BNM_NewFieldInit(Value, BNM::GetType<int>());

    // All possible parameters
    // BNM_NewClassInit - start
    // BNM_NewClassWithDllInit - start
    // BNM_NewMethodInit - method
    // BNM_NewStaticMethodInit - static method
    // BNM_NewConstructorInit - constructor
    // BNM_NewMethodInitCustomName - method
    // BNM_NewStaticMethodInitCustomName - static method
    // BNM_NewFieldInit - field
    //! BNM_NewOwnerInit - class owner
    /**
       public class Owner {
            public class YourClass {
            };
       };
    */
};

//! Modify the Player class
/*
    public class Player : UnityEngine.MonoBehaviour {
        int Coins;
    };
*/
//! Bringing it to the form
/*
    public class Player : UnityEngine.MonoBehaviour {
        int Coins;
        void Start();
    };
*/
// Inheritance is not required
class Player {
    BNM_ModClassInit(Player, {
        return BNM::LoadClass(OBFUSCATE_BNM(""), OBFUSCATE_BNM("Player"));
    });
    void Start() {
        BNM_LOG_INFO("Player::Start!");
        BNM::LoadClass((BNM::IL2CPP::Il2CppObject *)this).GetFieldByName(OBFUSCATE_BNM("Coins")) .cast<int>()[(void *)this] = 2147483647;
    }
    BNM_ModAddMethod(BNM::GetType<void>(), Start, 0);

    // All possible parameters
    // BNM_ModClassInit - start
    // BNM_ModAddMethod - method
    // BNM_ModAddStaticMethod - static method
    // BNM_ModAddField - field
    // BNM_ModNewParent - parent
    // BNM_ModNewOwner - class owner
};

void OnLoaded_Example_03() {
    using namespace BNM;

    //! auto BNM_ExampleObjectClass = LoadClass(OBFUSCATE_BNM("BNM_Example_03"), OBFUSCATE_BNM("BNM_ExampleObject"))
    // or
    LoadClass BNM_ExampleObjectClass = BNM_ExampleObject::BNMClass.myClass;
}

#endif