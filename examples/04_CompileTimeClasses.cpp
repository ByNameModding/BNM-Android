#include <BNM/UserSettings/GlobalSettings.hpp>
#include <BNM/Class.hpp>

void OnLoaded_Example_04() {
    using namespace BNM;

    // CompileTimeClass is used for ClassesManagement to find classes
    // It cannot be created directly, so there is a CompileTimeClassBuilder

    // Suppose there is such a structure:
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
    // And you need an array class ExampleInnerGenericClass<int, float, ExampleClass>[]

    auto targetClass = CompileTimeClassBuilder(OBFUSCATE_BNM("ExampleNamespace"), OBFUSCATE_BNM("ExampleClass")) // ExampleNamespace.ExampleClass
            .Class(OBFUSCATE_BNM("ExampleInnerClass")) // ExampleNamespace.ExampleClass.ExampleInnerClass
            .Class(OBFUSCATE_BNM("ExampleInnerGenericClass`3")) // ExampleNamespace.ExampleClass.ExampleInnerClass.ExampleInnerGenericClass<T1, T2, T3>
            .Generic({
                BNM::GetType<int>(),
                BNM::GetType<float>(),
                CompileTimeClassBuilder(OBFUSCATE_BNM("ExampleNamespace"), OBFUSCATE_BNM("ExampleClass")).Build()
            }) // ExampleNamespace.ExampleClass.ExampleInnerClass.ExampleInnerGenericClass<int, float, ExampleNamespace.ExampleClass>
            .Modifier(CompileTimeClass::ModifierType::Array) // ExampleNamespace.ExampleClass.ExampleInnerClass.ExampleInnerGenericClass<int, float, ExampleNamespace.ExampleClass>[]
            .Build();

}