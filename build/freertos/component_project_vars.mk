# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(PROJECT_PATH)/3rd-party/esp-idf/components/freertos/include
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/freertos -lfreertos -Wl,--undefined=uxTopUsedPriority
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += freertos
freertos-build: 
