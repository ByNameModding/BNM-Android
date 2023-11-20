#include "../ByNameModding/BNM.hpp"

#if !BNM_DISABLE_NEW_CLASSES

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
//! BNM::UnityEngine::Object нужен если объект наследует UnityEngine.MonoBehaviour или UnityEngine.ScriptableObject
struct BNM_ExampleObject : public BNM::UnityEngine::Object {
    BNM_NewClassInit("BNM_Example_03", BNM_ExampleObject, {
        // Код для поиска родителя
        //! BNM::LoadClass третьим аргументом принимает имя dll (можно с '.dll' можно без)
        return BNM::LoadClass(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("MonoBehaviour"), OBFUSCATE_BNM("UnityEngine.CoreModule"));
    },
    // Интерфейсы
    BNM::GetType(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("IExposedPropertyTable"))
    );

    int Value;
    void Start() {
        BNM_LOG_INFO("BNM_ExampleObject::Start!");
    }

    void *GetReferenceValue(int id, bool *isValid) {
        *isValid = false;
        BNM_LOG_INFO("BNM_ExampleObject::GetReferenceValue(%d)!", id);
        return nullptr;
    }

    // Для переопределения virtual методов, типы должны полностью совпадать
    BNM_NewMethodInit(BNM::GetType<BNM::IL2CPP::Il2CppObject *>(), GetReferenceValue, 2,
                      BNM::GetType(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("IExposedPropertyTable")),
                      BNM::GetType<bool>());

    BNM_NewMethodInit(BNM::GetType<void>(), Start, 0);
    BNM_NewFieldInit(Value, BNM::GetType<int>());

    // Все возможные параметры
    // BNM_NewClassInit - начало
    // BNM_NewClassWithDllInit - начало
    // BNM_NewMethodInit - метод
    // BNM_NewStaticMethodInit - статический метод
    // BNM_NewConstructorInit - конструктор
    // BNM_NewMethodInitCustomName - метод
    // BNM_NewStaticMethodInitCustomName - статический метод
    // BNM_NewFieldInit - поле
    //! BNM_NewOwnerInit - владелец класса
    /**
       public class Владелец {
            public class ВашКласс {
            };
       };
    */
};

//! Изменим класс Player
/*
    public class Player : UnityEngine.MonoBehaviour {
        int Coins;
    };
*/
//! Приведя к виду
/*
    public class Player : UnityEngine.MonoBehaviour {
        int Coins;
        void Start();
    };
*/
// Наследование не требуется
class Player {
    BNM_ModClassInit(Player, {
        return BNM::LoadClass(OBFUSCATE_BNM(""), OBFUSCATE_BNM("Player"));
    });
    void Start() {
        BNM_LOG_INFO("Player::Start!");
        BNM::LoadClass((BNM::IL2CPP::Il2CppObject *)this).GetFieldByName(OBFUSCATE_BNM("Coins")).cast<int>()[(void *)this] = 2147483647;
    }
    BNM_ModAddMethod(BNM::GetType<void>(), Start, 0);

    // Все возможные параметры
    // BNM_ModClassInit - начало
    // BNM_ModAddMethod - метод
    // BNM_ModAddStaticMethod - статический метод
    // BNM_ModAddField - поле
    // BNM_ModNewParent - родитель
    // BNM_ModNewOwner - владелец класса
};

void OnLoaded_Example_03() {
    using namespace BNM;

    //! auto BNM_ExampleObjectClass = LoadClass(OBFUSCATE_BNM("BNM_Example_03"), OBFUSCATE_BNM("BNM_ExampleObject"))
    // или
    LoadClass BNM_ExampleObjectClass = BNM_ExampleObject::BNMClass.myClass;
}


[[maybe_unused]] __attribute__((constructor))
void Example_03_main() {
    BNM::AddOnLoadedEvent(OnLoaded_Example_03);
}

#endif