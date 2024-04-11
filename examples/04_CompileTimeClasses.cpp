#include <BNM/UserSettings/GlobalSettings.hpp>
#include <BNM/Class.hpp>

void OnLoaded_Example_04() {
    using namespace BNM;

    // CompileTimeClass используется для ClassesManagement для поиска классов
    // Напрямую его создать нельзя, поэтому существует CompileTimeClassBuilder

    // Предположим, есть такая структура:
    /*
     namespace ExampleNamespace {
        class ExampleClass {
            class ExampleInnerClass {
                class ExampleInnerGenericClass<T1, T2, T3> {

                }
            }
        }
     }
     */
    // И вам нужен класс-массив ExampleInnerGenericClass<int, float, ExampleClass>[]

    auto targetClass = CompileTimeClassBuilder().Class(OBFUSCATE_BNM("ExampleClass"), OBFUSCATE_BNM("ExampleNamespace"))
            .Class(OBFUSCATE_BNM("ExampleInnerClass"))
            .Class(OBFUSCATE_BNM("ExampleInnerGenericClass`3"))
            .Generic({
                BNM::GetType<int>(),
                BNM::GetType<float>(),
                CompileTimeClassBuilder().Class(OBFUSCATE_BNM("ExampleClass"), OBFUSCATE_BNM("ExampleNamespace")).Build()
            })
            .Modifier(CompileTimeClass::ModifierType::Array).Build();
}