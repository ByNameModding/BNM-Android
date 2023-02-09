#include "ByNameModding/BNM.hpp"
using namespace BNM::UNITY_STRUCTS; // Vector3, Vector2 and etc
using namespace BNM::MONO_STRUCTS; // monoString, monoArray and etc

/* getExternMethod: edit fov example
* code from here
* Il2CppResolver
* https://github.com/kp7742/IL2CppResolver/blob/master/Android/test/src/demo.cpp
* MJx0's IL2CppResolver doesn't work in all unity versions
* getExternMethod working ONLY with extern methods
*/
void set_fov(float value) {
    int (*Screen$$get_height)();
    int (*Screen$$get_width)();
    InitResolveFunc(Screen$$get_height, OBFUSCATE_BNM("UnityEngine.Screen::get_height")); // #define InitResolveFunc(x, y)
    InitResolveFunc(Screen$$get_width, OBFUSCATE_BNM("UnityEngine.Screen::get_width"));
    if (Screen$$get_height && Screen$$get_width) LOGIBNM(OBFUSCATE_BNM("[set_fov] %dx%d"), Screen$$get_height(), Screen$$get_width());

    uintptr_t (*Camera$$get_main)(); // you can use void *
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
            LOGIBNM(OBFUSCATE_BNM("[set_fov] Camera Ptr: %p  |  oldFOV: %.2f  |  newFOV: %.2f"), (void *) mainCamera, oldFOV, newFOV);
        } else LOGEBNM(OBFUSCATE_BNM("[set_fov] mainCamera is currently not available!"));
    }
}

