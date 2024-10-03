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

    auto targetClass = CompileTimeClassBuilder(BNM_OBFUSCATE("ExampleNamespace"), BNM_OBFUSCATE("ExampleClass")) // ExampleNamespace.ExampleClass
            .Class(BNM_OBFUSCATE("ExampleInnerClass")) // ExampleNamespace.ExampleClass.ExampleInnerClass
            .Class(BNM_OBFUSCATE("ExampleInnerGenericClass`3")) // ExampleNamespace.ExampleClass.ExampleInnerClass.ExampleInnerGenericClass<T1, T2, T3>
            .Generic({
                BNM::Defaults::Get<int>(),
                BNM::Defaults::Get<float>(),
                CompileTimeClassBuilder(BNM_OBFUSCATE("ExampleNamespace"), BNM_OBFUSCATE("ExampleClass")).Build()
            }) // ExampleNamespace.ExampleClass.ExampleInnerClass.ExampleInnerGenericClass<int, float, ExampleNamespace.ExampleClass>
            .Modifier(CompileTimeClass::ModifierType::Array) // ExampleNamespace.ExampleClass.ExampleInnerClass.ExampleInnerGenericClass<int, float, ExampleNamespace.ExampleClass>[]
            .Build();

}