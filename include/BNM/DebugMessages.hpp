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
#define DBG_BNM_MSG_Class_CreateNewInstance_Abstract_Warn "You are trying to create an object of an abstract class or interface %s?\nThis is not possible in C#."
#define DBG_BNM_MSG_Class_Dead_Error "An attempt to use dead class!"


#define DBG_BNM_MSG_CompileTimeClass_ToClass_default_Warn "[CompileTimeClassProcessors::Warn] No type of information or unknown type!"
#define DBG_BNM_MSG_CompileTimeClass_ToClass_Inner_Warn "[CompileTimeClassProcessors::ProcessModifierInfo] Getting an inner class before getting the main class is impossible!"
#define DBG_BNM_MSG_CompileTimeClass_ToClass_Modifier_Warn "[CompileTimeClassProcessors::ProcessModifierInfo] Getting the class modifier before getting the class itself is impossible!"
#define DBG_BNM_MSG_CompileTimeClass_ToClass_Generic_Warn "[CompileTimeClassProcessors::ProcessGenericInfo] Getting a generic class before getting the class itself is impossible!"
#define DBG_BNM_MSG_CompileTimeClass_ToClass_OoB_Warn "[CompileTimeClass::ToClass] Type %lu, greater than or equal to the number of possible handlers!"



#define DBG_BNM_MSG_String_str_Error "ERROR: Mono::String dead"
#define DBG_BNM_MSG_String_SelfCheck_Error "[String::SelfCheck] Trying to use a dead string!"
#define DBG_BNM_MSG_Array_SelfCheck_Error "[monoArray<>::SelfCheck] Trying to use a dead array!"
#define DBG_BNM_MSG_List_SelfCheck_Error "[monoList<>::SelfCheck] Trying to use a dead list!"


#define DBG_BNM_MSG_FieldBase_str_nullptr "Dead field"
#define DBG_BNM_MSG_FieldBase_SetInstance_Warn "An attempt to set an object to the static field %s. Please remove the SetInstance call in the code."
#define DBG_BNM_MSG_FieldBase_SetInstance_Wrong_Instance_Error "An attempt to set an object of type %s to field %s!"
#define DBG_BNM_MSG_FieldBase_GetFieldPointer_Error_instance_dead_instance "Can't get instance %s field pointer without instance! Please set instance before trying to get the pointer."
#define DBG_BNM_MSG_FieldBase_GetFieldPointer_Error_static_dead_parent "Something went wrong, the static field %s does not have a class."
#define DBG_BNM_MSG_FieldBase_GetFieldPointer_Error_thread_static_unsupported "Getting a pointer to thread static fields is not supported, field: %s."
#define DBG_BNM_MSG_FieldBase_GetFieldPointer_Error_const_impossible "Getting a pointer to const fields is impossible, field: %s."

#define DBG_BNM_MSG_Field_GetPointer_Dead "Dead"
#define DBG_BNM_MSG_Field_GetPointer_Error "Null pointer in the field %s!"
#define DBG_BNM_MSG_Field_GetSet_Error "An attempt to use dead field!"
#define DBG_BNM_MSG_Field_Set_const_Error "Setting a const fields is impossible, field: %s."


#define DBG_BNM_MSG_MethodBase_str_nullptr "Dead method"
#define DBG_BNM_MSG_MethodBase_SetInstance_Warn "An attempt to set an object to the static method %s. Please remove the SetInstance call in the code."
#define DBG_BNM_MSG_MethodBase_SetInstance_Wrong_Instance_Error "An attempt to set an object of type %s to method %s!"
#define DBG_BNM_MSG_MethodBase_str_args_count "arguments count"
#define DBG_BNM_MSG_MethodBase_str_static "static"
#define DBG_BNM_MSG_MethodBase_GetGeneric_Warn "Method %s is not generic!"
#define DBG_BNM_MSG_MethodBase_Virtualize_Warn "Failed to get the virtual version of %s method - the object is not set."

#define DBG_BNM_MSG_Method_Call_Dead "An attempt to call dead method!"
#define DBG_BNM_MSG_Method_Call_Warn "An attempt to call %s with an incorrect number of arguments... I hope you know what you're doing. BNM cannot add MethodInfo to arguments :(. Please fix it."
#define DBG_BNM_MSG_Method_Call_Error "You cannot call the instance %s method without instance! Please set instance before calling the method."


