# ByNameModding
Modding (hacking) il2cpp games by classes, methods, fields names.
# Status: Work in progres
# Bugs:
### Everything is fixed. but it is not exactly :)
# File structure:
  + ## Class LoadClass
     + ### Methods:
       + LoadClass(const char *namespce, const char *clazz, const char *dllname - optional)
       + GetFieldInfoByName(const char *name)
       + GetFieldByName(const char *name)
       + GetFieldOffset(const char *name or Fieldinfo *filed)
       + GetMethodInfoByName(const char *name, int paramcount)
       + GetMethodOffsetByName(const char *name, int paramcoun)
   + ## Class Field
     + ### Methods:
       + Field(FieldInfo *thiz, void *clas - optional for static)
       + get_offset()
       + get()
       + set(T val)
     + ### Fields:
       + init
       + thread_static
       + clazz
  + ## void * get_Method(const char *str) 
# Usage
## get_method example
```c++
/* get_method: edit fov example
* code from here
* Il2CppResolver
* https://github.com/MJx0/IL2CppResolver/blob/master/Android/test/src/demo.cpp
* MJx0's IL2CppResolver doesn't work
* get_method working ONLY with extren methods
*/
void *set_fov(float value) {
    int (*Screen$$get_height)();
    int (*Screen$$get_width)();
    InitResolveFunc(Screen$$get_height, OBFUSCATE("UnityEngine.Screen::get_height()")); // #define InitResolveFunc(x, y)
    InitResolveFunc(Screen$$get_width, OBFUSCATE("UnityEngine.Screen::get_width()"));// Don't forgot about OBFUSCATE
    if (Screen$$get_height && Screen$$get_width) {
        LOGI(OBFUSCATE("%dx%d"), Screen$$get_height(), Screen$$get_width());
    }

    uintptr_t (*Camera$$get_main)(); // you can use void *
    float (*Camera$$get_fieldofview)(uintptr_t);
    void (*Camera$$set_fieldofview)(uintptr_t, float);

    InitResolveFunc(Camera$$get_main, OBFUSCATE("UnityEngine.Camera::get_main()"));
    InitResolveFunc(Camera$$set_fieldofview, OBFUSCATE("UnityEngine.Camera::set_fieldOfView(System.Single)"));
    InitResolveFunc(Camera$$get_fieldofview, OBFUSCATE("UnityEngine.Camera::get_fieldOfView()"));

    if (Camera$$get_main && Camera$$get_fieldofview && Camera$$set_fieldofview) {
        uintptr_t mainCamera = Camera$$get_main();
        if (mainCamera != 0) {
            float oldFOV = Camera$$get_fieldofview(mainCamera);
            Camera$$set_fieldofview(mainCamera, value);
            float newFOV = Camera$$get_fieldofview(mainCamera);
            LOGI(OBFUSCATE("Camera Ptr: %p  |  oldFOV: %.2f  |  newFOV: %.2f"), (void *) mainCamera, oldFOV,
                 newFOV);
        } else {
            LOGE(OBFUSCATE("mainCamera is currently not available!"));
        }
    }
}
```
## LoadClass and Field exampels
```c++
void *(*get_Transform)(void *instance);
void (*set_position)(void *Transform, Vector3);
void *myPlayer;
void (*old_Update)(void *instance);
void Update(void *instance){
    old_Update(instance);
    if (instance){
        /** We have public static FPSControler LocalPlayer; **/
        FieldBN(localpalyer, void *, 0, "", "FPSControler", "LocalPlayer", 'z') // #define FieldBN(myfield, type, inst, nameSpacec, clazzz, fieldName, key)
        myPlayer = localpalyer; // or myPlayer = localpalyer();
        void *myPlayer_Transform = get_Transform(myPlayer);
        set_position(myPlayer_Transform, Vector3(0, 0, 0);
    }
}
void *hack_thread(void *) {
    do {
        sleep(1);
    } while (!isLibraryLoaded(libName));
    auto *Transform = new LoadClass(OBFUSCATE("UnityEngine"), OBFUSCATE("Transform"));
    auto *Component = new LoadClass(OBFUSCATE("UnityEngine"), OBFUSCATE("Component"));
    InitFunc(get_Transform, Component->GetMethodOffsetByName(OBFUSCATE("get_transform"), 0); // 0 - parametrs count in original c# method
    InitFunc(set_position, Transform->GetMethodOffsetByName(OBFUSCATE("set_position_Injected"), 0); // set_position working badly
    MSHookFunction((void *)(new LoadClass(OBFUSCATE_KEY("", 'i'),
                        OBFUSCATE("FPSControler")))->GetMethodOffsetByName(
            OBFUSCATE_KEY("Update", '|'), 0), (void *) Update, (void **) &old_Update);
    
}
```