bool setName;
bool parseDict = true;
BNM::LoadClass FPSController;
//! Find example
BNM::Method<void *> get_Transform;
BNM::Property<Vector3> transformPosition;
BNM::Method<void> set_position_Injected;
void *myPlayer;
BNM::Field<void *> LocalPlayer; // public static FPSControler LocalPlayer;
BNM::Field<monoString *> PlayerName; // private String PlayerName;
BNM::Field<monoDictionary<monoString *, void *> *> Players; // private static Dictionary<String, FPSControler> Players;
void (*old_Update)(...);
void Update(void *instance) {
    old_Update(instance);

    myPlayer = LocalPlayer();

    if (BNM::IsSameUnityObject(myPlayer, instance)) { // Check is unity object (UnityEngine.Object) are same
        // Do sth
    }

    if (BNM::IsA(myPlayer, FPSController)) { // Check is object instantiated from special class or type
        // Do sth
    }

    if (BNM::IsUnityObjectAlive(myPlayer)) { // Check is unity object (UnityEngine.Object) not null
        //! set main camera fov to 180
        set_fov(180.f);

        //! Set player pos to 0, 0, 0
        void *myPlayer_Transform = get_Transform(myPlayer);
        transformPosition[myPlayer_Transform] = Vector3(0, 0, 0);
        Vector3 pos(0, 0, 0);
        set_position_Injected(myPlayer_Transform, &pos); // You can't use Vector3 in Injected, you need pointer to Vector3

        //! Get and Set player name
        if (!setName) {
            PlayerName[myPlayer]; //! Same as PlayerName.setInstance(myPlayer); but less code
            LOGIBNM(OBFUSCATE_BNM("myPlayer old name is %s"), PlayerName()->c_str());
            PlayerName = BNM::CreateMonoString(OBFUSCATE_BNM("ByNameModding_Player"));
            //! Less safety but work
            // PlayerName = monoString::Create(OBFUSCATE_BNM("ByNameModding_Player"));
            LOGIBNM(OBFUSCATE_BNM("myPlayer new name is %s"), PlayerName()->c_str());
            setName = true;
        }

        //! Parse monoDictionary
        if (parseDict) {
            auto map = Players()->toMap();
            for (auto &it : map)
                if (it.first)
                    LOGIBNM(OBFUSCATE_BNM("Found Player: [%s, %p]"), it.first->c_str(), it.second);
            parseDict = false;
        }
    }
}
#if __cplusplus >= 201703 && !BNM_DISABLE_NEW_CLASSES
namespace geokar2006 {
    class BNM_ExampleClass : public BNM::UnityEngine::Object {  // Behaviour, MonoBehaviour don't contain fields, therefore, you can use UnityEngine.Object
    // BNM_NewClassInit(namespace, class, parent class namespace, parent class name);
    BNM_NewClassInit("geokar2006", BNM_ExampleClass, "UnityEngine", "MonoBehaviour");
        void FixedUpdate();
        void Update();
        void Awake();
        void Start();
        void LateUpdate();
        static void MethodWithGameArgs(void *PhotonPlayer) { // Code can by here
            LOGIBNM(OBFUSCATE_BNM("geokar2006::BNM_ExampleClass::PhotonPlayer Called with (%p) as PhotonPlayer!"), PhotonPlayer);
        }
        int FixedFrames = 0;
        int LateFrames = 0;
        int Frames = 0;
    BNM_NewMethodInit(BNM::GetType<void>(), FixedUpdate, 0); // 0 - args count
    BNM_NewMethodInit(BNM::GetType<void>(), LateUpdate, 0);
    BNM_NewMethodInit(BNM::GetType<void>(), Update, 0);
    BNM_NewMethodInit(BNM::GetType<void>(), Awake, 0);
    BNM_NewMethodInit(BNM::GetType<void>(), Start, 0);
    BNM_NewStaticMethodInit(BNM::GetType<void>(), MethodWithGameArgs, 1, BNM::GetType(OBFUSCATE_BNM(""), OBFUSCATE_BNM("PhotonPlayer")));
    };
    class BNM_DllExampleClass : public BNM::IL2CPP::Il2CppObject { // Il2CppObject - due System.Object, null parent class namespace and parent class name = System.Object
    // BNM_NewClassWithDllInit(dll, namespace, class, parent class namespace (maybe ""), parent class name (maybe ""));
    BNM_NewClassWithDllInit("mscorlib", "geokar2006", BNM_DllExampleClass, "", "");
        void Start() {
            LOGIBNM(OBFUSCATE_BNM("geokar2006::BNM_DllExampleClass::Start Called!"));
        }
    BNM_NewMethodInit(BNM::GetType<void>(), Start, 0);
    };
}
void geokar2006::BNM_ExampleClass::Awake() {
    LOGIBNM(OBFUSCATE_BNM("geokar2006::BNM_ExampleClass::Awake Called!"));
}
void geokar2006::BNM_ExampleClass::Start() {
    LOGIBNM(OBFUSCATE_BNM("geokar2006::BNM_ExampleClass::Start Called!"));
}
void geokar2006::BNM_ExampleClass::FixedUpdate() {
    if (FixedFrames == 10)
        LOGIBNM(OBFUSCATE_BNM("geokar2006::BNM_ExampleClass::FixedUpdate Called!"));
    FixedFrames++;
    if (FixedFrames == 11) FixedFrames = 0;
}
void geokar2006::BNM_ExampleClass::LateUpdate() {
    if (LateFrames == 10)
        LOGIBNM(OBFUSCATE_BNM("geokar2006::BNM_ExampleClass::LateUpdate Called!"));
    LateFrames++;
    if (LateFrames == 11) LateFrames = 0;
}
void geokar2006::BNM_ExampleClass::Update() {
    if (Frames == 10)
        LOGIBNM(OBFUSCATE_BNM("geokar2006::BNM_ExampleClass::Update Called!"));
    Frames++;
    if (Frames == 11) Frames = 0;
}
void *MyGameObject = nullptr;
//! Create new object
BNM::LoadClass GameObject;
BNM::Method AddComponent;
BNM::Method DontDestroyOnLoad;
void *Example_NewGameObject() {
    void *new_GameObject = GameObject.CreateNewObject();
    AddComponent[new_GameObject](geokar2006::BNM_ExampleClass::BNMClass.type);
    DontDestroyOnLoad(new_GameObject);
    return new_GameObject;
}
#endif
void (*old_FPS$$ctor)(void*);
void FPS$$ctor(void *instance) {
    old_FPS$$ctor(instance);
    // Do sth
}
void hack_thread() {
    using namespace BNM; // You can use namespace in methods
    do {
        usleep(1);
    } while (!Il2cppLoaded());
    AttachIl2Cpp(); // Stabilization

    //! Create GameObject and add new class to it and get your own update and other methods
    //! New classes work with AssetBundles too!
    GameObject = LoadClass(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("GameObject"));
    AddComponent = GameObject.GetMethodByName(OBFUSCATE_BNM("AddComponent"), 1);
    DontDestroyOnLoad = LoadClass(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Object"))
            .GetMethodByName(OBFUSCATE_BNM("DontDestroyOnLoad"));
    MyGameObject = Example_NewGameObject();

    //! Find example
    auto Transform = LoadClass(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Transform"));
    auto Component = LoadClass(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Component"));
    FPSController = LoadClass(OBFUSCATE_BNM(""), OBFUSCATE_BNM("FPSController"));

    //! Allow hook methods that crash game using basic HOOK, but only if method called by il2cpp_invoke
    //! Methods that called by invoke: .ctor(no args!), ..ctor(), unity events like Update and some other methods that generated by compiler
    //! If you hook Update using this and in child class it overwritten it won't be called
    BNM::InvokeHook(FPSController.GetMethodByName(OBFUSCATE_BNM(".ctor")), (void*) FPS$$ctor, (void**)&old_FPS$$ctor);

    PlayerName = FPSController.GetFieldByName(OBFUSCATE_BNM("PlayerName")); // Field, Methods, Properties can automatically cast type
    LocalPlayer = FPSController.GetFieldByName(OBFUSCATE_BNM("LocalPlayer"));
    Players = FPSController.GetFieldByName(OBFUSCATE_BNM("Players"));

    get_Transform = Component.GetMethodByName(OBFUSCATE_BNM("get_transform"), 0); // 0 - parameters count in original c# method
    transformPosition = Transform.GetPropertyByName(OBFUSCATE_BNM("position"));
    set_position_Injected = Transform.GetMethodByName(OBFUSCATE_BNM("set_position_Injected"), 1);

    HOOK(FPSController.GetMethodByName(OBFUSCATE_BNM("Update"), 0).GetOffset(), Update, old_Update); // ByNameModding HOOK lambda

    LoadClass Physics = LoadClass(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("Physics"));
    //! Find method by name and parameters names
    /** 
    In UnityEngine.Physics we have 16 Raycast methods
    Some have the same number of parameters.
    For example:
    We need:
    Raycast(Ray ray, out RaycastHit hitInfo)
    but LoadClass finds by number of parameters:
    Raycast(Vector3 origin, Vector3 direction)
    Now this is not a problem.
    **/
    auto RayCastOffset1 = Physics.GetMethodByName(OBFUSCATE_BNM("Raycast"), {OBFUSCATES_BNM("ray"), OBFUSCATES_BNM("hitInfo")});
    LOGIBNM("RayCastOffset1 ptr: %p", BNM::offsetInLib((void *)RayCastOffset1.GetOffset()));

    /**
    Or you can find by parameters type
    **/
    auto RayCastOffset2 = Physics.GetMethodByName(OBFUSCATE_BNM("Raycast"), {GetType<Ray>().ToIl2CppType(), GetType<RaycastHit>().ToIl2CppType()});
    LOGIBNM("RayCastOffset2 ptr: %p", BNM::offsetInLib((void *)RayCastOffset2.GetOffset()));

    /**
    Or all together
    **/
    auto RayCastOffset3 = Physics.GetMethodByName(OBFUSCATE_BNM("Raycast"), {OBFUSCATES_BNM("ray"), OBFUSCATES_BNM("hitInfo")}, {GetType<Ray>().ToIl2CppType(), GetType<RaycastHit>().ToIl2CppType()});
    LOGIBNM("RayCastOffset3 ptr: %p", BNM::offsetInLib((void *)RayCastOffset3.GetOffset()));

    //! Get Inner class example
    auto HatManager = LoadClass(OBFUSCATE_BNM(""), OBFUSCATE_BNM("HatManager"));

    // You can't make namespace in class due that, method has only class name
    auto HatManager_c = HatManager.GetInnerClass(OBFUSCATE_BNM("<>c"));
    LOGIBNM("HatManager_c ptr: %p", HatManager_c.GetIl2CppClass());

    DetachIl2Cpp(); // Stabilization
}

// BNM::HardBypass example
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    BNM::HardBypass(env); // BNM can work without this, but need to load lib before game loads to bypass some protections
    return JNI_VERSION_1_6;
}

#include <thread>
[[maybe_unused]] __attribute__((constructor))
void lib_main() {
    // The method will be called when il2cpp_init completes
    BNM::SetIl2CppLoadEvent(hack_thread);
    // Or
    // std::thread(hack_thread).detach();
}