#define DBG_BNM_MSG_PropertyBase_str_nullptr "Dead property"
#define DBG_BNM_MSG_PropertyBase_str_getter "getter"
#define DBG_BNM_MSG_PropertyBase_str_setter "setter"
#define DBG_BNM_MSG_PropertyBase_str_exists "exists"
#define DBG_BNM_MSG_PropertyBase_str_not_exists "not exists"
#define DBG_BNM_MSG_PropertyBase_str_static "static"

#define DBG_BNM_MSG_Property_Get_Error "An attempt to call property's (%s) getter that does not exist"
#define DBG_BNM_MSG_Property_Set_Error "An attempt to call property's (%s) setter that does not exist"


#define DBG_BNM_MSG_EventBase_str_nullptr "Dead event"
#define DBG_BNM_MSG_EventBase_str_add "add method"
#define DBG_BNM_MSG_EventBase_str_remove "remove method"
#define DBG_BNM_MSG_EventBase_str_raise "raise method"
#define DBG_BNM_MSG_EventBase_str_exists "exists"
#define DBG_BNM_MSG_EventBase_str_not_exists "not exist"
#define DBG_BNM_MSG_EventBase_str_static "static"

#define DBG_BNM_MSG_Event_Add_Error "An attempt to call event's (%s) add method that does not exist"
#define DBG_BNM_MSG_Event_Remove_Error "An attempt to call event's (%s) remove method that does not exist"
#define DBG_BNM_MSG_Event_Raise_Error "An attempt to call event's (%s) raise method that does not exist"


#define DBG_BNM_MSG_ClassesManagement_Method_Static "static "
#define DBG_BNM_MSG_ClassesManagement_MakeImage_Added_Image "Added new image: [%s]."
#define DBG_BNM_MSG_ClassesManagement_ModifyClasses_Target "[ModifyClasses] Target: %s"
#define DBG_BNM_MSG_ClassesManagement_ModifyClasses_Added_Method "\tAdded %smethod %s %lu."
#define DBG_BNM_MSG_ClassesManagement_ModifyClasses_Hooked_Method "\tHooked %smethod %s %lu."
#define DBG_BNM_MSG_ClassesManagement_ModifyClasses_Overridden_Method "\t\tOverridden method - %s."
#define DBG_BNM_MSG_ClassesManagement_ModifyClasses_Added_Field "\tAdded field %s."
#define DBG_BNM_MSG_ClassesManagement_CreateClass_Target "[CreateClass] Target: [%s]::[%s]::[%s]"
#define DBG_BNM_MSG_ClassesManagement_CreateClass_Added_Method "\tAdded %smethod %s %lu."
#define DBG_BNM_MSG_ClassesManagement_CreateClass_Overridden_Method "\t\tOverridden method - %s."
#define DBG_BNM_MSG_ClassesManagement_ProcessCustomClasses_Error "Failed find class by CompileTimeClass:"
#define DBG_BNM_MSG_ClassesManagement_LogCompileTimeClass_None "None"


#define DBG_BNM_MSG_DataIterator_Error "Null object in the iterator"

#define DBG_BNM_MSG_GetExternMethod_Warn "Extern method %s isn't found. Please check the code."


#define DBG_BNM_MSG_TryLoadByJNI_Fail "Failed to load libil2cpp.so by path!"


#define DBG_BNM_MSG_SetupBNM_Class_Init "[SetupBNM] il2cpp::vm::Class::Init in lib: %p."
#define DBG_BNM_MSG_SetupBNM_Image_GetTypes "[SetupBNM] il2cpp::vm::Image::GetTypes in lib: %p."
#define DBG_BNM_MSG_SetupBNM_image_get_class_exists "[SetupBNM] code has il2cpp_image_get_class. BNM will use it."
#define DBG_BNM_MSG_SetupBNM_Class_FromIl2CppType "[SetupBNM] il2cpp::vm::Class::FromIl2CppType in lib: %p."
#define DBG_BNM_MSG_SetupBNM_Type_GetClassOrElementClass "[SetupBNM] il2cpp::vm::Type::GetClassOrElementClass in lib: %p."
#define DBG_BNM_MSG_SetupBNM_Image_FromName "[SetupBNM] il2cpp::vm::Image::FromName in lib: %p."
#define DBG_BNM_MSG_SetupBNM_MetadataCache_GetImageFromIndex "[SetupBNM] il2cpp::vm::MetadataCache::GetImageFromIndex in lib: %p."
#define DBG_BNM_MSG_SetupBNM_Assembly_Load "[SetupBNM] il2cpp::vm::Assembly::Load in lib: %p."
#define DBG_BNM_MSG_SetupBNM_Assembly_GetAllAssemblies "[SetupBNM] il2cpp::vm::Assembly::GetAllAssemblies in lib: %p."

#endif