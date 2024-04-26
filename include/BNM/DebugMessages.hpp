#pragma once
#include "UserSettings/GlobalSettings.hpp"

#ifdef BNM_DEBUG

#define DBG_BNM_MSG_Image_Constructor_NotFound "Image \"%s\" - not found."
#define DBG_BNM_MSG_Image_str_nullptr "Dead image"

#define DBG_BNM_MSG_Class_str_nullptr "Dead class"
#define DBG_BNM_MSG_Class_NewList_Error "Failed to create a List for the class: %s"
#define DBG_BNM_MSG_Class_Constructor_NotFound "Class: [%s]::[%s] - not found."
#define DBG_BNM_MSG_Class_Constructor_Image_Warn "Class: [%s]::[%s]::[%s] - dead image."
#define DBG_BNM_MSG_Class_Constructor_Image_NotFound "Class: [%s]::[%s]::[%s] - not found."
#define DBG_BNM_MSG_Class_GetMethod_Count_NotFound "Method: [%s]::[%s].[%s], %d - not found"
#define DBG_BNM_MSG_Class_GetMethod_Names_NotFound DBG_BNM_MSG_Class_GetMethod_Count_NotFound " by arguments names."
#define DBG_BNM_MSG_Class_GetMethod_Types_NotFound DBG_BNM_MSG_Class_GetMethod_Count_NotFound " by arguments types."
#define DBG_BNM_MSG_Class_GetProperty_NotFound "Property: %s.(%s) - not found."
#define DBG_BNM_MSG_Class_GetInnerClass_NotFound "Inner class: [%s]::[%s]::[%s] - not found."
#define DBG_BNM_MSG_Class_GetField_NotFound "Field: [%s]::[%s].(%s) - not found."
#define DBG_BNM_MSG_Class_GetEvent_NotFound "Event: [%s]::[%s].(%s) - not found."
#define DBG_BNM_MSG_Class_CreateNewInstance_Abstract_Warn "You trying to create an object of an abstract class or interface %s?\nThis is not possible in C#."


#define DBG_BNM_MSG_CompileTimeClass_ToClass_default_Warn "[CompileTimeClassProcessors::Warn] No type of information or unknown type!"
#define DBG_BNM_MSG_CompileTimeClass_ToClass_Modifier_Warn "[CompileTimeClassProcessors::ProcessModifierInfo] Getting the class modifier before getting the class itself is impossible!"
#define DBG_BNM_MSG_CompileTimeClass_ToClass_Generic_Warn "[CompileTimeClassProcessors::ProcessGenericInfo] Getting a generic class before getting the class itself is impossible!"
#define DBG_BNM_MSG_CompileTimeClass_ToClass_OoB_Warn "[CompileTimeClass::ToClass] Type %u, greater than or equal to the number of possible handlers!"



#define DBG_BNM_MSG_String_SelfCheck_Error "ERROR: monoString dead"
#define DBG_BNM_MSG_Array_SelfCheck_Error "[monoArray<>::SelfCheck] Trying to use a dead array!"
#define DBG_BNM_MSG_List_SelfCheck_Error "[monoList<>::SelfCheck] Trying to use a dead list!"


#define DBG_BNM_MSG_FieldBase_str_nullptr "Dead field"
#define DBG_BNM_MSG_FieldBase_SetInstance_Warn "An attempt to set an object to the static field %s. Please remove the SetInstance call in the code."
#define DBG_BNM_MSG_FieldBase_GetFieldPointer_Error_instance_dead_instance "Can't get instance %s field pointer without instance! Please set instance before trying to get the pointer."
#define DBG_BNM_MSG_FieldBase_GetFieldPointer_Error_static_dead_parent "Something went wrong, the static field %s does not have a class."
#define DBG_BNM_MSG_FieldBase_GetFieldPointer_Error_thread_static_unsupported "Getting a pointer to thread static fields is not supported, field: %s."

