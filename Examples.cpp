#include "ByNameModding.h"

/* get_method: edit fov example
* code from here
* Il2CppResolver
* https://github.com/MJx0/IL2CppResolver/blob/master/Android/test/src/demo.cpp
* MJx0's IL2CppResolver doesn't work
* get_method working ONLY with extren unity methods
*/
void *set_fov(float value) {
    int (*Screen$$get_height)();
    int (*Screen$$get_width)();
    InitResolveFunc(Screen$$get_height, "UnityEngine.Screen::get_height()"); // #define InitResolveFunc(x, y)
    InitResolveFunc(Screen$$get_width, "UnityEngine.Screen::get_width()");
    if (Screen$$get_height && Screen$$get_width) {
        LOGI("%dx%d", Screen$$get_height(), Screen$$get_width());
    }

    uintptr_t (*Camera$$get_main)(); // you can use void *
    float (*Camera$$get_fieldofview)(uintptr_t);
    void (*Camera$$set_fieldofview)(uintptr_t, float);

    InitResolveFunc(Camera$$get_main, "UnityEngine.Camera::get_main()");
    InitResolveFunc(Camera$$set_fieldofview, "UnityEngine.Camera::set_fieldOfView(System.Single)");
    InitResolveFunc(Camera$$get_fieldofview, "UnityEngine.Camera::get_fieldOfView()");

    if (Camera$$get_main && Camera$$get_fieldofview && Camera$$set_fieldofview) {
        uintptr_t mainCamera = Camera$$get_main();
        if (IsNativeObjectAlive(mainCamera)) {
            float oldFOV = Camera$$get_fieldofview(mainCamera);
            Camera$$set_fieldofview(mainCamera, value);
            float newFOV = Camera$$get_fieldofview(mainCamera);
            LOGI("Camera Ptr: %p  |  oldFOV: %.2f  |  newFOV: %.2f", (void *) mainCamera, oldFOV,
                 newFOV);
        } else {
            LOGE("mainCamera is currently not available!");
        }
    }
}


//! Create new class exampele
LoadClass GameObject;
LoadClass ExampleClass;
void *NewExampleClass(){
    void *new_GameObject = GameObject.CreateNewClass({}, 0); // No args
    bool ExampleArg1 = false;
    float ExampleArg2 = 0.f;
    void* args[] = {&ExampleArg1, &ExampleArg2, new_GameObject};
    void *new_ExampleClass = ExampleClass.CreateNewClass(args, 0, {"boolean", "value", "gameObject"}); // 3 args
    return new_ExampleClass;
}

