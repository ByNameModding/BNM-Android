#include "../ByNameModding/BNM.hpp"

// Needed so we don't have to write
// BNM::Structures:: for every type
using namespace BNM::Structures::Unity; // Vector3, Vector2 etc
using namespace BNM::Structures::Mono; // monoString, monoArray etc

void MonoArray() {
    // Type[] - array of any objects
    monoArray<int> *array = nullptr;

    //! It can be created in two ways
    // * monoArray<Type>::Create(size or std::vector<Type>) - doesn't get garbage collected
    // * LoadClass().NewArray<Type>(size) - gets garbage collected
    // Let's create it the 1st way
    array = monoArray<int>::Create(10);

    //! Data can be accessed using
    auto dataPtr = array->GetData(); // Pointer to C array
    // or
    auto dataVec = array->ToVector(); // std::vector<int>
    // or
    auto firstData = array->At(0); // First array element

    //! Delete the array to free memory
    //! ONLY needed when created via monoArray<Type>::Create!
    array->Destroy();
}

void MonoList() {
    // System.Collections.Generic.List<Type> - a list of any objects
    monoList<int> *list = nullptr;

    //! It can only be created via a class i.e.
    // LoadClass().NewList<Type>(size) - gets garbage collected

    //! To avoid finding the System.Int32 class (int value class in C#)
    //! You can use BNM::GetType<Type>()
    //! BNM::GetType only supports basic types
    auto intClass = BNM::GetType<int>().ToLC();

    list = intClass.NewList<int>();

    //! Data can be accessed using
    auto dataPtr = list->GetData(); // Pointer to C array
    // or
    auto dataVec = list->ToVector(); // std::vector<int>
    // or
    auto firstData = list->At(0); // First array element
}

void MonoDictionary() {
    // System.Collections.Generic.Dictionary<KeyType, ValueType> - dictionary
    monoDictionary<int, int> *dictionary = nullptr;
    //! Cannot be created via BNM

    //! Data can be accessed using
    auto keys = dictionary->GetKeys(); // std::vector<KeyType>
    // or
    auto values = dictionary->GetValues(); // std::vector<ValueType>
    // or
    auto map = dictionary->ToMap(); // std::map<KeyType, ValueType>
    // or
    int value = 0;
    if (dictionary->TryGet(1, &value))
        ; // Value found
}

void OnLoaded_Example_02() {
    using namespace BNM;

    //! Unity structures

    // Mathematical structures
    // You can perform mathematical operations on these structures similar to those in Unity
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

    //! System.String, described in more detail in example 01
    monoString *string;

    //! monoArray is described in the method
    MonoArray();

    //! monoList is described in the method
    MonoList();

    //! monoDictionary is described in the method
    // MonoDictionary();
}

[[maybe_unused]] __attribute__((constructor))
void Example_02_main() {
    // Runs immediately after il2cpp loads from its thread
    BNM::AddOnLoadedEvent(OnLoaded_Example_02);
}