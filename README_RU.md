## ByNameModding
#### [English](https://github.com/ByNameModding/BNM-Android/blob/master/README.md) | Русский
ByNameModding - это библиотека для моддинга Unity-игр по именам классов, методов, полей. Данная редакция ориентирована на работу на Android с il2cpp. Библиотека включает в себя все, что вам нужно для моддинга игр на Unity.<br>
Требуется минимум C++20.

## Начало работы:
Смотри документацию на [ByNameModding.github.io](https://bynamemodding.github.io/).

## Поддерживаемые версии Unity: 5.6.4f1, 2017.x - 6000.0.x

## Зависимости
[UTF8-CPP](https://github.com/nemtrif/utfcpp) используется в il2cpp и в BNM тоже.<br>
[Open-hierarchy custom RTTI](https://github.com/royvandam/rtti/tree/cf0dee6fb3999573f45b0726a8d5739022e3dacf) используется для оптимизации использования памяти
### ПО для подмены методов на Android:
[Dobby](https://github.com/jmpews/Dobby)<br>
[ShadowHook](https://github.com/bytedance/android-inline-hook)<br>
[Substrate](https://github.com/jbro129/Unity-Substrate-Hook-Android/tree/master/C%2B%2B/Substrate) с [And64InlineHook](https://github.com/Rprop/And64InlineHook) - не поддерживают откат подмены