#pragma once

#include <new>

#include "UnityStructures/Color.hpp"
#include "UnityStructures/Quaternion.hpp"
#include "UnityStructures/Ray.hpp"
#include "UnityStructures/RaycastHit.hpp"
#include "UnityStructures/Rect.hpp"
#include "UnityStructures/Vector2.hpp"
#include "UnityStructures/Vector3.hpp"
#include "UnityStructures/Vector4.hpp"
#include "UnityStructures/Matrix3x3.hpp"
#include "UnityStructures/Matrix4x4.hpp"

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"
#include "Utils.hpp"
#include "Delegates.hpp"

namespace BNM::UnityEngine {
    // Should be used for new classes if the parent is UnityEngine.Object, ScriptableObject
    // For System.Object, use BNM::IL2CPP::Il2CppObject
    struct Object : public BNM::IL2CPP::Il2CppObject {
        constexpr Object() : BNM::IL2CPP::Il2CppObject({}) {}
        BNM_INT_PTR m_CachedPtr = 0;
        inline bool Alive() { return std::launder(this) && (BNM_PTR)m_CachedPtr; }
        inline bool Same(void *object) { return Same((Object *)object); }
        inline bool Same(Object *object) { return (!Alive() && !object->Alive()) || (Alive() && object->Alive() && m_CachedPtr == object->m_CachedPtr); }
    };

    // Should be used for new classes if the parent is MonoBehaviour
    struct MonoBehaviour : public Object {
#if UNITY_VER >= 222
        void *m_CancellationTokenSource{};
#endif
    };

    // Only if the object is a child of the UnityEngine.Object class or object is a UnityEngine.Object
    template <typename T>
    inline bool IsUnityObjectAlive(T o) {
        return ((UnityEngine::Object *)o)->Alive();
    }

    // Only if the object is a child of the UnityEngine.Object class or object is a UnityEngine.Object
    template <typename T1, typename T2>
    inline bool IsSameUnityObject(T1 o1, T2 o2) {
        auto obj1 = (UnityEngine::Object *)o1;
        auto obj2 = (UnityEngine::Object *)o2;
        return obj1->Same(obj2);
    }

    template <typename ...Params>
    struct UnityAction : public MulticastDelegate<void> {
        inline void Invoke(Params ...params) { (MulticastDelegate<void>(*this)).Invoke(params...); }
    };

    struct ArgumentCache : public BNM::IL2CPP::Il2CppObject {
        constexpr ArgumentCache() : BNM::IL2CPP::Il2CppObject({}) {}
        UnityEngine::Object *m_ObjectArgument{};
        Structures::Mono::String *m_ObjectArgumentAssemblyTypeName{};
        int m_IntArgument{};
        float m_FloatArgument{};
        Structures::Mono::String *m_StringArgument{};
        bool m_BoolArgument{};
    };

    struct PersistentCall : public BNM::IL2CPP::Il2CppObject {
        constexpr PersistentCall() : BNM::IL2CPP::Il2CppObject({}) {}
        UnityEngine::Object *m_Target{};
        Structures::Mono::String *m_TargetAssemblyTypeName{};
        Structures::Mono::String *m_MethodName{};
        int m_Mode{};
        ArgumentCache *m_Arguments{};
        int m_CallState{};
        [[nodiscard]] inline bool IsValid() const { return m_TargetAssemblyTypeName && m_TargetAssemblyTypeName->length && m_MethodName && m_MethodName->length; }
    };

    struct PersistentCallGroup : public BNM::IL2CPP::Il2CppObject {
        constexpr PersistentCallGroup() : BNM::IL2CPP::Il2CppObject({}) {}
        Structures::Mono::List<PersistentCall *> *m_Calls{};
    };

    struct InvokableCallBase : public BNM::IL2CPP::Il2CppObject {
        constexpr InvokableCallBase() : BNM::IL2CPP::Il2CppObject({}) {}
        UnityAction<> *action{};
    };

    template <typename ...Params>
    struct InvokableCall : public BNM::IL2CPP::Il2CppObject {
        constexpr InvokableCall() : BNM::IL2CPP::Il2CppObject({}) {}
        UnityAction<Params...> *action{};
    };

