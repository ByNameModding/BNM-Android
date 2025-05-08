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

/**
    @brief Namespace that has types and helpers to work with Unity objects.
*/
namespace BNM::UnityEngine {
    // For System.Object, use BNM::IL2CPP::Il2CppObject
    /**
        @brief UnityEngine.Object implementation

        Should be used for all objects that inherit UnityEngine.Object.
    */
    struct Object : public BNM::IL2CPP::Il2CppObject {

        constexpr Object() : BNM::IL2CPP::Il2CppObject({}) {}
        BNM_INT_PTR m_CachedPtr = 0;

        /**
            @brief Check if object is valid.
            @attention To check Unity object for null <b>ALWAYS</b> use this method. It checks object pointer and pointer to Unity object. These checks reduces crashes significantly.
            @return True if object is not null and valid for usage in Unity.
        */
        [[nodiscard]] inline bool IsValid() const __attribute__((always_inline)) {
            return CheckForNull(this) && m_CachedPtr;
        }

        /**
            @brief Alias for IsValid()
        */
        [[nodiscard]] inline bool Alive() const __attribute__((always_inline)) { return IsValid(); }

        /**
            @brief Check if current object is the same with other one.
            @param object Comparison object
            @return True if objects are the same.
        */
        inline bool Same(void *object) const { return Same((Object *)object); }

        /**
            @brief Check if current object is the same with other one.
            @param object Comparison object
            @return True if objects are the same.
        */
        inline bool Same(Object *object) const { return (!Alive() && !object->Alive()) || (Alive() && object->Alive() && m_CachedPtr == object->m_CachedPtr); }
    };

    /**
        @brief UnityEngine.MonoBehaviour implementation

        Should be used as parent in new classes if you are trying to create your own MonoBehaviour class.
    */
    struct MonoBehaviour : public Object {
#if UNITY_VER >= 222
        void *m_CancellationTokenSource{};
#endif
    };

    /**
        @brief Alias for BNM::UnityEngine::Object::IsValid()
    */
    template <typename T>
    inline bool IsUnityObjectAlive(T o) {
        return ((UnityEngine::Object *)o)->Alive();
    }

    /**
        @brief Alias for BNM::UnityEngine::Object::Same()
    */
    template <typename T1, typename T2>
    inline bool IsSameUnityObject(T1 o1, T2 o2) {
        auto obj1 = (UnityEngine::Object *)o1;
        auto obj2 = (UnityEngine::Object *)o2;
        return obj1->Same(obj2);
    }

    /**
        @brief UnityEngine.Events.UnityAction implementation
    */
    template <typename ...Parameters>
    struct UnityAction : public MulticastDelegate<void> {
        inline void Invoke(Parameters ...parameters) { (MulticastDelegate<void>(*this)).Invoke(parameters...); }
    };

    /**
        @brief UnityEngine.Events.ArgumentCache implementation
    */
    struct ArgumentCache : public BNM::IL2CPP::Il2CppObject {
        constexpr ArgumentCache() : BNM::IL2CPP::Il2CppObject({}) {}
        UnityEngine::Object *m_ObjectArgument{};
        Structures::Mono::String *m_ObjectArgumentAssemblyTypeName{};
        int m_IntArgument{};
        float m_FloatArgument{};
        Structures::Mono::String *m_StringArgument{};
        bool m_BoolArgument{};
    };

    /**
        @brief UnityEngine.Events.PersistentListenerMode implementation
    */
    enum class PersistentListenerMode : int {
        EventDefined = 0, Void = 1, Object = 2, Int = 3, Float = 4, String = 5, Bool = 6
    };

    /**
        @brief UnityEngine.Events.UnityEventCallState implementation
    */
    enum class UnityEventCallState : int {
        Off = 0, EditorAndRuntime = 1, RuntimeOnly = 2
    };

    /**
        @brief UnityEngine.Events.PersistentCall implementation
    */
    struct PersistentCall : public BNM::IL2CPP::Il2CppObject {
        constexpr PersistentCall() : BNM::IL2CPP::Il2CppObject({}) {}
        UnityEngine::Object *m_Target{};
        Structures::Mono::String *m_TargetAssemblyTypeName{};
        Structures::Mono::String *m_MethodName{};
        PersistentListenerMode m_Mode{};
        ArgumentCache *m_Arguments{};
        UnityEventCallState m_CallState{};
        [[nodiscard]] inline bool IsValid() const { return m_TargetAssemblyTypeName && m_TargetAssemblyTypeName->length && m_MethodName && m_MethodName->length; }
    };

    /**
        @brief UnityEngine.Events.PersistentCallGroup implementation
    */
    struct PersistentCallGroup : public BNM::IL2CPP::Il2CppObject {
        constexpr PersistentCallGroup() : BNM::IL2CPP::Il2CppObject({}) {}
        Structures::Mono::List<PersistentCall *> *m_Calls{};
    };

    /**
        @brief UnityEngine.Events.InvokableCallBase implementation
    */
    struct InvokableCallBase : public BNM::IL2CPP::Il2CppObject {
        constexpr InvokableCallBase() : BNM::IL2CPP::Il2CppObject({}) {}
        UnityAction<> *action{};
    };

