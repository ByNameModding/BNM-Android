#include "ByNameModding/BNM.hpp"
using namespace BNM::UNITY_STRUCTS; // Vector3, Vector2 и т. д.
using namespace BNM::MONO_STRUCTS; // monoString, monoArray и т. д.

/* getExternMethod: пример изменения поля зрения
* Код взят отсюда:
* Il2CppResolver
* https://github.com/kp7742/IL2CppResolver/blob/master/Android/test/src/demo.cpp
* IL2CppResolver, сделанный MJx0, не работает во всех версиях Unity
* getExternMethod работает ТОЛЬКО с внешними (extern) методами
*/
void set_fov(float value) {
    int (*Screen$$get_height)();
    int (*Screen$$get_width)();
    InitResolveFunc(Screen$$get_height, OBFUSCATE_BNM("UnityEngine.Screen::get_height")); // #define InitResolveFunc(x, y)
    InitResolveFunc(Screen$$get_width, OBFUSCATE_BNM("UnityEngine.Screen::get_width"));
    if (Screen$$get_height && Screen$$get_width) LOGIBNM(OBFUSCATE_BNM("[set_fov] %dx%d"), Screen$$get_height(), Screen$$get_width());

    uintptr_t (*Camera$$get_main)(); // Можно использовать void *
    float (*Camera$$get_fieldofview)(uintptr_t);
    void (*Camera$$set_fieldofview)(uintptr_t, float);

    InitResolveFunc(Camera$$get_main, OBFUSCATE_BNM("UnityEngine.Camera::get_main"));
    InitResolveFunc(Camera$$set_fieldofview, OBFUSCATE_BNM("UnityEngine.Camera::set_fieldOfView"));
    InitResolveFunc(Camera$$get_fieldofview, OBFUSCATE_BNM("UnityEngine.Camera::get_fieldOfView"));

    if (Camera$$get_main && Camera$$get_fieldofview && Camera$$set_fieldofview) {
        uintptr_t mainCamera = Camera$$get_main();
        if (BNM::IsUnityObjectAlive(mainCamera)) {
            float oldFOV = Camera$$get_fieldofview(mainCamera);
            Camera$$set_fieldofview(mainCamera, value);
            float newFOV = Camera$$get_fieldofview(mainCamera);
            LOGIBNM(OBFUSCATE_BNM("[set_fov] Адрес камеры: %p  |  старое поле зрения: %.2f  |  новое поле зрения: %.2f"), (void *) mainCamera, oldFOV, newFOV);
        } else LOGEBNM(OBFUSCATE_BNM("[set_fov] mainCamera сейчас мертва"));
    }
}

