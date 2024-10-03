#include <BNM/UserSettings/GlobalSettings.hpp>

#include <BNM/Class.hpp>
#include <BNM/Field.hpp>
#include <BNM/Method.hpp>
#include <BNM/Property.hpp>
#include <BNM/Operators.hpp>
#include <BNM/BasicMonoStructures.hpp>
#include <BNM/ComplexMonoStructures.hpp>

#include <BNM/ClassesManagement.hpp>

using namespace BNM::Structures::Unity; // Vector3, Vector2 etc.
using namespace BNM::Structures; // Mono::String, Mono::Array etc.

void MonoArray() {
    // Type[] - an array of any objects
    Mono::Array<int> *array = nullptr;

    //! It can be created in two ways
    // * Array<Тип>::Create(размер или std::vector<Тип>) - does not get into the garbage collector
    // * LoadClass().NewArray<Тип>(размер) - gets into the garbage collector

    // Let's create it in 1 way
    array = Mono::Array<int>::Create(10);

    //! You can get the data using
    auto dataPtr = array->GetData(); // Pointer to the C array
    // or
    auto dataVec = array->ToVector(); // std::vector<int>
    // or
    auto firstData = array->At(0); // First element of the array
}

void MonoList() {
    // System.Collections.Generic.List<Тип> - a list of any objects
    Mono::List<int> *list = nullptr;

    //! It can only be created through a class:
    // LoadClass().NewList<Type>(size) - gets into the garbage collector
    // LoadClass().NewListUnsafe<Type>(size) - gets into the garbage collector, but all methods are handled by BNM

    //! To avoid searching for the System.Int32 class (the int value class in C#)
    //! You can use BNM::Defaults::Get<Type>()
    //! BNM::Defaults::Get supports only basic types
    auto intClass = BNM::Defaults::Get<int>().ToClass();

    list = intClass.NewList<int>();

    //! You can get the data using
    auto dataPtr = list->GetData(); // Pointer to the C array
    // or
    auto dataVec = list->ToVector(); // std::vector<int>
    // or
    auto firstData = list->At(0); // The first element of the array
}

void MonoDictionary() {
    // System.Collections.Generic.Dictionary<Key type, Value type> - dictionary
    Mono::Dictionary<int, int> *dictionary;

    // For more information about generic, see example 03
    auto dictionaryClass = BNM::Class(BNM_OBFUSCATE("System.Collections.Generic"), BNM_OBFUSCATE("Dictionary`2"), BNM::Image(BNM_OBFUSCATE("mscorlib.dll")));
    auto dictionary_int_int_Class = dictionaryClass.GetGeneric({BNM::Defaults::Get<int>(), BNM::Defaults::Get<int>()});

    dictionary = (Mono::Dictionary<int, int> *) dictionary_int_int_Class.CreateNewObjectParameters();

    //! You can get the data using
    auto keys = dictionary->GetKeys(); // std::vector<Key type>
    // or
    auto values = dictionary->GetValues(); // std::vector<Value type>
    // or
    auto map = dictionary->ToMap(); // std::map<Key type, Value type>
    // or
    int value = 0;
    if (dictionary->TryGet(1, &value))
        ; // Value found
}

// The example below can be compiled in Unity and it will work
namespace DelegatesAndActions {
    // C# class
    /*
     public class Delegates : MonoBehaviour {
        public delegate int JustDelegate(int x, int y);
        public JustDelegate justDelegateDef;
        public UnityAction<int, int> JustUnityAction;
        public Action<int, int> JustAction;
        public UnityEvent<int, int> JustEvent;

        void Start() {
            justDelegateDef += delegate(int x, int y) {
                Log($"justDelegateDef(1) x: {x}, y: {y}");
                return 1;
            };
            justDelegateDef += delegate(int x, int y) {
                Log($"justDelegateDef(3) x: {x}, y: {y}");
                return 3;
            };
            justDelegateDef += delegate(int x, int y) {
                Log($"justDelegateDef(500) x: {x}, y: {y}");
                return 500;
            };

            JustAction += delegate(int x, int y) {
                Log($"JustAction x: {x}, y: {y}");
                return;
            };
            JustUnityAction += delegate(int x, int y) {
                Log($"JustUnityAction x: {x}, y: {y}");
                return;
            };
            JustEvent.AddListener(delegate(int x, int y) {
                Log($"JustEvent x: {x}, y: {y}");
                return;
            });

            // Just output of messages
            logClass = new AndroidJavaClass("android.util.Log");
        }
        // Just output of messages
        private AndroidJavaClass logClass;
        void Log(string s) { logClass.CallStatic<int>("e", "BNM_TargetApp", s); }
    }
    */

    // ClassesManagement is used here. It is described in more detail in Example 05.
    struct Delegates : BNM::UnityEngine::MonoBehaviour {
        BNM::MulticastDelegate<int> *justDelegateDef;
        BNM::UnityEngine::UnityAction<int, int> *JustUnityAction;
        BNM::Structures::Mono::Action<int, int> *JustAction;
        BNM::UnityEngine::UnityEvent<int, int> *JustEvent;
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
        BNM_CustomMethod(Start, false, BNM::Defaults::Get<void>(), BNM_OBFUSCATE("Start"));
        BNM_CustomMethodSkipTypeMatch(Start);
        BNM_CustomMethodMarkAsInvokeHook(Start);
    };

    // You will see something like this in log:
    /*
        ByNameModding           justDelegateDef: 0x7986ef6900
        ByNameModding           JustUnityAction: 0x7986ef67e0
        ByNameModding           JustAction: 0x7986ef6870
        ByNameModding           JustEvent: 0x7986eea480
        BNM_TargetApp           justDelegateDef(1) x: 10, y: 60
        BNM_TargetApp           justDelegateDef(3) x: 10, y: 60
        BNM_TargetApp           justDelegateDef(500) x: 10, y: 60
        BNM_TargetApp           JustUnityAction x: 70, y: 9
        BNM_TargetApp           JustAction x: 30, y: 42
        BNM_TargetApp           JustEvent x: 7, y: 234
     */
}

void OnLoaded_Example_02() {
    using namespace BNM;

    //! Unity structures

    // Mathematical structures
    // Mathematical operations similar to those in Unity can be performed on these structures
    Vector2 vector2;
    Vector3 vector3;
    Vector4 vector4;
    Matrix3x3 matrix3x3;
    Matrix4x4 matrix4x4;
    Quaternion quaternion;

    // Structures for Raycast
    Ray ray;
    RaycastHit raycastHit;

    //! Mono structures

    //! System.String, for more information, see Example 01
    Mono::String *string;

    //! Method describes Array
    MonoArray();

    //! Method describes List
    MonoList();

    //! Method describes Dictionary
    MonoDictionary();

    //! Namespace describes Delegates and Actions
   using namespace DelegatesAndActions;
}
