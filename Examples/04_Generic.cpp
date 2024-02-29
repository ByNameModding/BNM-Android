#include "../ByNameModding/BNM.hpp"

void OnLoaded_Example_04() {
    using namespace BNM;

    auto object = GetType<void *>().ToLC();

    auto gameObject = LoadClass(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("GameObject"), OBFUSCATE_BNM("UnityEngine.CoreModule"));

    //! Есть:
    //! public T GetComponent<T>() {}
    auto GetComponent = gameObject.GetMethodByName(OBFUSCATE_BNM("GetComponent"), 0);

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
    auto ExposedReference = LoadClass(OBFUSCATE_BNM("UnityEngine"), OBFUSCATE_BNM("ExposedReference`1"), gameObject.GetIl2CppClass()->image);

    // Получить ExposedReference<GameObject>
    auto ExposedReference_GameObject = ExposedReference.GetGeneric({gameObject});

    BNM_LOG_INFO("ExposedReference<T>::Resolve: 0x%lX", (BNM_PTR)BNM::Utils::OffsetInLib((void *)ExposedReference.GetMethodByName("Resolve").GetOffset()));
    BNM_LOG_INFO("ExposedReference<GameObject>::Resolve: 0x%lX", (BNM_PTR)BNM::Utils::OffsetInLib((void *)ExposedReference_GameObject.GetMethodByName("Resolve").GetOffset()));
}
