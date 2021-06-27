# ByNameModding
Modding (hacking) il2cpp games by classes, methods, fields names.

# Information
Status: **Ready to use** <br/>
Bugs: **Everything is fixed. but it is not exactly :)** <br/>

# Classes and files structure:
```cpp
class LoadClass {
	LoadClass(const char *namespce, const char *clazz, const char *dllname [optional], bool _new = false [optional]);
   
	LoadClass(Il2CppClass *clazz, bool _new = false [optional]);
   
	FieldInfo *GetFieldInfoByName(const char *name);
   
	template<typename T> Field<T> GetFieldByName(const char *name, void *inst = NULL [optional]);
   
	DWORD GetFieldOffset(const char *name);
  
	static DWORD GetOffset(FieldInfo *filed or MethodInfo *methodInfo);
   
	MethodInfo *GetMethodInfoByName(const char *name, int paramcount);
  
	DWORD GetMethodOffsetByName(const char *name, int paramcoun);
  
	template<typename T> static T Object_Unbox(T obj);
  
	template<typename T> static T Object_Box_My(T obj);
}
template<typename T>
class Field : FieldInfo {
	static bool CheckStaticFieldInfo(FieldInfo *fieldInfo);
  
	bool init;
	bool thread_static;
	bool is_instance;
	void *instance;
  
	Field(FieldInfo *thiz, void *_instance [optional for static]);
  
	DWORD get_offset();
  
	T get();
  
	void set(T val);
}
struct Metadata {
	void *file;
	const Il2CppGlobalMetadataHeader *header;
	const Il2CppMetadataRegistration *registartion;
	const Il2CppCodeRegistration *Il2CppRegistration;
	uint32_t version;
	int32_t ImagesCount;
	const Il2CppImageDefinition *ImageDefinitionTable;
	const Il2CppTypeDefinition *TypeDefinitionTable;
	const Il2CppFieldDefinition *FieldDefinitionTable;
	const Il2CppMethodDefinition *MethodDefinitionTable;
	const Il2CppParameterDefinition *ParameterDefinitionTable;
	Il2CppImage *ImagesTable;
	Il2CppClass **TypeInfoDefinitionTable;
	Il2CppClass **TypeInfoTable;
	
	template<typename T> static T MetadataOffset(void *metadata, size_t sectionOffset, size_t itemIndex);
	
	const Il2CppType *GetIl2CppTypeFromIndex(TypeIndex index);
	
	const Il2CppImageDefinition *getImageDefinitionByIndex(ImageIndex index);
	
	const Il2CppTypeDefinition *getTypeDefinitionByIndex(TypeDefinitionIndex index);
	
	const Il2CppFieldDefinition *getFieldDefinitionByIndex(FieldIndex index);
	
	const Il2CppMethodDefinition *getMethodDefinitionByIndex(MethodIndex index);
	
	const Il2CppParameterDefinition *getParameterDefinitionByIndex(ParameterIndex index);
	
	Il2CppClass *GetTypeInfoFromTypeDefinitionIndex(TypeDefinitionIndex index);
	
	const char *GetStringFromIndex(StringIndex index);
	
	MethodInfo *GetMethodInfoFromIndex(MethodIndex index);
	
	const Il2CppImage *GetImageForTypeDefinitionIndex(TypeDefinitionIndex index);
	
	const TypeDefinitionIndex GetIndexForTypeDefinition(const Il2CppClass *typeDefinition);
	
	FieldInfo *GetFieldInfoFromIndexAndClass(FieldIndex index, Il2CppClass *klass);
	
	Il2CppClass *FromTypeDefinition(TypeDefinitionIndex index);
	
	static uint8_t ConvertPackingSizeEnumToValue(PackingSize packingSize);
	
	InvokerMethod GetMethodInvoker(const Il2CppImage *image, uint32_t token);
	
	const Il2CppMethodDefinition *GetMethodDefinitionFromIndex(MethodIndex index);
	
	Il2CppMethodPointer GetMethodPointer(const Il2CppImage *image, uint32_t token);
}

struct LibInfo {
	uintptr_t startAddr;
	uintptr_t endAddr;
	intptr_t size;
	std::string path;
};
struct MetaDataUtils {
	static void *LoadMetadataDat();
	
	static const Il2CppGlobalMetadataHeader *LoadMetadataDatHeader();
	
	static Metadata *CrateMetadata();
	
	static const char *getFirstPattern();
	
	static void *getRegister();
}
const char *readFileTxt(const char *myFile);

// Some modified version of this:
// https://github.com/Jupiops/NEWQuizDuelHookLibrary/blob/master/x64/jni/include/utils.h
DWORD findPatternInMem(const char *pattern, uintptr_t start, intptr_t size, uintptr_t libBase)

std::string readHexStrFromMem(const void *addr, size_t len); // For armv7 finder

LibInfo GetLibInfo(const char *libraryName);

//https://github.com/4ch12dy/XB1nLib/blob/master/XB1nLib.c#L29
void *MapFile2Mem(const char *file_path);

void *get_Method(const char *str);

// For arm64 or x86 or if armv7 finder don't work
DWORD MetadataRegistrationOffset = 0x0;
DWORD Il2CppRegistrationOffset = 0x0;

void *get_il2cpp();

template<typename T> bool isNOT_Allocated(T x);

template<typename T> T CheckObj(T obj);

std::string revhexstr(std::string hex); // Revers hex string

std::string fixhex(std::string str); // Fix hex string

std::string DWORD2HexStr(DWORD hex);

DWORD HexStr2DWORD(std::string hex);

DWORD getOffsetFromB_Hex(std::string hex, DWORD offset, bool idk = false); // For armv7 finder

DWORD abs (DWORD val);
```
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
        if (mainCamera != 0) {
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
```
## LoadClass and Field exampels
```c++
void *(*get_Transform)(void *instance);
void (*set_position)(void *Transform, Vector3);
void *myPlayer;
void (*old_Update)(void *instance);
void Update(void *instance){
    old_Update(instance);
    /** We have: public static FPSControler LocalPlayer; **/
    myPlayer = FieldBNC(void *, 0, "", "FPSControler", "LocalPlayer");
    void *myPlayer_Transform = get_Transform(myPlayer);
    set_position(myPlayer_Transform, Vector3(0, 0, 0));
}
void *instance_from_IEnumerator;
bool (*old_MainLoop$$MoveNext)(void *instance);
bool MainLoop$$MoveNext(void *instance) {
    LOGI("MainLoop$$MoveNext");
	instance_from_IEnumerator = FieldBNC(void *, instance, "", "<MainLoop>d__1", "<>4__this");
    return old_MainLoop$$MoveNext(instance);
}

void *hack_thread(void *) {
    do {
        sleep(1);
    } while (!isLibraryLoaded(libName));
    auto Transform = LoadClass("UnityEngine", "Transform");
    auto Component = LoadClass("UnityEngine", "Component");
    auto FPSControler = LoadClass("", "FPSControler");
	
    InitFunc(get_Transform, Component.GetMethodOffsetByName("get_transform", 0)); // 0 - parametrs count in original c# method
    InitFunc(set_position,  Transform.GetMethodOffsetByName("set_position_Injected", 1); // set_position does not work well, so we use set_position_Injected
	
    MSHookFunction((void *)FPSControler.GetMethodOffsetByName("Update", 0), (void *) Update, (void **) &old_Update);
	
    /** Class generated by IEnumerator MainLoop(); **/
    auto MainLoop_d1 = LoadClass(OBFUSCATE(""), OBFUSCATE("<MainLoop>d__1"), true); // true - new method to find class
    // Only using new method you can get class with CompilerGeneratedAttribute
	
    MSHookFunction((void *)MainLoop_d1.GetMethodOffsetByName(OBFUSCATE("MoveNext"), 0), (void *) MainLoop$$MoveNext, (void **) &old_MainLoop$$MoveNext);
}
```
