LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= xo3update.c

LOCAL_C_INCLUDES:=$(LOCAL_PATH)/inc
LOCAL_CFLAGS += -Wall -Wextra

LOCAL_SHARED_LIBRARIES:= \
    libcutils \
    libutils \
    liblog \
    libbinder

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= xo3update
include $(BUILD_EXECUTABLE)
