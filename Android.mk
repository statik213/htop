LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS := \
	-pedantic -Wall -std=c99 -rdynamic -D_XOPEN_SOURCE_EXTENDED \
	-DSYSCONFDIR=\"/system/etc\"

# note: keep $(LOCAL_PATH)/android first to find the correct config.h
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/android \
	$(LOCAL_PATH)/linux \
	external/libncurses/include \

# from Makefile.am myhtopsources
LOCAL_SRC_FILES := \
  AvailableMetersPanel.c CategoriesPanel.c CheckItem.c \
  ClockMeter.c ColorsPanel.c ColumnsPanel.c CPUMeter.c CRT.c MainPanel.c \
  DisplayOptionsPanel.c FunctionBar.c Hashtable.c Header.c htop.c ListItem.c \
  LoadAverageMeter.c MemoryMeter.c Meter.c MetersPanel.c Object.c Panel.c \
  BatteryMeter.c Process.c ProcessList.c RichString.c ScreenManager.c Settings.c \
  SignalsPanel.c StringUtils.c SwapMeter.c TasksMeter.c UptimeMeter.c \
  TraceScreen.c UsersTable.c Vector.c AvailableColumnsPanel.c AffinityPanel.c \
  HostnameMeter.c OpenFilesScreen.c Affinity.c IncSet.c Action.c EnvScreen.c \
  InfoScreen.c XAlloc.c

# from Makefile.am myhtopplatsources
LOCAL_SRC_FILES += \
  linux/Platform.c linux/IOPriorityPanel.c linux/IOPriority.c \
  linux/LinuxProcess.c linux/LinuxProcessList.c linux/LinuxCRT.c linux/Battery.c

LOCAL_SHARED_LIBRARIES := libncurses

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := htop

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)

include $(BUILD_EXECUTABLE)
