#pragma once
#include "UserSettings/GlobalSettings.hpp"

#ifdef BNM_DEBUG

#define DBG_BNM_MSG_Image_Constructor_NotFound "Образ \"%s\" - не найден."
#define DBG_BNM_MSG_Image_str_nullptr "Мёртвый образ"

#define DBG_BNM_MSG_Class_str_nullptr "Мёртвый класс"
#define DBG_BNM_MSG_Class_NewList_Error "Не удалось создать List для класса: %s"
#define DBG_BNM_MSG_Class_Constructor_NotFound "Класс: [%s]::[%s] - не найден."
#define DBG_BNM_MSG_Class_Constructor_Image_Warn "Класс: [%s]::[%s]::[%s] - образ мёртв."
#define DBG_BNM_MSG_Class_Constructor_Image_NotFound "Класс: [%s]::[%s]::[%s] - не найден."
#define DBG_BNM_MSG_Class_GetMethod_Count_NotFound "Метод: [%s]::[%s].[%s], %d - не найден"
#define DBG_BNM_MSG_Class_GetMethod_Names_NotFound DBG_BNM_MSG_Class_GetMethod_Count_NotFound " использовались имена аргументов."
#define DBG_BNM_MSG_Class_GetMethod_Types_NotFound DBG_BNM_MSG_Class_GetMethod_Count_NotFound " использовались типы аргументов."
#define DBG_BNM_MSG_Class_GetProperty_NotFound "Свойство: %s.(%s) - не найдено."
#define DBG_BNM_MSG_Class_GetInnerClass_NotFound "Вложенный класс: [%s]::[%s]::[%s] - не найден."
#define DBG_BNM_MSG_Class_GetField_NotFound "Поле: [%s]::[%s].(%s) - не найдено."
#define DBG_BNM_MSG_Class_CreateNewInstance_Abstract_Warn "Вы пытаетесь создать объект абстрактного класса или интерфейса %s?\nВ C# это невозможно."


#define DBG_BNM_MSG_CompileTimeClass_ToClass_default_Warn "[CompileTimeClassProcessors::Warn] Нет типа информации или неизвестный тип!"
#define DBG_BNM_MSG_CompileTimeClass_ToClass_Modifier_Warn "[CompileTimeClassProcessors::ProcessModifierInfo] Получения типа класса до получения самого класса невозможно!"
#define DBG_BNM_MSG_CompileTimeClass_ToClass_Generic_Warn "[CompileTimeClassProcessors::ProcessGenericInfo] Получения generic класса до получения самого класса невозможно!"
#define DBG_BNM_MSG_CompileTimeClass_ToClass_OoB_Warn "[CompileTimeClass::ToClass] Тип %u, больше или равен количеству возможных обработчиков!"



#define DBG_BNM_MSG_monoString_SelfCheck_Error "ОШИБКА: monoString мёртв"
#define DBG_BNM_MSG_monoArray_SelfCheck_Error "[monoArray<>::SelfCheck] Попытка использовать мёртвый массив!"
#define DBG_BNM_MSG_monoList_SelfCheck_Error "[monoList<>::SelfCheck] Попытка использовать мёртвый список!"


#define DBG_BNM_MSG_FieldBase_str_nullptr "Мёртвое поле"
#define DBG_BNM_MSG_FieldBase_SetInstance_Warn "Попытка установить объект статическому полю %s. Пожалуйста, уберите вызов SetInstance в коде."
#define DBG_BNM_MSG_FieldBase_GetFieldPointer_Error_instance_dead_instance "Не могу получить не статический указатель поля %s без объекта! Пожалуйста, установите объект перед попыткой получить указатель."
#define DBG_BNM_MSG_FieldBase_GetFieldPointer_Error_static_dead_parent "Что-то пошло не так, статическое поле %s не имеет класс."
#define DBG_BNM_MSG_FieldBase_GetFieldPointer_Error_thread_static_unsupported "Получение указателя на статические поля потоков не поддерживается, поле: %s."

#define DBG_BNM_MSG_Field_GetPointer_Dead "Мёртво"
#define DBG_BNM_MSG_Field_GetPointer_Error "Пустой указатель на поле [%s]"


#define DBG_BNM_MSG_MethodBase_str_nullptr "Мёртвый метод"
#define DBG_BNM_MSG_MethodBase_SetInstance_Warn "Попытка установить объект статическому методу %s. Пожалуйста, уберите вызов SetInstance в коде."
#define DBG_BNM_MSG_MethodBase_str_args_count "кол-во аргументов"
#define DBG_BNM_MSG_MethodBase_str_static "статический"
#define DBG_BNM_MSG_MethodBase_GetGeneric_Warn "Метод %s не generic!"
#define DBG_BNM_MSG_MethodBase_Virtualize_Warn "Не удалось получить virtual версию метода %s - объект не установлен."

#define DBG_BNM_MSG_Method_Call_Warn "Попытка вызвать %s с неправильным кол-вом аргументов... Я надеюсь, вы знаете, что делаете. BNM не может добавить MethodInfo к аргументам :(. Пожалуйста, исправьте это."
#define DBG_BNM_MSG_Method_Call_Error "Нельзя вызвать нестатический метод %s без объекта! Пожалуйста, установите объект перед вызовом метода."


#define DBG_BNM_MSG_PropertyBase_str_nullptr "Мёртвое свойство"
#define DBG_BNM_MSG_PropertyBase_str_getter "метод получения"
#define DBG_BNM_MSG_PropertyBase_str_setter "метод установки"
#define DBG_BNM_MSG_PropertyBase_str_exists "есть"
#define DBG_BNM_MSG_PropertyBase_str_not_exists "нет"
#define DBG_BNM_MSG_PropertyBase_str_static "статическое"

#define DBG_BNM_MSG_Property_Get_Error "Попытка вызвать свойство %s в котором отсутствует метод получения"
#define DBG_BNM_MSG_Property_Set_Error "Попытка вызвать свойство %s в котором отсутствует метод установки"


#define DBG_BNM_MSG_ClassesManagement_MakeImage_Added_Image "Добавлен новый образ: [%s]."
#define DBG_BNM_MSG_ClassesManagement_ModifyClasses_Target "[ModifyClasses] Цель: %s"
#define DBG_BNM_MSG_ClassesManagement_ModifyClasses_Added_Method "\tДобавлен %sметод %s %d."
#define DBG_BNM_MSG_ClassesManagement_ModifyClasses_Hooked_Method "\tПодменён %sметод %s %d."
#define DBG_BNM_MSG_ClassesManagement_ModifyClasses_Method_Static "статический "
#define DBG_BNM_MSG_ClassesManagement_ModifyClasses_Added_Field "\tДобавлено поле %s."
#define DBG_BNM_MSG_ClassesManagement_CreateClass_Added "[CreateClass] Добавлен новый класс (%p) [%s]::[%s], родитель которого - [%s]::[%s], в [%s]."
#define DBG_BNM_MSG_ClassesManagement_ProcessCustomClasses_Error "Не удалось найти класс по CompileTimeClass:"
#define DBG_BNM_MSG_ClassesManagement_LogCompileTimeClass_None "Пусто"


#define DBG_BNM_MSG_DataIterator_Error "Пустой объект в итераторе"

#define DBG_BNM_MSG_GetExternMethod_Warn "Внешний метод %s не найден. Пожалуйста, проверьте код."

#endif