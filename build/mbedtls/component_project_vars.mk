# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(PROJECT_PATH)/3rd-party/esp-idf/components/mbedtls/port/include $(PROJECT_PATH)/3rd-party/esp-idf/components/mbedtls/include
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/mbedtls -lmbedtls
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += mbedtls
mbedtls-build: 
