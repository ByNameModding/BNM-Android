#pragma once

#include <new>

#include "UnityStructures/Color.h"
#include "UnityStructures/Quaternion.h"
#include "UnityStructures/Ray.h"
#include "UnityStructures/RaycastHit.h"
#include "UnityStructures/Rect.h"
#include "UnityStructures/Vector2.h"
#include "UnityStructures/Vector3.h"
#include "UnityStructures/Vector4.h"
#include "UnityStructures/Matrix3x3.h"
#include "UnityStructures/Matrix4x4.h"

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "Utils.hpp"



namespace BNM::UnityEngine {
    // Должен быть использован для новых классов, если родитель: UnityEngine.Object, ScriptableObject
    // Для System.Object нужно использовать BNM::IL2CPP::Il2CppObject
    struct Object : public BNM::IL2CPP::Il2CppObject {
        BNM_INT_PTR m_CachedPtr = 0;
        inline bool Alive() { return std::launder(this) && (BNM_PTR)m_CachedPtr; }
        inline bool Same(void *object) { return Same((Object *)object); }
        inline bool Same(Object *object) { return (!Alive() && !object->Alive()) || (Alive() && object->Alive() && m_CachedPtr == object->m_CachedPtr); }
    };

    // Должен быть использован для новых классов, если родитель: MonoBehaviour
    struct MonoBehaviour : public Object {
#if UNITY_VER >= 222
        void *m_CancellationTokenSource{};
#endif
    };

    // Только если объект - дочерний элемент класса UnityEngine.Object или объект - это UnityEngine.Object
    template <typename T>
    inline bool IsUnityObjectAlive(T o) {
        return ((UnityEngine::Object *)o)->Alive();
    };
    // Только если объект - дочерний элемент класса UnityEngine.Object или объект - это UnityEngine.Object
    template <typename T1, typename T2>
    inline bool IsSameUnityObject(T1 o1, T2 o2) {
        auto obj1 = (UnityEngine::Object *)o1;
        auto obj2 = (UnityEngine::Object *)o2;
        return obj1->Same(obj2);
    };

}