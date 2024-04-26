#include <BNM/UserSettings/GlobalSettings.hpp>

#include <BNM/Class.hpp>
#include <BNM/Field.hpp>
#include <BNM/Method.hpp>
#include <BNM/Property.hpp>
#include <BNM/Operators.hpp>
#include <BNM/BasicMonoStructures.hpp>
#include <BNM/ComplexMonoStructures.hpp>

// Нужно, чтобы не писать
// BNM::Structures:: для каждого типа
using namespace BNM::Structures::Unity; // Vector3, Vector2 и т. д.
using namespace BNM::Structures; // String, Array и т. д.

void MonoArray() {
    // Тип[] - массив из каких-либо объектов
    Mono::Array<int> *array = nullptr;

    //! Его можно создать двумя путями
    // * Array<Тип>::Create(размер или std::vector<Тип>) - не попадает в сборщик мусора
    // * LoadClass().NewArray<Тип>(размер) - попадает в сборщик мусора
    // Создадим 1 способом
    array = Mono::Array<int>::Create(10);

    //! Получить данные можно используя
    auto dataPtr = array->GetData(); // Ссылка на С массив
    // или
    auto dataVec = array->ToVector(); // std::vector<int>
    // или
    auto firstData = array->At(0); // Первый элемент массива

    //! Удалить массив для освобождения памяти
    //! Нужно ТОЛЬКО при создании через Array<Тип>::Create!
    array->Destroy();
}

void MonoList() {
    // System.Collections.Generic.List<Тип> - список каких-либо объектов
    Mono::List<int> *list = nullptr;

    //! Его можно создать только через класс т.е.
    // LoadClass().NewList<Тип>(размер) - попадает в сборщик мусора

    //! Чтобы не искать класс System.Int32 (класс int значений в C#)
    //! Можно использовать BNM::GetType<Тип>()
    //! BNM::GetType поддерживает только основные типы
    auto intClass = BNM::GetType<int>().ToClass();

    list = intClass.NewList<int>();

    //! Получить данные можно используя
    auto dataPtr = list->GetData(); // Ссылка на С массив
    // или
    auto dataVec = list->ToVector(); // std::vector<int>
    // или
    auto firstData = list->At(0); // Первый элемент массива
}

void Dictionary() {
    // System.Collections.Generic.Dictionary<Тпи-ключ, Тип-значение> - словарь
    Mono::Dictionary<int, int> *dictionary = nullptr;

    // Подробнее про generic описано в примере 03
    auto dictionaryClass = BNM::Class(OBFUSCATE_BNM("System.Collections.Generic"), OBFUSCATE_BNM("Dictionary`2"), BNM::Image(OBFUSCATE_BNM("mscorlib.dll")));
    auto dictionary_int_int_Class = dictionaryClass.GetGeneric({BNM::GetType<int>(), BNM::GetType<int>()});

    dictionary = (Mono::Dictionary<int, int> *) dictionary_int_int_Class.CreateNewObjectParameters();

    //! Получить данные можно используя
    auto keys = dictionary->GetKeys(); // std::vector<Тпи-ключ>
    // или
    auto values = dictionary->GetValues(); // std::vector<Тип-значение>
    // или
    auto map = dictionary->ToMap(); // std::map<Тпи-ключ, Тип-значение>
    // или
    int value = 0;
    if (dictionary->TryGet(1, &value))
        ; // Значение найдено
}

void OnLoaded_Example_02() {
    using namespace BNM;

    //! Unity структуры

    // Математические структуры
    // Над этими структурами можно проводить математические операции аналогичные оным в Unity
    Vector2 vector2;
    Vector3 vector3;
    Vector4 vector4;
    Matrix3x3 matrix3x3;
    Matrix4x4 matrix4x4;
    Quaternion quaternion;

    // Структуры для Raycast
    Ray ray;
    RaycastHit raycastHit;

    //! Mono структуры

    //! System.String, подробнее описано в примере 01
    Mono::String *string;

    //! В методе описан Array
    MonoArray();

    //! В методе описан List
    MonoList();

    //! В методе описан Dictionary
    // MonoDictionary();
}