    struct InvokableCallList : public BNM::IL2CPP::Il2CppObject {
        constexpr InvokableCallList() : BNM::IL2CPP::Il2CppObject({}) {}
        Structures::Mono::List<InvokableCallBase *> *m_PersistentCalls{};
        Structures::Mono::List<InvokableCallBase *> *m_RuntimeCalls{};
        Structures::Mono::List<InvokableCallBase *> *m_ExecutingCalls{};
        bool m_NeedsUpdate{};

    };

    struct UnityEventBase : public IL2CPP::Il2CppObject {
        constexpr UnityEventBase() : BNM::IL2CPP::Il2CppObject({}) {}
        InvokableCallList *m_Calls{};
        PersistentCallGroup *m_PersistentCalls{};
        bool m_CallsDirty = true;
        BNM::Class GetArgumentType(PersistentCall *call);
        BNM::Class GetTargetType(PersistentCall *call);
    };
    enum class PersistentListenerMode {
        EventDefined = 0, Void = 1, Object = 2, Int = 3, Float = 4, String = 5, Bool = 6
    };
    template<typename ...Params>
    struct UnityEvent : public UnityEventBase {
        Structures::Mono::Array<IL2CPP::Il2CppObject *> *m_InvokeArray{};

        inline void AddListener(UnityAction<Params...> *action) { BNM::Class(klass).GetMethod(OBFUSCATE_BNM("AddListener")).template cast<void>().Call(action); }
        inline void RemoveListener(UnityAction<Params...> *action) { BNM::Class(klass).GetMethod(OBFUSCATE_BNM("RemoveListener")).template cast<void>().Call(action); }

        inline void Invoke(Params ...params) {
            InvokePersistent(params...);

            if (!m_PersistentCalls || !m_PersistentCalls->m_Calls || m_PersistentCalls->m_Calls->size) return;

            InvokeCalls(params...);
        }

        void InvokePersistent(Params ...params) {
            Structures::Mono::List<PersistentCall *> *calls;

            if (!m_PersistentCalls) return;
            if (!(calls = m_PersistentCalls->m_Calls)) return;

            for (int i = 0; i < calls->size; ++i) {
                auto persistentCall = calls->At(i);
                if (!persistentCall->IsValid() || persistentCall->m_CallState == 0) continue;
                auto argumentType = GetArgumentType(persistentCall);
                auto targetType = GetTargetType(persistentCall);

                MethodBase methodBase{};

                auto methodName = persistentCall->m_MethodName->str();

                switch ((PersistentListenerMode) persistentCall->m_Mode) {
                    case PersistentListenerMode::EventDefined:
                        methodBase = targetType.GetMethod(methodName, {BNM::GetType<Params>()...});
                        break;
                    case PersistentListenerMode::Void:
                        methodBase = targetType.GetMethod(methodName, 0);
                        break;
                    case PersistentListenerMode::Object:
                        methodBase = targetType.GetMethod(methodName, {argumentType.Alive() ? argumentType : BNM::GetType<Object *>()});
                        break;
                    case PersistentListenerMode::Int:
                        methodBase = targetType.GetMethod(methodName, {BNM::GetType<int>()});
                        break;
                    case PersistentListenerMode::Float:
                        methodBase = targetType.GetMethod(methodName, {BNM::GetType<float>()});
                        break;
                    case PersistentListenerMode::String:
                        methodBase = targetType.GetMethod(methodName, {BNM::GetType<Structures::Mono::String *>()});
                        break;
                    case PersistentListenerMode::Bool:
                        methodBase = targetType.GetMethod(methodName, {BNM::GetType<bool>()});
                        break;
                }
                if (!methodBase.Initialized() || !methodBase._isStatic && !persistentCall->m_Target) continue;

                methodBase.cast<void>()(params...);
            }
        }

        inline void InvokeCalls(Params ...params) {
            if (!m_Calls) return;

            InvokeList((Structures::Mono::List<InvokableCall<Params...> *> *) m_Calls->m_PersistentCalls, params...);
            InvokeList((Structures::Mono::List<InvokableCall<Params...> *> *) m_Calls->m_RuntimeCalls, params...);
        }
    private:
        void InvokeList(Structures::Mono::List<InvokableCall<Params...> *> *list, Params ...params) {
            if (!list) return;

            for (int i = 0; i < list->size; ++i) {
                auto call = list->At(i);
                if (!call || !call->action) continue;
                call->action->Invoke(params...);
            }
        }
    };
}