//! Find exampele
bool setName;
bool parseLst;
void *(*get_Transform)(void *instance);
void (*set_position)(void *Transform, Vector3 pos);
void *myPlayer;
void (*old_Update)(...);
void Update(void *instance){
    old_Update(instance);
	
    /** We have: public static FPSControler LocalPlayer; **/
    myPlayer = FieldBN(void *, 0, "", "FPSControler", "LocalPlayer");
	
	if (IsNativeObjectAlive(myPlayer)) { // Normal check is unity Object (UnityEngine.Object) not null
	
		//! Set player pos to 0, 0, 0
		void *myPlayer_Transform = get_Transform(myPlayer);
		set_position(myPlayer_Transform, Vector3(0, 0, 0));
		
		//! Get and Set palyer name
		if (!setName){
			auto nameF = FieldBN(monoString *, myPlayer, "", "FPSControler", "Name");
			LOGI("myPlayer old name is %s", nameF()->get_string().c_str());
			/* monoString::CreateMethod::MONO - using System.String.CreateString(sbyte *value)
			 * monoString::CreateMethod::IL2CPP - using il2cpp_string_new
			 * monoString::CreateMethod::C_LIKE - C like creating
			 * C_LIKE - Defalut
			*/
			nameF = CreateMonoString("ByNameModding_Player", monoString::CreateMethod::C_LIKE); 
			std::string newName = nameF; // operator std::string()
			LOGI("myPlayer new name is %s", newName.c_str());
			setName = true;
		}
		
		//! Parse monoList
		if (!parseLst){
			auto Lst = FieldBN(monoList<monoString *> *, myPlayer, "", "FPSControler", "AllPlayersNames")->toCPPList();
			for (auto name : Lst){
				if (name)
					LOGI("Found Player name %s", name->get_string().c_str());
			}
			parseLst = false;
		}
	}
}
namespace geokar2006 {
    class BNM_ExampleClass {
	//NewClassInit(namespace, class, parent class namespace, parent class name, parent class size); // for System.Object only sizeof(Il2CppObject)
    NewClassInit(geokar2006, BNM_ExampleClass, "UnityEngine", "MonoBehaviour", sizeof(Il2CppObject) + sizeof(void *));
        void FixedUpdate();
        void Update();
        void Awake();
        void Start();
        void LateUpdate();
		int FixedFrames = 0;
		int LateFrames = 0;
		int Frames = 0;
    NewMethodInit(GetType<void>(), FixedUpdate, 0); // 0 - args count 
    NewMethodInit(GetType<void>(), LateUpdate, 0);
    NewMethodInit(GetType<void>(), Update, 0);
    NewMethodInit(GetType<void>(), Awake, 0);
    NewMethodInit(GetType<void>(), Start, 0);
    };
}
void geokar2006::BNM_ExampleClass::Awake(){
	LOGI("geokar2006::BNM_ExampleClass::Awake Called!");
}
void geokar2006::BNM_ExampleClass::Start(){
	LOGI("geokar2006::BNM_ExampleClass::Start Called!");
}
void geokar2006::BNM_ExampleClass::FixedUpdate(){
	if (FixedFrames == 10)
		LOGI("geokar2006::BNM_ExampleClass::FixedUpdate Called!");
	FixedFrames++;
	if (FixedFrames == 11) FixedFrames = 0;
}
void geokar2006::BNM_ExampleClass::LateUpdate(){
	if (LateFrames == 10)
		LOGI("geokar2006::BNM_ExampleClass::LateUpdate Called!");
	LateFrames++;
	if (LateFrames == 11) LateFrames = 0;
}
void geokar2006::BNM_ExampleClass::Update(){
	if (Frames == 10)
		LOGI("geokar2006::BNM_ExampleClass::Update Called!");
	Frames++;
	if (Frames == 11) Frames = 0;
}
void *hack_thread(void *) {
    do {
        sleep(1);
    } while (!isLibraryLoaded(libName));
	sleep(2); // Il2cpp can be not initialized
	
	InitNewClasses(); // Add new clases to il2cpp 
	
    //! Create new instance of class exampele
    GameObject = LoadClass("UnityEngine", "GameObject");
    ExampleClass = LoadClass("", "ExampleClass");
	
    //! Find exampele
    auto Transform = LoadClass("UnityEngine", "Transform");
    auto Component = LoadClass("UnityEngine", "Component");
    auto FPSControler = LoadClass("", "FPSControler");
	
    InitFunc(get_Transform, Component.GetMethodOffsetByName("get_transform", 0)); // 0 - parameters count in original c# method
    InitFunc(set_position,  Transform.GetMethodOffsetByName("set_position", 1); // For Injected methods use IVector3 IVector2 IQuaternion ("Injected" structs can be converted to normal) Quartenion a = IQuartenion(2, 14, 13, 0);
	
    HOOK(FPSControler.GetMethodOffsetByName("Update", 0), Update, old_Update); // ByNameModding HOOK lambda
	
	
    //! Find metohod by name and parameters names
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
    auto RayCastOffset1 = LoadClass("UnityEngine", "Physics").GetMethodOffsetByName("Raycast", {"ray", "hitInfo"});
    /**
	Or you can find by parameters type
	**/
	auto RayCastOffset2 = LoadClass("UnityEngine", "Physics").GetMethodOffsetByName("Raycast", {GetType<Ray>().ToIl2CppType(), GetType<RaycastHit>().ToIl2CppType()});
    
	/**
	Or all together
	**/
	auto RayCastOffset3 = LoadClass("UnityEngine", "Physics").GetMethodOffsetByName("Raycast", {"ray", "hitInfo"}, {GetType<Ray>().ToIl2CppType(), GetType<RaycastHit>().ToIl2CppType()});
    
    //! Find Class by name and method name
    //! GetLC_ByClassAndMethodName
    /**
    Example from among us:
    We have HatManager class and <>c class in it.
    To get in il2cpp 
    In Il2Cpp the class is named not like this:
    HatManager.<>c
    Like this:
    <>c
    And to Get it you need use:
    GetLC_ByClassAndMethodName
    **/
    // Then you can get any method
    auto HatManager_c = LoadClass::GetLC_ByClassAndMethodName("", "<>c", "<GetUnlockedHats>b__10_0");
	
	
	return nullptr;
}