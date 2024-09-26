BNM_LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := BNM

LOCAL_CPPFLAGS += -std=c++20

INCLUDE_DIRS := $(BNM_LOCAL_PATH)/include \
                $(BNM_LOCAL_PATH)/external/include \
                $(BNM_LOCAL_PATH)/external \
                $(BNM_LOCAL_PATH)/external/utf8 \
                $(BNM_LOCAL_PATH)/src/private

LOCAL_SRC_FILES := \
    $(BNM_LOCAL_PATH)/src/Class.cpp \
    $(BNM_LOCAL_PATH)/src/ClassesManagement.cpp \
    $(BNM_LOCAL_PATH)/src/Coroutine.cpp \
    $(BNM_LOCAL_PATH)/src/Delegates.cpp \
    $(BNM_LOCAL_PATH)/src/Defaults.cpp \
    $(BNM_LOCAL_PATH)/src/EventBase.cpp \
    $(BNM_LOCAL_PATH)/src/Exceptions.cpp \
    $(BNM_LOCAL_PATH)/src/FieldBase.cpp \
    $(BNM_LOCAL_PATH)/src/Hooks.cpp \
    $(BNM_LOCAL_PATH)/src/Image.cpp \
    $(BNM_LOCAL_PATH)/src/Internals.cpp \
    $(BNM_LOCAL_PATH)/src/Loading.cpp \
    $(BNM_LOCAL_PATH)/src/MethodBase.cpp \
    $(BNM_LOCAL_PATH)/src/MonoStructures.cpp \
    $(BNM_LOCAL_PATH)/src/PropertyBase.cpp \
    $(BNM_LOCAL_PATH)/src/UnityStructures.cpp \
    $(BNM_LOCAL_PATH)/src/Utils.cpp

LOCAL_C_INCLUDES := $(INCLUDE_DIRS)

include $(BUILD_STATIC_LIBRARY)
