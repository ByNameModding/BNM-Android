## ByNameModding
#### [English](https://github.com/ByNameModding/BNM-Android/blob/master/README.md) | Русский
ByNameModding - это библиотека для моддинга Unity-игр по именам классов, методов, полей. Данная редакция ориентирована на работу на Android с il2cpp. Библиотека включает в себя все, что вам нужно для моддинга игр на Unity.<br>
Требуется минимум C++20.

### Что можно делать с BNM?
1. [Искать всё по именам](examples/01_Basics.cpp) (классы, методы, поля и т. д.).
2. Искать [Generic](examples/03_Generic.cpp) классы и методы.
3. Добавлять в игру [ваши собственные классы](examples/05_ClassesManagement.cpp), которые работают даже с AssetBundles!
4. [Изменять классы игры](examples/05_ClassesManagement.cpp), добавляя в них поля и методы.
5. Использовать [обычные структуры С#](examples/02_OtherStructures.cpp), например, строки (Mono::String) или словари (Mono::Dictionary), которые полностью работают.

### Новые классы
BNM может добавить ваши собственные классы в игру.<br>
Если вы наследуете свой класс от MonoBehaviour (или другой любой класс с событиями), будут работать такие методы, как `Update`.<br>
И это полностью совместимо с AssetBundles!<br>
Для пакетов данных нужно добавить свой класс в библиотеки dll, перечисленные в ScriptingAssemblies.json.<br>
По умолчанию BNM добавляет классы к `Assembly-CSharp.dll`, который всегда есть в этом списке.<br>
Если вы хотите использовать свой dll с пакетами данных, добавьте dll в этот файл. Но это будет работать только в том случае, если вы используете BNM внутренне.<br>

## Начало работы:
+ сначала клонируйте репозиторий и добавьте его в свой проект;
+ установите версию C++ на 20 или выше;
+ добавьте код ниже в зависимости от того, что вы используете:<br>
    Android.mk
    ```mk
    BNM_PATH := $(LOCAL_PATH)/путь/до/ByNameModding
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
    add_subdirectory(путь/до/ByNameModding EXCLUDE_FROM_ALL)
    get_property(BNM_INCLUDE_DIRECTORIES TARGET BNM PROPERTY BNM_INCLUDE_DIRECTORIES)
	
    # ...
	
    target_include_directories(
        # Имя вашей библиотеки
        PUBLIC
        ${BNM_INCLUDE_DIRECTORIES}
        # ...
    )
    target_link_libraries(
        # Имя вашей библиотеки
        PUBLIC
        BNM
        # ...
    )
    ```
+ измените `UNITY_VER`, добавьте свой шифровщик строк и ПО для подмены методов, раскомментируйте `BNM_DOTNET35`, если игра использует .NET 3.5 в [GlobalSettings.hpp](include/BNM/UserSettings/GlobalSettings.hpp);
+ смотрите [примеры](examples) или добавьте их в свой проект, чтобы понять, как работать с BNM.

## Поддерживаемые версии Unity: 5.6.4f1, 2017.x - 6000.0.x

## Зависимости
[UTF8-CPP](https://github.com/nemtrif/utfcpp) используется в il2cpp и в BNM тоже.<br>
[Open-hierarchy custom RTTI](https://github.com/royvandam/rtti/tree/cf0dee6fb3999573f45b0726a8d5739022e3dacf) используется для оптимизации использования памяти
### ПО для подмены методов на Android:
[Dobby](https://github.com/jmpews/Dobby)<br>
[ShadowHook](https://github.com/bytedance/android-inline-hook)<br>
[Substrate](https://github.com/jbro129/Unity-Substrate-Hook-Android/tree/master/C%2B%2B/Substrate) с [And64InlineHook](https://github.com/Rprop/And64InlineHook) - не поддерживают отмену подмены