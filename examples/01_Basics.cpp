#include <BNM/UserSettings/GlobalSettings.hpp>

#include <BNM/Class.hpp>
#include <BNM/Field.hpp>
#include <BNM/Method.hpp>
#include <BNM/Property.hpp>
#include <BNM/Operators.hpp>
#include <BNM/BasicMonoStructures.hpp>

// Нужно, чтобы не писать BNM::Structures для каждого типа
using namespace BNM::Structures; // Mono::String, Mono::Array и т. д.
using namespace BNM::Operators; // Операторы для методов, полей и свойств

// Переменная для хранения класса
BNM::Class ObjectClass{};
// Переменная для хранения свойства
BNM::Property<Mono::String *> ObjectName{};
// Переменная для хранения метода
BNM::Method<Mono::String *> ObjectToString{};

// Переменная для хранения поля
BNM::Field<void *> PlayerConfig{};


BNM::Field<Mono::String *> ConfigName{};
BNM::Field<int> ConfigHealth{};
BNM::Field<int> ConfigCoins{};

// Переменная для хранения ссылки на поле
void **PlayerConfigPtr = nullptr;
BNM::UnityEngine::Object *Player = nullptr;

void (*old_PlayerStart)(BNM::UnityEngine::Object *);
void PlayerStart(BNM::UnityEngine::Object *instance) {
    old_PlayerStart(instance); // Вызвать оригинальный код

    // Проверка на то, жив ли Unity объект (UnityEngine.Object и его дочерние классы)
    //! ВСЕГДА используйте такую проверку вместо простой проверки на nullptr!
    if (Player->Alive() /*BNM::UnityEngine::IsUnityObjectAlive(Player)*/) {
        BNM_LOG_WARN("Start игрока вызван дважды?");
    }

    Player = instance;

    //! String - C# строка (string или System.String) в BNM

    // В данном случае у поля есть тип и свойство автоматически вызовет метод Get чтобы получить имя.
    // Если использовать auto, то поле будет копией ObjectName и придётся либо вызывать Get() либо добавлять () к ObjectName т.е.:
    // (instance >> ObjectName)() // для >> и ->*
    // ObjectName[instance]() // для []
    Mono::String *playerObjectName =
            // Установить instance в ObjectName через оператор ->*
            // Алтернативы:
            // * instance >> ObjectName
            // * ObjectName[instance]
            instance->*ObjectName;

    // Получить ссылку на данные поля
    PlayerConfigPtr = PlayerConfig[instance].GetPointer();

    // В случае полей оператор ->* сразу возвращает ссылку на данные поля
    auto playerName = *PlayerConfigPtr->*ConfigName;

    // Вывести в лог имя объекта игрока
    BNM_LOG_INFO("Имя объекта игрока: \"%s\"; Имя игрока: \"%s\"",
                 // str() - конвертировать строку в std::string
                 playerObjectName->str().c_str(),
                 // -> - оператор итератора для проверки данных и предупреждения, если они не верны (в режиме отладке)
                 playerName->str().c_str());

    //! Вызвать ObjectToString, в () можно писать аргументы, как при вызове любых методов
    // Альтернативный вариант вызова метода с объектом
    //     auto objectToStringResult = (instance >> ObjectToString)();

    auto objectToStringResult = ObjectToString[instance]();
    BNM_LOG_INFO("objectToStringResult: \"%s\"", objectToStringResult->str().c_str());


    //! Меняем имя игрока
    
    // Строку можно создать 2 способами:
    // * String::Create - не попадает в сборщик мусора Unity, т.е. вам нужно будет вручную удалять её
    // * BNM::CreateMonoString - попадает в сборщик мусора Unity, и он сам удалит её когда будет нужно
    
    // В данном случае лучше использовать BNM::CreateMonoString т.к. эта строка будет в игре ещё какое-то время и поэтому вручную её удалять будет неудобно
    *playerName = BNM::CreateMonoString(OBFUSCATE_BNM("BNM_Player"));

}

void (*old_PlayerUpdate)(BNM::UnityEngine::Object *);
void PlayerUpdate(BNM::UnityEngine::Object *instance) {
    old_PlayerUpdate(instance); // Вызвать оригинальный код

    // Проверка верна ли ссылка на данные поля m_Config
    if (PlayerConfigPtr == nullptr) return;

    // Установить 99999 жизней используя оператор ->*
    //! *((*PlayerConfigPtr)->*ConfigHealth) = 99999;
    // Или используя []
    ConfigHealth[*PlayerConfigPtr] = 99999;

    // Установить 99999 монет
    ConfigCoins[*PlayerConfigPtr] = 99999;
}


// Тут можно получить всю нужную информацию
void OnLoaded_Example_01() {
    using namespace BNM; // Чтобы не писать BNM:: в этом методе

    // Получить класс UnityEngine.Object
    ObjectClass = Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Object"));

    // Получить метод UnityEngine.Object::ToString с 0 параметров
    ObjectToString = ObjectClass.GetMethod(OBFUSCATE_BNM("ToString"), 0);

    // Получить свойство UnityEngine.Object::name
    ObjectName = ObjectClass.GetProperty(OBFUSCATE_BNM("name"));


    /* Представим себе, что в игре есть класс:
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
    // Получить класс Player
    auto PlayerClass = Class(OBFUSCATE_BNM(""), OBFUSCATE_BNM("Player"));

    // Получить класс Player::Config
    auto PlayerConfigClass = PlayerClass.GetInnerClass(OBFUSCATE_BNM("Config"));

    // Получить методы Update и Start класса Player
    auto Update = PlayerClass.GetMethod(OBFUSCATE_BNM("Update"));
    auto Start = PlayerClass.GetMethod(OBFUSCATE_BNM("Start"));

    // Получить поле Player.m_Config
    PlayerConfig = PlayerClass.GetField(OBFUSCATE_BNM("m_Config"));

    // Получить поля Player::Config
    ConfigName = PlayerConfigClass.GetField(OBFUSCATE_BNM("Name"));
    ConfigHealth = PlayerConfigClass.GetField(OBFUSCATE_BNM("Health"));
    ConfigCoins = PlayerConfigClass.GetField(OBFUSCATE_BNM("Coins"));

    // Подменить методы Update и Start

    // Есть 3 метода для подмены методов:
    // * HOOK - подмена через ПО для подмены методов
    // * InvokeHook - подмена работающая для тех методов, которые вызываются напрямую движком (конструкторы (.ctor), события (Start, Update и т.п.) )
    // * VirtualHook - подмена для virtual методов

    // Для подмены Start и Update для данного класса самым лучим вариантом является InvokeHook
    InvokeHook(Update, PlayerUpdate, old_PlayerUpdate);
    InvokeHook(Start, PlayerStart, old_PlayerStart);
}