bool setName;
bool parseDict = true;
BNM::LoadClass FPSController;
//! Пример поиска методов и остального
BNM::Method<void *> get_Transform;
BNM::Property<Vector3> transformPosition;
BNM::Method<void> set_position_Injected;
void *myPlayer;
BNM::Field<void *> LocalPlayer; // Поле: public static FPSControler LocalPlayer;
BNM::Field<monoString *> PlayerName; // Поле: private string PlayerName;
BNM::Field<monoDictionary<monoString *, void *> *> Players; // Поле: private static Dictionary<string, FPSControler> Players;
void (*old_Update)(...);
void Update(void *instance) {
    old_Update(instance);

    myPlayer = LocalPlayer();

    if (BNM::IsSameUnityObject(myPlayer, instance)) { // Проверить, одинаковы ли два Unity-объекта (UnityEngine.Object)
        // Делать что-либо
    }

    if (BNM::IsA(myPlayer, FPSController)) { // Проверить класс или тип объекта
        // Делать что-либо
    }

    if (BNM::IsUnityObjectAlive(myPlayer)) { // Проверить, жив ли Unity-объект (UnityEngine.Object)
        //! Установить поле зрения главной камеры на 180°
        set_fov(180.f);

        //! Установить координаты персонажа на 0, 0, 0
        void *myPlayer_Transform = get_Transform(myPlayer);
        transformPosition[myPlayer_Transform] = Vector3(0, 0, 0);
        Vector3 pos(0, 0, 0);
		// Нельзя использовать просто Vector3 в методах, где перед аргументом написано `ref` или `out`, нужен указатель на Vector3
		// Поэтому &pos
        set_position_Injected(myPlayer_Transform, &pos); 

        //! Получить и изменить имя игрока 
        if (!setName) {
            PlayerName[myPlayer]; //! То же самое, что и PlayerName.setInstance(myPlayer); 
            LOGIBNM(OBFUSCATE_BNM("Старое имя myPlayer: %s"), PlayerName()->c_str());
			
            PlayerName = BNM::CreateMonoString(OBFUSCATE_BNM("ByNameModding_Игрок"));
            //! Опаснее, но работает
            // PlayerName = monoString::Create(OBFUSCATE_BNM("ByNameModding_Игрок"));
			
            LOGIBNM(OBFUSCATE_BNM("Новое имя myPlayer: %s"), PlayerName()->c_str());
            setName = true;
        }

        //! Перебрать monoDictionary
        if (parseDict) {
            auto map = Players()->toMap();
            for (auto &it : map)
                if (it.first)
                    LOGIBNM(OBFUSCATE_BNM("Найден игрок: [%s, %p]"), it.first->c_str(), it.second);
            parseDict = false;
        }
    }
}
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
namespace BNM {
    class BNM_ExampleClass : public BNM::UnityEngine::Object {  // Behaviour и MonoBehaviour не содержат полей, поэтому пишем UnityEngine.Object
    // BNM_NewClassInit(имя пространства имен, класс, имя пространства имен родителя, имя родителя);
    BNM_NewClassInit("BNM", BNM_ExampleClass, "UnityEngine", "MonoBehaviour");
        void FixedUpdate();
        void Update();
        void Awake();
        void Start();
        void LateUpdate();
        static void MethodWithGameArgs(void *PhotonPlayer) { // Код может быть здесь
            LOGIBNM(OBFUSCATE_BNM("BNM::BNM_ExampleClass::MethodWithGameArgs вызван с PhotonPlayer = %p!"), PhotonPlayer);
        }
        int FixedFrames = 0;
        int LateFrames = 0;
        int Frames = 0;
    BNM_NewMethodInit(BNM::GetType<void>(), FixedUpdate, 0); // 0 - кол-во параметров
    BNM_NewMethodInit(BNM::GetType<void>(), LateUpdate, 0);
    BNM_NewMethodInit(BNM::GetType<void>(), Update, 0);
    BNM_NewMethodInit(BNM::GetType<void>(), Awake, 0);
    BNM_NewMethodInit(BNM::GetType<void>(), Start, 0);
    BNM_NewStaticMethodInit(BNM::GetType<void>(), MethodWithGameArgs, 1, BNM::GetType(OBFUSCATE_BNM(""), OBFUSCATE_BNM("PhotonPlayer")));
    };
    class BNM_DllExampleClass : public BNM::IL2CPP::Il2CppObject { // Il2CppObject, потому что System.Object, при пустых данных о родителе автоматически выбирается System.Object
    // BNM_NewClassWithDllInit(имя dll БЕЗ `.dll`, имя пространства имен, класс, имя пространства имен родителя, имя родителя);
    BNM_NewClassWithDllInit("mscorlib", "BNM", BNM_DllExampleClass, "", "");
        void Start() {
            LOGIBNM(OBFUSCATE_BNM("BNM::BNM_DllExampleClass::Start вызван!"));
        }
    BNM_NewMethodInit(BNM::GetType<void>(), Start, 0);
    };
}
void BNM::BNM_ExampleClass::Awake() {
    LOGIBNM(OBFUSCATE_BNM("BNM::BNM_ExampleClass::Awake вызван!"));
}
void BNM::BNM_ExampleClass::Start() {
    LOGIBNM(OBFUSCATE_BNM("BNM::BNM_ExampleClass::Start вызван!"));
}
void BNM::BNM_ExampleClass::FixedUpdate() {
    if (FixedFrames == 10)
        LOGIBNM(OBFUSCATE_BNM("BNM::BNM_ExampleClass::FixedUpdate вызван!"));
    FixedFrames++;
    if (FixedFrames == 11) FixedFrames = 0;
}
void BNM::BNM_ExampleClass::LateUpdate() {
    if (LateFrames == 10)
        LOGIBNM(OBFUSCATE_BNM("BNM::BNM_ExampleClass::LateUpdate вызван!"));
    LateFrames++;
    if (LateFrames == 11) LateFrames = 0;
}
void BNM::BNM_ExampleClass::Update() {
    if (Frames == 10)
        LOGIBNM(OBFUSCATE_BNM("BNM::BNM_ExampleClass::Update вызван!"));
    Frames++;
    if (Frames == 11) Frames = 0;
}
void *MyGameObject = nullptr;
//! Создать новый объект
BNM::LoadClass GameObject;
BNM::Method AddComponent;
BNM::Method DontDestroyOnLoad;
void *Example_NewGameObject() {
    void *new_GameObject = GameObject.CreateNewObject();
    AddComponent[new_GameObject](BNM::BNM_ExampleClass::BNMClass.type);
    DontDestroyOnLoad(new_GameObject);
    return new_GameObject;
}
#endif
void (*old_FPS$$ctor)(void*);
void FPS$$ctor(void *instance) {
    old_FPS$$ctor(instance);
    // Делать что-либо
}
void hack_thread() {
    using namespace BNM; // Чтобы не писать BNM:: в этом методе
    do {
        usleep(1);
    } while (!Il2cppLoaded());
	// Требуется, только если использовать std::thread или pthread_create
    // Но здесь, в примере, используется BNM::SetIl2CppLoadEvent
    // AttachIl2Cpp(); // Стабилизация

	//! Создать GameObject и добавить новый класс к нему, и тем самым получить личный Update и остальные методы.
	//! Новые классы работают с AssetBundles!
    GameObject = LoadClass(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("GameObject"));
    AddComponent = GameObject.GetMethodByName(OBFUSCATE_BNM("AddComponent"), 1);
    DontDestroyOnLoad = LoadClass(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Object"))
            .GetMethodByName(OBFUSCATE_BNM("DontDestroyOnLoad"));
    MyGameObject = Example_NewGameObject();

    //! Пример поиска всякой всячины.
    auto Transform = LoadClass(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Transform"));
    auto Component = LoadClass(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Component"));
    FPSController = LoadClass(OBFUSCATE_BNM(""), OBFUSCATE_BNM("FPSController"));

	//! Позволяет подменять методы, которые при использовании базового HOOK вылетают, НО если они вызываются через il2cpp_invoke
	//! Методы, которые вызываются через invoke: .ctor(без аргументов!), ..ctor(), события от Unity (Update и т.п.) и другие методы, созданные компилятором.
	//! Если вызвать этот метод на Update и он перезаписан в дочернем классе, он не будет вызван
    BNM::InvokeHook(FPSController.GetMethodByName(OBFUSCATE_BNM(".ctor")), (void*) FPS$$ctor, (void**)&old_FPS$$ctor);

    PlayerName = FPSController.GetFieldByName(OBFUSCATE_BNM("PlayerName")); // Поля, методы, свойства могут автоматически менять свой тип
    LocalPlayer = FPSController.GetFieldByName(OBFUSCATE_BNM("LocalPlayer"));
    Players = FPSController.GetFieldByName(OBFUSCATE_BNM("Players"));

    get_Transform = Component.GetMethodByName(OBFUSCATE_BNM("get_transform"), 0); // 0 - кол-во аргументов в C#-методе
    transformPosition = Transform.GetPropertyByName(OBFUSCATE_BNM("position"));
    set_position_Injected = Transform.GetMethodByName(OBFUSCATE_BNM("set_position_Injected"), 1);

    HOOK(FPSController.GetMethodByName(OBFUSCATE_BNM("Update"), 0).GetOffset(), Update, old_Update);

    LoadClass Physics = LoadClass(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Physics"));
	
	//! Поиск метода по имени и именам аргументов
    /** 
    В UnityEngine.Physics есть 16 Raycast методов
	Часть имеет одинаковое кол-во аргументов.
    Для примера:
    Нам нужен:
    Raycast(Ray ray, out RaycastHit hitInfo)
    НО LoadClass найдёт по кол-ву аргументов это:
    Raycast(Vector3 origin, Vector3 direction)
    **/
    auto RayCastOffset1 = Physics.GetMethodByName(OBFUSCATE_BNM("Raycast"), {OBFUSCATES_BNM("ray"), OBFUSCATES_BNM("hitInfo")});
    LOGIBNM("RayCastOffset1 указатель: %p", BNM::offsetInLib((void *)RayCastOffset1.GetOffset()));

    /**
	Также можно искать по типам аргументов
    **/
    auto RayCastOffset2 = Physics.GetMethodByName(OBFUSCATE_BNM("Raycast"), {GetType<Ray>(), GetType<RaycastHit>()});
    LOGIBNM("RayCastOffset2 указатель: %p", BNM::offsetInLib((void *)RayCastOffset2.GetOffset()));

    //! Пример поиска вложенного класса
    auto HatManager = LoadClass(OBFUSCATE_BNM(""), OBFUSCATE_BNM("HatManager"));

	// Нельзя создать пространство имён в классе, поэтому в методе можно написать только имя вложенного класса
    auto HatManager_c = HatManager.GetInnerClass(OBFUSCATE_BNM("<>c"));
    LOGIBNM("HatManager_c указатель: %p", HatManager_c.GetIl2CppClass());

	// Требуется, только если использовать std::thread или pthread_create
    // Но здесь, в примере, используется BNM::SetIl2CppLoadEvent
    // DetachIl2Cpp(); // Стабилизация
}

// Пример использования BNM::HardBypass
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
	// BNM может работать без HardBypass, но нужно загрузить ваш код до загрузки игры, для обхода защит
    BNM::HardBypass(env);
    return JNI_VERSION_1_6;
}

#include <thread>
[[maybe_unused]] __attribute__((constructor))
void lib_main() {
	// Метод, установленный здесь, будет вызван после завершения il2cpp_init
    BNM::SetIl2CppLoadEvent(hack_thread);
    // или
    // std::thread(hack_thread).detach();
}