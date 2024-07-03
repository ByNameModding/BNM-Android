#include <BNM/UserSettings/GlobalSettings.hpp>

#include <BNM/Class.hpp>
#include <BNM/Field.hpp>
#include <BNM/Method.hpp>
#include <BNM/Property.hpp>
#include <BNM/Operators.hpp>
#include <BNM/BasicMonoStructures.hpp>

// Used to not write BNM::Structures for each type
using namespace BNM::Structures; // Mono::String, Mono::Array etc.
using namespace BNM::Operators; // Operators for methods, fields, and properties

// Variable for storing class
BNM::Class ObjectClass{};
// Variable for storing property
BNM::Property<Mono::String *> ObjectName{};
// Variable for storing method
BNM::Method<Mono::String *> ObjectToString{};

// Variable for storing field
BNM::Field<void *> PlayerConfig{};


BNM::Field<Mono::String *> ConfigName{};
BNM::Field<int> ConfigHealth{};
BNM::Field<int> ConfigCoins{};

// Variable for storing pointer to field
void **PlayerConfigPtr = nullptr;
BNM::UnityEngine::Object *Player = nullptr;

void (*old_PlayerStart)(BNM::UnityEngine::Object *);
void PlayerStart(BNM::UnityEngine::Object *instance) {
    old_PlayerStart(instance); // Call original code

    // Checking whether the Unity object is alive (UnityEngine.Object and its child classes)
    //! ALWAYS use this check instead of basic nullptr check!
    if (Player->Alive() /*BNM::UnityEngine::IsUnityObjectAlive(Player)*/) {
        BNM_LOG_WARN("Player's Start called twice?");
    }

    Player = instance;

    //! Mono::String - C# string (string or System.String) in BNM

    // In this case, the field has a type and the property will automatically call the Get method to get the name.
    // If you use auto, the field will be a copy of objectName and you will either have to call Get() or add() to objectName, i.e.:
    // (instance >> ObjectName)() // for >> and ->*
    // ObjectName[instance]() // for []
    Mono::String *playerObjectName =
            // Alternatives:
            // * instance >> ObjectName
            // * instance->*ObjectName;
            ObjectName[instance];

    // Get pointers to these fields
    PlayerConfigPtr = PlayerConfig[instance].GetPointer();

    // In the case of fields, the ->* operator immediately returns a reference to the field data
    auto playerName = *PlayerConfigPtr->*ConfigName;

    // Log name of the player's object
    BNM_LOG_INFO("Player's object name: \"%s\"; Player's name: \"%s\"",
                 // str() - convert string to std::string
                 playerObjectName->str().c_str(),
                 // -> - iterator operator for checking data and warning if it is incorrect (in debugging mode)
                 playerName->str().c_str());

    //! Call ObjectToString, you can write arguments in (), as when calling any methods
    // Alternative method call with an object
    //     auto objectToStringResult = (instance >> ObjectToString)();

    auto objectToStringResult = ObjectToString[instance]();
    BNM_LOG_INFO("objectToStringResult: \"%s\"", objectToStringResult->str().c_str());


    //! Changing the player's name

    // The string can be created in 2 ways:
    // * String::Create - does not get into the Unity garbage collector, i.e. you will need to manually delete it
    // * BNM::CreateMonoString - gets into the Unity garbage collector, and it will delete it itself when needed

    // In this case, it is better to use BNM::CreateMonoString because this string will be in the game for some time and therefore it will be inconvenient to manually delete it
    *playerName = BNM::CreateMonoString(OBFUSCATE_BNM("BNM_Player"));
}

void (*old_PlayerUpdate)(BNM::UnityEngine::Object *);
void PlayerUpdate(BNM::UnityEngine::Object *instance) {
    old_PlayerUpdate(instance); // Call original code

    // Checking whether the pointer to the m_Config field data is correct
    if (PlayerConfigPtr == nullptr) return;

    // Set 99999 lives using the operator ->*
    //! *((*PlayerConfigPtr)->*ConfigHealth) = 99999;
    // Or using []
    ConfigHealth[*PlayerConfigPtr] = 99999;

    // Set 99999 coins
    ConfigCoins[*PlayerConfigPtr] = 99999;
}


// Here you can get all the necessary information
void OnLoaded_Example_01() {
    using namespace BNM; // To avoid writing BNM:: in this method

    // Get the UnityEngine class.Object
    ObjectClass = Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Object"));

    // Get UnityEngine.Object::ToString method with 0 parameters
    ObjectToString = ObjectClass.GetMethod(OBFUSCATE_BNM("ToString"), 0);

    // Get UnityEngine.Object::name property
    ObjectName = ObjectClass.GetProperty(OBFUSCATE_BNM("name"));


    /* Let's imagine that there is a class in the game:
        public class Player : MonoBehaviour {
            private void Start();
            private void Update();
            private Config m_Config;

            public class Config {
                string Name;
                int Health;
                int Coins;
            }
        }
    */
    // Get the Player class
    auto PlayerClass = Class(OBFUSCATE_BNM(""), OBFUSCATE_BNM("Player"));

    // Get the Player::Config class
    auto PlayerConfigClass = PlayerClass.GetInnerClass(OBFUSCATE_BNM("Config"));

    // Get the Update and Start methods of the Player class
    auto Update = PlayerClass.GetMethod(OBFUSCATE_BNM("Update"));
    auto Start = PlayerClass.GetMethod(OBFUSCATE_BNM("Start"));

    // Get the Player.m_Config field
    PlayerConfig = PlayerClass.GetField(OBFUSCATE_BNM("m_Config"));

    // Get Player::Config's fields
    ConfigName = PlayerConfigClass.GetField(OBFUSCATE_BNM("Name"));
    ConfigHealth = PlayerConfigClass.GetField(OBFUSCATE_BNM("Health"));
    ConfigCoins = PlayerConfigClass.GetField(OBFUSCATE_BNM("Coins"));

    // Hook Update and Start methods

    // There are 3 methods for hooking methods:
    // * HOOK - hook via hooking software
    // * InvokeHook - hook that works for those methods that are called directly by the engine (constructors (.ctor), events (Start, Update, etc.))
    // * VirtualHook - hook for virtual methods

    // To replace Start and Update for this class, the best option is InvokeHook
    InvokeHook(Update, PlayerUpdate, old_PlayerUpdate);
    InvokeHook(Start, PlayerStart, old_PlayerStart);
}
