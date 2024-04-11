#include <BNM/Class.hpp>
#include <BNM/Method.hpp>

void OnLoaded_Example_03() {
    using namespace BNM;

    auto object = GetType<void *>().ToClass();

    auto gameObject = Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("GameObject"), Image(OBFUSCATE_BNM("UnityEngine.CoreModule")));

    //! Есть:
    //! public T GetComponent<T>() {}
    auto GetComponent = gameObject.GetMethod(OBFUSCATE_BNM("GetComponent"), 0);

    //! Получить:
    //! public object GetComponent<object>() {}
    Method<void *> GetComponentObject = GetComponent.GetGeneric({object});

    BNM_LOG_INFO("GameObject::GetComponent<T>: 0x%lX", (BNM_PTR)BNM::Utils::OffsetInLib((void *)GetComponent.GetOffset()));
    BNM_LOG_INFO("GameObject::GetComponent<object>: 0x%lX", (BNM_PTR)BNM::Utils::OffsetInLib((void *)GetComponentObject.GetOffset()));
    //! Есть:
    /**
      public struct ExposedReference<T> where T : Object {
		public T Resolve(IExposedPropertyTable resolver);
        // ...
	  }
    */
    // Чтобы найти тип с <T1, T2, T3, ..., Tx> нужно искать "ИмяТипа`(кол-во параметров в <>)"
    auto ExposedReference = Class(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("ExposedReference`1"), gameObject.GetClass()->image);

    // Получить ExposedReference<GameObject>
    auto ExposedReference_GameObject = ExposedReference.GetGeneric({gameObject});

    BNM_LOG_INFO("ExposedReference<T>::Resolve: 0x%lX", (BNM_PTR)BNM::Utils::OffsetInLib((void *)ExposedReference.GetMethod("Resolve").GetOffset()));
    BNM_LOG_INFO("ExposedReference<GameObject>::Resolve: 0x%lX", (BNM_PTR)BNM::Utils::OffsetInLib((void *)ExposedReference_GameObject.GetMethod("Resolve").GetOffset()));
}
