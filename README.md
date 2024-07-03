## ByNameModding
#### English | [Русский](https://github.com/ByNameModding/BNM-Android/blob/master/README_RU.md)
ByNameModding is a library for modding il2cpp games by classes, methods, field names on Android. This edition is focused on working on Android with il2cpp. It includes everything you need for modding unity games.<br>
Requires C++20 minimum.

### What you can do with BNM?
+ [Finding everything by names](examples/01_Basics.cpp) (classes, methods, fields ane etc.)
+ Finding [Generic](examples/03_Generic.cpp) classes and methods.
+ Adding [your own classes](examples/05_ClassesManagement.cpp) to game that work even with AssetBundles!
+ [Change game classes](examples/05_ClassesManagement.cpp) by addeding fields and methods.
+ Use [basic C# structs](examples/02_OtherStructures.cpp) like string (Mono::String), array (Mono::Array), dictionary (Mono::Dictionary) that fully work.

### New classes
BNM can add your own class to game, it requires C++20 and above.<br>
If you nest your class from MonoBehaviour (or other class with events) methods like `Update` will work.<br>
And it fully computable with AssetBundles!<br>
For bundles need add your class to dlls that listed in ScriptingAssemblies.json.<br>
Basically BNM add classes to `Assembly-CSharp.dll` that always in this list.<br>
If you want to use your own dll with bundles, add it to this file, but this will work only if you use BNM internally.<br>

## Getting Started
+ First clone the repo and add it to your project.
+ Set C++ version to 20 or higher
+ add the code below depending on what you are using:<br>
    Android.mk
    ```mk
    BNM_PATH := $(LOCAL_PATH)/path/to/ByNameModding
    LOCAL_C_INCLUDES += $(BNM_PATH)/include $(BNM_PATH)/external/include
    LOCAL_STATIC_LIBRARIES += BNM
    # ...
    include $(BUILD_SHARED_LIBRARY)
    # ...
    include $(CLEAR_VARS)
    include $(BNM_PATH)/Android.mk
    ```
    CMakeLists.txt
    ```cmake
    add_subdirectory(path/to/ByNameModding EXCLUDE_FROM_ALL)
    get_property(BNM_INCLUDE_DIRECTORIES TARGET BNM PROPERTY BNM_INCLUDE_DIRECTORIES)
	
    # ...

    target_include_directories(
        # Your lib name
        PUBLIC
        ${BNM_INCLUDE_DIRECTORIES}
        # ...
    )
    target_link_libraries(
        # Your lib name
        PUBLIC
        BNM
        # ...
    )
    ```
+ Change `UNITY_VER`, include your string obfuscator and hooking software, uncomment `BNM_DOTNET35` if the game uses .NET 3.5 in [BNM_settings.hpp](ByNameModding/BNM_settings.hpp)
+ Done! See [examples](Examples) or add it to your project to understand how to work with BNM.

## Supported Unity versions: 5.6.4f1, 2017.x - 6000.0.x

## Dependencies
[UTF8-CPP](https://github.com/nemtrif/utfcpp) used by il2cpp and by BNM too.<br>
[Open-hierarchy custom RTTI](https://github.com/royvandam/rtti/tree/cf0dee6fb3999573f45b0726a8d5739022e3dacf) used to optimize memory usage
### Android hookinng software for example:
[Dobby](https://github.com/jmpews/Dobby)<br>
[ShadowHook](https://github.com/bytedance/android-inline-hook)<br>
[Substrate](https://github.com/jbro129/Unity-Substrate-Hook-Android/tree/master/C%2B%2B/Substrate) with [And64InlineHook](https://github.com/Rprop/And64InlineHook) - do not support unhook