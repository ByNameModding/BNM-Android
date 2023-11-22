#include "../ByNameModding/BNM.hpp"

// Needed so we don't have to write
// BNM::Structures:: for every type
using namespace BNM::Structures::Mono; // monoString, monoArray etc.
using namespace BNM::Operators; // Operators for methods, fields and properties

// Variable to store class
BNM::LoadClass ObjectClass{};
// Variable to store property
BNM::Property<monoString *> ObjectName{};
// Variable to store method
BNM::Method<monoString *> ObjectToString{};

// Variable to store field
BNM::Field<void *> PlayerConfig{};


BNM::Field<monoString *> ConfigName{};
BNM::Field<int> ConfigHealth{};
BNM::Field<int> ConfigCoins{};

// Variable to store reference to field
void **PlayerConfigPtr = nullptr;

void (*old_PlayerStart)(BNM::UnityEngine::Object *);
void PlayerStart(BNM::UnityEngine::Object *instance) {
    old_PlayerStart(instance); // Call original code

    //! monoString - C# string (string or System.String) in BNM

    // In this case the field has a type and the property will automatically call the Get method to get the name.
    // If using auto, the field will be a copy of ObjectName and you'll have to either call Get() or add () to ObjectName i.e.:
    // (instance >> ObjectName)() // for >> and ->*
    // ObjectName[instance]() // for []
    monoString *playerObjectName =
            // Set instance in ObjectName via ->* operator
            // Alternatives:
            // * instance >> ObjectName
            // * ObjectName[instance]
            instance->*ObjectName;

    // Get reference to field data
    PlayerConfigPtr = PlayerConfig[instance].GetPointer();

    // In case of fields, ->* operator immediately returns reference to field data
    auto playerName = *PlayerConfigPtr->*ConfigName;

    // Print player object name to log
    BNM_LOG_INFO("Имя объекта игрока: \"%s\"; Имя игрока: \"%s\"",
                 // str() - convert string to std::string
                 playerObjectName->str().c_str(),
                 // -> - iterator operator to check data and warn if invalid (in debug mode)
                 playerName->str().c_str());

    //! Call ObjectToString, you can pass arguments in () like when calling any method
    // Alternative way to call method with object
    //     auto objectToStringResult = (instance >> ObjectToString)();

    auto objectToStringResult = ObjectToString[instance]();
    BNM_LOG_INFO("objectToStringResult: \"%s\"", objectToStringResult->str().c_str());


    //! Change player name

    // String can be created 2 ways:
    // * monoString::Create - doesn't get garbage collected by Unity, i.e. you'll need to manually delete it
    // * BNM::CreateMonoString - gets garbage collected by Unity, it will delete it automatically when needed

    // In this case it's better to use BNM::CreateMonoString because this string will exist in the game for some time and manually deleting it will be inconvenient
    *playerName = BNM::CreateMonoString(OBFUSCATE_BNM("BNM_Player"));

}

void (*old_PlayerUpdate)(BNM::UnityEngine::Object *);
void PlayerUpdate(BNM::UnityEngine::Object *instance) {
    old_PlayerUpdate(instance); // Call original code

    // Check if m_Config field reference is valid
    if (PlayerConfigPtr == nullptr) return;

    // Set 99999 health using ->* operator
    //! *((*PlayerConfigPtr)->*ConfigHealth) = 99999;
    // Or using []
    ConfigHealth[*PlayerConfigPtr] = 99999;

    // Set 99999 coins
    ConfigCoins[*PlayerConfigPtr] = 99999;
}


// Here you can get all the info you need
void OnLoaded_Example_01() {
    using namespace BNM; // So we don't have to write BNM:: in this method

    // Get UnityEngine.Object class
    ObjectClass = LoadClass(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Object"));

    // Get UnityEngine.Object::ToString method with 0 parameters
    ObjectToString = ObjectClass.GetMethodByName(OBFUSCATE_BNM("ToString"), 0);

    // Get UnityEngine.Object::name property
    ObjectName = ObjectClass.GetPropertyByName(OBFUSCATE_BNM("name"));


    /* Let's imagine there is a class in the game:
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
    // Get Player class
    auto PlayerClass = LoadClass(OBFUSCATE_BNM(""), OBFUSCATE_BNM("Player"));

    // Get Player::Config class
    auto PlayerConfigClass = PlayerClass.GetInnerClass(OBFUSCATE_BNM("Config"));

    // Get Update and Start methods of Player class
    auto Update = PlayerClass.GetMethodByName(OBFUSCATE_BNM("Update"));
    auto Start = PlayerClass.GetMethodByName(OBFUSCATE_BNM("Start"));

    // Get Player.m_Config field
    PlayerConfig = PlayerClass.GetFieldByName(OBFUSCATE_BNM("m_Config"));

    // Get Player::Config fields
    ConfigName = PlayerConfigClass.GetFieldByName(OBFUSCATE_BNM("Name"));
    ConfigHealth = PlayerConfigClass.GetFieldByName(OBFUSCATE_BNM("Health"));
    ConfigCoins = PlayerConfigClass.GetFieldByName(OBFUSCATE_BNM("Coins"));

    // Override Update and Start methods

    // There are 3 methods for overriding methods:
    // * HOOK - override using method overriding software
    // * InvokeHook - override that works for methods called directly by the engine (constructors (.ctor), events (Start, Update, etc.))
    // * VirtualHook - override for virtual methods

    // For overriding Start and Update for this class, the best option is InvokeHook
    InvokeHook(Update, PlayerUpdate, old_PlayerUpdate);
    InvokeHook(Start, PlayerStart, old_PlayerStart);
}

[[maybe_unused]] __attribute__((constructor))
void Example_01_main() {
    // Runs immediately after il2cpp loads from its thread
    BNM::AddOnLoadedEvent(OnLoaded_Example_01);
}