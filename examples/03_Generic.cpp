#include <BNM/Class.hpp>
#include <BNM/Method.hpp>

void OnLoaded_Example_03() {
    using namespace BNM;

    // ---------------------------------------------------- Generic methods ----------------------------------------------------

    auto object = BNM::Defaults::Get<void *>().ToClass();

    auto gameObject = Class(BNM_OBFUSCATE("UnityEngine"), BNM_OBFUSCATE("GameObject"), Image(BNM_OBFUSCATE("UnityEngine.CoreModule")));

    //! There is GameObject class with the generic GetComponent method:
    /*
      public class GameObject : Object {
        // ...
		public T GetComponent<T>() {}
        // ...
	  }
    */

    // So we get `public T GetComponent<T>() {}`
    auto GetComponent = gameObject.GetMethod(BNM_OBFUSCATE("GetComponent"), 0);

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
    auto dictionaryClass = BNM::Class(BNM_OBFUSCATE("System.Collections.Generic"), BNM_OBFUSCATE("Dictionary`2"), BNM::Image(BNM_OBFUSCATE("mscorlib.dll")));

    // And so - `Dictionary<int, int>`
    auto dictionary_int_int_Class = dictionaryClass.GetGeneric({BNM::Defaults::Get<int>(), BNM::Defaults::Get<int>()});

    // Let's check the results using the address of the Add method as an example
    BNM_LOG_INFO("Dictionary<TKey, TValue>::Add: 0x%lX", (BNM_PTR)BNM::Utils::OffsetInLib((void *)dictionaryClass.GetMethod("Add", 2).GetOffset()));
    BNM_LOG_INFO("Dictionary<int, int>::Add: 0x%lX", (BNM_PTR)BNM::Utils::OffsetInLib((void *)dictionary_int_int_Class.GetMethod("Add", 2).GetOffset()));
}
