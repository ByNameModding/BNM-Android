#include <BNM/UserSettings/GlobalSettings.hpp>

#include <BNM/Class.hpp>
#include <BNM/Field.hpp>
#include <BNM/Method.hpp>
#include <BNM/Property.hpp>
#include <BNM/Defaults.hpp>
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

    *playerName = BNM::CreateMonoString(BNM_OBFUSCATE("BNM_Player"));
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
    ObjectClass = Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("Object"));

    // Get UnityEngine.Object::ToString method with 0 parameters
    ObjectToString = ObjectClass.GetMethod(BNM_OBFUSCATE("ToString"), 0);

    // Get UnityEngine.Object::name property
    ObjectName = ObjectClass.GetProperty(BNM_OBFUSCATE("name"));


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
    auto PlayerClass = Class(BNM_OBFUSCATE(""), BNM_OBFUSCATE("Player"));

    // Get the Player::Config class
    auto PlayerConfigClass = PlayerClass.GetInnerClass(BNM_OBFUSCATE("Config"));

    // Get the Update and Start methods of the Player class
    auto Update = PlayerClass.GetMethod(BNM_OBFUSCATE("Update"));
    auto Start = PlayerClass.GetMethod(BNM_OBFUSCATE("Start"));

    // Get the Player.m_Config field
    PlayerConfig = PlayerClass.GetField(BNM_OBFUSCATE("m_Config"));

    // Get Player::Config's fields
    ConfigName = PlayerConfigClass.GetField(BNM_OBFUSCATE("Name"));
    ConfigHealth = PlayerConfigClass.GetField(BNM_OBFUSCATE("Health"));
    ConfigCoins = PlayerConfigClass.GetField(BNM_OBFUSCATE("Coins"));

    // Hook Update and Start methods

    // There are 3 methods for hooking methods:
    // * HOOK - hook via hooking software
    // * InvokeHook - hook that works for those methods that are called directly by the engine (constructors (.ctor), events (Start, Update, etc.))
    // * VirtualHook - hook for virtual methods

    // To replace Start and Update for this class, the best option is InvokeHook
    InvokeHook(Update, PlayerUpdate, old_PlayerUpdate);
    InvokeHook(Start, PlayerStart, old_PlayerStart);

    // BNM::Defaults:

    // BNM::Defaults::Get<type>() - fast API that allow get all basic C# types and some basic Unity's types
    // Supported types list:
    /*
     C++, C# keyword, .NET type

     void, void, System.Void
     bool, bool, System.Boolean

     BNM::Types::byte (uint8_t), byte, System.Byte
     BNM::Types::sbyte (int8_t), sbyte, System.SByte

     short (int16_t), short, System.Int16
     BNM::Types::ushort (uint16_t), ushort, System.UInt16

     int (int32_t), int, System.Int32
     BNM::Types::uint (unsigned int), uint, System.UInt32

     BNM::Types::nint (intptr_t), nint, System.IntPtr
     BNM::Types::nuint (uintptr_t), nuint, System.UIntPtr

     long, long, System.Int64
     BNM::Types::ulong (unsigned long), ulong, System.UInt64

     float, float, System.Single
     double, double, System.Double
     BNM::Types::decimal, decimal, System.Double

     BNM::IL2CPP::Il2CppString *, BNM::Structures::Mono::String *, string, System.String
     BNM::Structures::Unity::Vector2, Vector2, UnityEngine.Vector2
     BNM::Structures::Unity::Vector3, Vector3, UnityEngine.Vector3
     BNM::Structures::Unity::Vector4, Vector4, UnityEngine.Vector4
     BNM::Structures::Unity::Color, Color, UnityEngine.Color
     BNM::Structures::Unity::Color32, Color32, UnityEngine.Color32
     BNM::Structures::Unity::Ray, Ray, UnityEngine.Ray
     BNM::Structures::Unity::Quaternion, Quaternion, UnityEngine.Quaternion
     BNM::Structures::Unity::Matrix3x3, Matrix3x3, UnityEngine.Matrix3x3
     BNM::Structures::Unity::Matrix4x4, Matrix4x4, UnityEngine.Matrix4x4
     BNM::Structures::Unity::RaycastHit, RaycastHit, UnityEngine.RaycastHit
     BNM::UnityEngine::Object *, Object, UnityEngine.Object
     BNM::UnityEngine::MonoBehaviour *, MonoBehaviour, UnityEngine.MonoBehaviour

     Any other pointer, object, System.Object
    */

    // Get int.Parse method
    BNM::Method<int> Parse = BNM::Defaults::Get<int>().ToClass().GetMethod(BNM_OBFUSCATE("Parse"), 1);
}
