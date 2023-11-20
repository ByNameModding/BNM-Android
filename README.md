## ByNameModding
#### [English](https://github.com/ByNameModding/BNM-Android/tree/master) | Русский
ByNameModding - это библиотека для моддинга Unity-игр по именам классов, методов, полей. Данная редакция ориентирована на работу на Android с il2cpp. Библиотека включает в себя все, что вам нужно для моддинга игр на Unity.<br>
Требуется минимум C++20.

### Что можно делать с BNM?
1. [Искать всё по именам](Examples/01_Basics.cpp) (классы, методы, поля и т. д.).
2. Искать [Generic](Examples/04_Generic.cpp) классы и методы.
3. Добавлять в игру [ваши собственные классы](Examples/03_NewOrModClasses.cpp), которые работают даже с AssetBundles!
4. [Изменять классы игры](Examples/03_NewOrModClasses.cpp), добавляя в них поля и методы.
5. Использовать [обычные структуры С#](Examples/02_OtherStructures.cpp), например, строки (monoString) или словари (monoDictionary), которые полностью работают.

### Новые классы
BNM может добавить ваши собственные классы в игру.<br>
Если вы наследуете свой класс от MonoBehaviour (или другой любой класс с событиями), будут работать такие методы, как `Update`.<br>
И это полностью совместимо с AssetBundles!<br>
Для пакетов данных нужно добавить свой класс в библиотеки dll, перечисленные в ScriptingAssemblies.json.<br>
По умолчанию BNM добавляет классы к `Assembly-CSharp.dll`, который всегда есть в этом списке.<br>
Если вы хотите использовать свой dll с пакетами данных, добавьте dll в этот файл. Но это будет работать только в том случае, если вы используете BNM внутренне.<br>

### Можно ли использовать BNM извне?
Да, но это не проверялось. Чтобы сделать это, смотрите `BNM::External::TryLoad`.

## Начало работы:
+ сначала клонируйте репозиторий и добавьте его в свой проект;
+ установите версию C++ на 20 или выше;
+ добавьте [BNM.cpp](ByNameModding/BNM.cpp) в Android.mk или CMakeLists.txt:<br>
    Android.mk
    ```mk
    LOCAL_SRC_FILES += ByNameModding/BNM.cpp
    ```
    CMakeLists.txt
    ```cmake
    add_library("Your lib name" SHARED ByNameModding/BNM.cpp "...other cpp files")
    ```
+ измените `UNITY_VER`, добавьте свой шифровщик строк и ПО для подмены методов, раскомментируйте `BNM_DOTNET35`, если игра использует .NET 3.5 в [BNM_settings.hpp](ByNameModding/BNM_settings.hpp);
+ смотрите [примеры](Examples) или добавьте их в свой проект, чтобы понять, как работать с BNM.

## Поддерживаемые версии Unity: 2017.x - 2023.1.x

## Зависимости
[UTF8-CPP](https://github.com/nemtrif/utfcpp) используется в il2cpp и в BNM тоже.<br>
[Open-hierarchy custom RTTI](https://github.com/royvandam/rtti/tree/cf0dee6fb3999573f45b0726a8d5739022e3dacf) используется для оптимизации использования памяти
### ПО для подмены методов на Android:
[Dobby](https://github.com/jmpews/Dobby) - рекомендуется<br>
[Substrate](https://github.com/jbro129/Unity-Substrate-Hook-Android/tree/master/C%2B%2B/Substrate) с [And64InlineHook](https://github.com/Rprop/And64InlineHook) - может не сработать.
