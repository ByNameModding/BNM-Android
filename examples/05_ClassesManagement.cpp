#include <BNM/UserSettings/GlobalSettings.hpp>

#include <BNM/ClassesManagement.hpp>
#include <BNM/BasicMonoStructures.hpp>
#include <BNM/UnityStructures.hpp>
#include <BNM/Field.hpp>

#ifdef BNM_CLASSES_MANAGEMENT

using namespace BNM::Structures::Unity;
using namespace BNM::Structures::Mono;

// Добавим в игру класс
/*
    namespace BNM_Example_03 {
        public class BNM_ExampleObject : UnityEngine.MonoBehaviour, UnityEngine.IExposedPropertyTable {
            int Value;
            void Start();
            Object GetReferenceValue(PropertyName id, out bool idValid);
        };
    };
*/
//! BNM::IL2CPP::Il2CppObject нужен если объект наследует System.Object или ничего
//! BNM::UnityEngine::Object нужен если объект наследует UnityEngine.ScriptableObject
//! BNM::UnityEngine::MonoBehaviour нужен если объект наследует UnityEngine.MonoBehaviour
struct BNM_ExampleObject : public BNM::UnityEngine::MonoBehaviour {

    BNM_CustomClass(BNM_ExampleObject,
                    BNM::CompileTimeClassBuilder().Class(OBFUSCATE_BNM("BNM_ExampleObject"), OBFUSCATE_BNM("BNM_Example_03")).Build(),
                    BNM::CompileTimeClassBuilder().Class(OBFUSCATE_BNM("MonoBehaviour"), OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("UnityEngine.CoreModule")).Build(),
                    {} /*BNM::CompileTimeClass()*/ /*BNM::CompileTimeClassBuilder().Build()*/,
                    BNM::CompileTimeClassBuilder().Class(OBFUSCATE_BNM("IExposedPropertyTable"), OBFUSCATE_BNM("UnityEngine")).Build(),
                    );

    // Чтобы установить поля, например `veryImportantValue`
    // Иначе в нём будет мусор из памяти
    void Constructor() {
        BNM::UnityEngine::MonoBehaviour tmp = *this;
        *this = BNM_ExampleObject();
        memcpy(this, &tmp, sizeof(BNM::UnityEngine::MonoBehaviour));
    }

    int Value{};
    uintptr_t veryImportantValue{0x424E4D};
    void Start() {
        BNM_LOG_INFO("BNM_ExampleObject::Start! Верен ли veryImportantValue: %d", veryImportantValue == 0x424E4D);
    }

    void *GetReferenceValue(int id, bool *isValid) {
        *isValid = false;
        BNM_LOG_INFO("BNM_ExampleObject::GetReferenceValue(%d)!", id);
        return nullptr;
    }

    // Для переопределения virtual методов или подмены методов, типы должны полностью совпадать
    BNM_CustomMethod(GetReferenceValue, false, BNM::GetType<BNM::IL2CPP::Il2CppObject *>(), "GetReferenceValue",
                 BNM::CompileTimeClassBuilder().Class(OBFUSCATE_BNM("IExposedPropertyTable"), OBFUSCATE_BNM("UnityEngine")).Build(),
                 BNM::GetType<bool>());

    BNM_CustomMethod(Start, false, BNM::GetType<void>(), "Start");

    BNM_CustomField(Value, BNM::GetType<int>(), "Value");

    BNM_CustomMethod(Constructor, false, BNM::GetType<void>(), ".ctor");
};

//! Изменим класс Player
/*
    public class Player : UnityEngine.MonoBehaviour {
        int Coins;
        void Update();
    };
*/
//! Приведя к виду
/*
    public class Player : UnityEngine.MonoBehaviour {
        int Coins;
        void Start();
        void Update();
    };
*/
// Наследование не требуется, но если вы добавите поле и будете использовать этот класс в коде, то все поля должны совпадать с полями в игре
struct Player {
    BNM_CustomClass(Player,
                    BNM::CompileTimeClassBuilder().Class(OBFUSCATE_BNM("Player"), nullptr, OBFUSCATE_BNM("Assembly-CSharp.dll") /*Можно не указывать, поиск будет по всем dll*/).Build(),
                    {}, {},
    );
    void Start() {
        BNM_LOG_INFO("Player::Start!");
        BNM::Class((BNM::IL2CPP::Il2CppObject *)this).GetField(OBFUSCATE_BNM("Coins")).cast<int>()[(void *)this] = 2147483647;
    }

    BNM_CustomMethod(Start, false, BNM::GetType<void>(), "Start");

    // Метод будет автоматически подменён
    BNM_CustomMethod(Update, false, BNM::GetType<void>(), "Update");
    void Update() {
        // Вызвать оригинальный Update
        BNM_CallOriginalCustomMethod(Update, this);
    }
};

void OnLoaded_Example_05() {
    using namespace BNM;

    //! auto BNM_ExampleObjectClass = LoadClass(OBFUSCATE_BNM("BNM_Example_03"), OBFUSCATE_BNM("BNM_ExampleObject"))
    // или
    Class BNM_ExampleObjectClass = BNM_ExampleObject::BNMCustomClass.myClass;
}

#endif