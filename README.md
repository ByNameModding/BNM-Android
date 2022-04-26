## ByNameModding
ByNameModding is a library for modding il2cpp games by classes, methods, field names on Android. It includes everything you need for modding unity games.

## Getting Started
+ First clone the repo and add it to your project.
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

# Unity support
| Il2Cpp Version | Unity Version                |
| -------------- | ---------------------------- |
| 24.0           | 2017.x - 2018.2.x            |
| 24.1           | 2018.3.x - 2018.4.x          |
| 24.2           | 2019.1.x - 2019.2.x          |
| 24.3           | 2019.3.x, 2019.4.x, 2020.1.x |
| 24.4           | 2019.4.x and 2020.1.x        |
| 27.0           | 2021.2.x                     |
| 27.1           | 2020.2.x - 2020.3.x          |
| 27.2           | 2021.1.x, 2021.2.x           |
| 28             | 2021.3.x, 2022.1.x           |

## Dependencies
[UTF8-CPP](https://github.com/nemtrif/utfcpp) used by il2cpp and by BNM too.
### Android hookinng software for example:
[Dobby](https://github.com/jmpews/Dobby) - recomended<br>
[Substrate](https://github.com/jbro129/Unity-Substrate-Hook-Android/tree/master/C%2B%2B/Substrate) with [And64InlineHook](https://github.com/Rprop/And64InlineHook) - may don't work
