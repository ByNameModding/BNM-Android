## ByNameModding
ByNameModding is a library for modding il2cpp games by classes, methods, field names on Android. It includes everything you need for modding unity games. 

### Requires c++14 and above for basic functionality and c++17 and above to create new il2cpp classes

## Getting Started
+ First clone the repo and add it to your project.
+ Set c++ version to 14 or higher
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
+ Done! See [Examples.cpp](Examples.cpp) or add it to your project to understand how to work with BNM.

## Supported Unity versions: 2017.x - 2022.1.x

## Dependencies
[UTF8-CPP](https://github.com/nemtrif/utfcpp) used by il2cpp and by BNM too.
### Android hookinng software for example:
[Dobby](https://github.com/jmpews/Dobby) - recomended<br>
[Substrate](https://github.com/jbro129/Unity-Substrate-Hook-Android/tree/master/C%2B%2B/Substrate) with [And64InlineHook](https://github.com/Rprop/And64InlineHook) - may don't work