#define DBG_BNM_MSG_Field_GetPointer_Dead "Dead"
#define DBG_BNM_MSG_Field_GetPointer_Error "Null pointer to the field [%s]"


#define DBG_BNM_MSG_MethodBase_str_nullptr "Dead method"
#define DBG_BNM_MSG_MethodBase_SetInstance_Warn "An attempt to set an object to the static method %s. Please remove the SetInstance call in the code."
#define DBG_BNM_MSG_MethodBase_str_args_count "arguments count"
#define DBG_BNM_MSG_MethodBase_str_static "static"
#define DBG_BNM_MSG_MethodBase_GetGeneric_Warn "Method %s is not generic!"
#define DBG_BNM_MSG_MethodBase_Virtualize_Warn "Failed to get the virtual version of %s method - the object is not set."

#define DBG_BNM_MSG_Method_Call_Warn "An attempt to call %s with an incorrect number of arguments... I hope you know what you're doing. BNM cannot add MethodInfo to arguments :(. Please fix it."
#define DBG_BNM_MSG_Method_Call_Error "You cannot call the instance %s method without instance! Please set instance before calling the method."


#define DBG_BNM_MSG_PropertyBase_str_nullptr "Dead property"
#define DBG_BNM_MSG_PropertyBase_str_getter "getter"
#define DBG_BNM_MSG_PropertyBase_str_setter "setter"
#define DBG_BNM_MSG_PropertyBase_str_exists "exists"
#define DBG_BNM_MSG_PropertyBase_str_not_exists "not exists"
#define DBG_BNM_MSG_PropertyBase_str_static "static"

#define DBG_BNM_MSG_Property_Get_Error "An attempt to call property %s that does not have a getter method"
#define DBG_BNM_MSG_Property_Set_Error "An attempt to call property %s that does not have a setter method"


#define DBG_BNM_MSG_EventBase_str_nullptr "Dead event"
#define DBG_BNM_MSG_EventBase_str_add "add method"
#define DBG_BNM_MSG_EventBase_str_remove "remove method"
#define DBG_BNM_MSG_EventBase_str_raise "raise method"
#define DBG_BNM_MSG_EventBase_str_exists "exists"
#define DBG_BNM_MSG_EventBase_str_not_exists "not exists"
#define DBG_BNM_MSG_EventBase_str_static "static"

#define DBG_BNM_MSG_Event_Add_Error "An attempt to call event %s that does not have add method"
#define DBG_BNM_MSG_Event_Remove_Error "An attempt to call event %s that does not have remove method"
#define DBG_BNM_MSG_Event_Raise_Error "An attempt to call event %s that does not have raise method"


#define DBG_BNM_MSG_ClassesManagement_MakeImage_Added_Image "Added new image: [%s]."
#define DBG_BNM_MSG_ClassesManagement_ModifyClasses_Target "[ModifyClasses] Target: %s"
#define DBG_BNM_MSG_ClassesManagement_ModifyClasses_Added_Method "\tAdded %smethod %s %d."
#define DBG_BNM_MSG_ClassesManagement_ModifyClasses_Hooked_Method "\tHooked %smethod %s %d."
#define DBG_BNM_MSG_ClassesManagement_ModifyClasses_Method_Static "static "
#define DBG_BNM_MSG_ClassesManagement_ModifyClasses_Added_Field "\tAdded field %s."
#define DBG_BNM_MSG_ClassesManagement_CreateClass_Added "[CreateClass] Added new class (%p) [%s]::[%s], parent - [%s]::[%s], в [%s]."
#define DBG_BNM_MSG_ClassesManagement_ProcessCustomClasses_Error "Failed find class by CompileTimeClass:"
#define DBG_BNM_MSG_ClassesManagement_LogCompileTimeClass_None "None"


#define DBG_BNM_MSG_DataIterator_Error "Null object in the iterator"

#define DBG_BNM_MSG_GetExternMethod_Warn "Extern method %s not found. Please check the code."

#endif