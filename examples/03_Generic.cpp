#include <BNM/Class.hpp>
#include <BNM/Method.hpp>

void OnLoaded_Example_03() {
    using namespace BNM;

    // ---------------------------------------------------- Generic methods ----------------------------------------------------

    auto object = GetType<void *>().ToClass();

    auto gameObject = Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("GameObject"), Image(OBFUSCATE_BNM("UnityEngine.CoreModule")));

    //! There is GameObject class with the generic GetComponent method:
    /*
      public class GameObject : Object {
        // ...
		public T GetComponent<T>() {}
        // ...
	  }
    */

    // So we get `public T GetComponent<T>() {}`
    auto GetComponent = gameObject.GetMethod(OBFUSCATE_BNM("GetComponent"), 0);

    // And so - `public object GetComponent<object>(){}`
    Method<void *> GetComponentObject = GetComponent.GetGeneric({object});

    // Let's check results by getting address of method
    BNM_LOG_INFO("GameObject::GetComponent<T>: 0x%lX", (BNM_PTR)BNM::Utils::OffsetInLib((void *)GetComponent.GetOffset()));
    BNM_LOG_INFO("GameObject::GetComponent<object>: 0x%lX", (BNM_PTR)BNM::Utils::OffsetInLib((void *)GetComponentObject.GetOffset()));


    // ---------------------------------------------------- Generic классы (classes) ----------------------------------------------------

    //! Let's analyze getting of generic classes in the code from example 02:
    /*
    namespace System.Collections.Generic
    {
      public class Dictionary<TKey, TValue> : ... {}
    }
    */


    // To find a type with <T1, T2, T3, ..., Tx>, you need to search for 'TypeName`(number of parameters in <>)"

    // So we get `Dictionary<TKey, TValue>`. Since the Dictionary class has two genetic types, then after '`' we write '2'
    auto dictionaryClass = BNM::Class(OBFUSCATE_BNM("System.Collections.Generic"), OBFUSCATE_BNM("Dictionary`2"), BNM::Image(OBFUSCATE_BNM("mscorlib.dll")));

    // And so - `Dictionary<int, int>`
    auto dictionary_int_int_Class = dictionaryClass.GetGeneric({BNM::GetType<int>(), BNM::GetType<int>()});

    // Let's check the results using the address of the Add method as an example
    BNM_LOG_INFO("Dictionary<TKey, TValue>::Add: 0x%lX", (BNM_PTR)BNM::Utils::OffsetInLib((void *)dictionaryClass.GetMethod("Add", 2).GetOffset()));
    BNM_LOG_INFO("Dictionary<int, int>::Add: 0x%lX", (BNM_PTR)BNM::Utils::OffsetInLib((void *)dictionary_int_int_Class.GetMethod("Add", 2).GetOffset()));
}
