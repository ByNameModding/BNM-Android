## ByNameModding
#### English | [Русский](https://github.com/ByNameModding/BNM-Android/tree/RU-master)
ByNameModding is a library for modding il2cpp games by classes, methods, field names on Android. This edition is focused on working on Android with il2cpp. It includes everything you need for modding unity games.<br>
Requires C++20 minimum.

### What you can do with BNM?
+ [Finding everything by names](Examples/01_Basics.cpp) (classes, methods, fields ane etc.)
+ Finding [Genetic](Examples/04_Generic.cpp) classes and methods.
+ Adding [your own classes](Examples/03_NewOrModClasses.cpp) to game that work even with AssetBundles!
+ [Change game classes](Examples/03_NewOrModClasses.cpp) by addeding fields and methods.
+ Use [basic C# structs](Examples/02_OtherStructures.cpp) like string (monoString), array (monoArray), dictionary (monoDictionary) that fully work.

### New classes
BNM can add your own class to game, it requires C++20 and above.<br>
If you nest your class from MonoBehaviour (or other class with events) methods like `Update` will work.<br>
And it fully computable with AssetBundles!<br>
For bundles need add your class to dlls that listed in ScriptingAssemblies.json.<br>
Basically BNM add classes to `Assembly-CSharp.dll` that always in this list.<br>
If you want to use your own dll with bundles, add it to this file, but this will work only if you use BNM internally.<br>

### Can I use BNM externally?
Yes, you can, but this is not tested. To do that see `BNM::External::LoadBNM`

## Getting Started
+ First clone the repo and add it to your project.
+ Set C++ version to 20 or higher
+ Add [BNM.cpp](ByNameModding/BNM.cpp) to Android.mk or CMakeLists.txt.<br>
    Android.mk
    ```mk
    LOCAL_SRC_FILES += ByNameModding/BNM.cpp
    ```
    CMakeLists.txt
    ```cmake
    add_library("Your lib name" SHARED ByNameModding/BNM.cpp "...other cpp files")
    ```
+ Change `UNITY_VER`, include your string obfuscator and hooking software, uncomment `BNM_DOTNET35` if the game uses .NET 3.5 in [BNM_settings.hpp](ByNameModding/BNM_settings.hpp)
+ Done! See [examples](Examples) or add it to your project to understand how to work with BNM.

## Supported Unity versions: 2017.x - 2023.1.x

## Dependencies
[UTF8-CPP](https://github.com/nemtrif/utfcpp) used by il2cpp and by BNM too.<br>
[Open-hierarchy custom RTTI](https://github.com/royvandam/rtti/tree/cf0dee6fb3999573f45b0726a8d5739022e3dacf) used to optimize memory usage
### Android hookinng software for example:
[Dobby](https://github.com/jmpews/Dobby) - recomended<br>
[Substrate](https://github.com/jbro129/Unity-Substrate-Hook-Android/tree/master/C%2B%2B/Substrate) with [And64InlineHook](https://github.com/Rprop/And64InlineHook) - may don't work