    /**
        @brief UnityEngine.Events.InvokableCall implementation
    */
    template <typename ...Parameters>
    struct InvokableCall : public BNM::IL2CPP::Il2CppObject {
        constexpr InvokableCall() : BNM::IL2CPP::Il2CppObject({}) {}
        UnityAction<Parameters...> *action{};
    };

    /**
        @brief UnityEngine.Events.InvokableCallList implementation
    */
    struct InvokableCallList : public BNM::IL2CPP::Il2CppObject {
        constexpr InvokableCallList() : BNM::IL2CPP::Il2CppObject({}) {}
        Structures::Mono::List<InvokableCallBase *> *m_PersistentCalls{};
        Structures::Mono::List<InvokableCallBase *> *m_RuntimeCalls{};
        Structures::Mono::List<InvokableCallBase *> *m_ExecutingCalls{};
        bool m_NeedsUpdate{};

    };

    /**
        @brief UnityEngine.Events.UnityEventBase implementation
    */
    struct UnityEventBase : public IL2CPP::Il2CppObject {
        constexpr UnityEventBase() : BNM::IL2CPP::Il2CppObject({}) {}
        InvokableCallList *m_Calls{};
        PersistentCallGroup *m_PersistentCalls{};
        bool m_CallsDirty = true;
        static BNM::Class GetArgumentType(PersistentCall *call);
        static BNM::Class GetTargetType(PersistentCall *call);
    };

    /**
        @brief UnityEngine.Events.UnityEvent implementation
    */
    template<typename ...Parameters>
    struct UnityEvent : public UnityEventBase {
        Structures::Mono::Array<IL2CPP::Il2CppObject *> *m_InvokeArray{};

        /**
            @brief Add listener to event.
        */
        inline void AddListener(UnityAction<Parameters...> *action) { BNM::Class(klass).GetMethod(BNM_OBFUSCATE("AddListener")).template cast<void>().Call(action); }

        /**
            @brief Remove listener from event.
        */
        inline void RemoveListener(UnityAction<Parameters...> *action) { BNM::Class(klass).GetMethod(BNM_OBFUSCATE("RemoveListener")).template cast<void>().Call(action); }

        /**
            @brief Invoke event.
            @param parameters Event parameters
        */
        inline void Invoke(Parameters ...parameters) const {
            InvokePersistent(parameters...);

            if (!m_PersistentCalls || !m_PersistentCalls->m_Calls || m_PersistentCalls->m_Calls->size) return;

            InvokeCalls(parameters...);
        }

        /**
            @brief Invoke event using persistent calls group.
            @param parameters Event parameters
        */
        void InvokePersistent(Parameters ...parameters) {
            Structures::Mono::List<PersistentCall *> *calls;

            if (!m_PersistentCalls) return;
            if (!(calls = m_PersistentCalls->m_Calls)) return;

            for (int i = 0; i < calls->size; ++i) {
                auto persistentCall = calls->At(i);
                if (!persistentCall->IsValid() || persistentCall->m_CallState == UnityEventCallState::Off) continue;
                auto argumentType = GetArgumentType(persistentCall);
                auto targetType = GetTargetType(persistentCall);

                MethodBase methodBase{};

                auto methodName = persistentCall->m_MethodName->str();

                switch (persistentCall->m_Mode) {
                    case PersistentListenerMode::EventDefined:
                        methodBase = targetType.GetMethod(methodName, {BNM::Defaults::Get<Parameters>()...});
                        break;
                    case PersistentListenerMode::Void:
                        methodBase = targetType.GetMethod(methodName, 0);
                        break;
                    case PersistentListenerMode::Object:
                        methodBase = targetType.GetMethod(methodName, {argumentType.Alive() ? argumentType : BNM::Defaults::Get<Object *>()});
                        break;
                    case PersistentListenerMode::Int:
                        methodBase = targetType.GetMethod(methodName, {BNM::Defaults::Get<int>()});
                        break;
                    case PersistentListenerMode::Float:
                        methodBase = targetType.GetMethod(methodName, {BNM::Defaults::Get<float>()});
                        break;
                    case PersistentListenerMode::String:
                        methodBase = targetType.GetMethod(methodName, {BNM::Defaults::Get<Structures::Mono::String *>()});
                        break;
                    case PersistentListenerMode::Bool:
                        methodBase = targetType.GetMethod(methodName, {BNM::Defaults::Get<bool>()});
                        break;
                }
                if (!methodBase.IsValid() || !methodBase._isStatic && !persistentCall->m_Target) continue;

                methodBase.cast<void>()(parameters...);
            }
        }

        /**
            @brief Invoke event using invokable calls list.
            @param parameters Event parameters
        */
        inline void InvokeCalls(Parameters ...parameters) {
            if (!m_Calls) return;

            InvokeList((Structures::Mono::List<InvokableCall<Parameters...> *> *) m_Calls->m_PersistentCalls, parameters...);
            InvokeList((Structures::Mono::List<InvokableCall<Parameters...> *> *) m_Calls->m_RuntimeCalls, parameters...);
        }

    private:
        void InvokeList(Structures::Mono::List<InvokableCall<Parameters...> *> *list, Parameters ...parameters) {
            if (!list) return;

            for (int i = 0; i < list->size; ++i) {
                auto call = list->At(i);
                if (!call || !call->action) continue;
                call->action->Invoke(parameters...);
            }
        }
    };
}