# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(PROJECT_PATH)/3rd-party/esp-idf/components/nghttp/port/include $(PROJECT_PATH)/3rd-party/esp-idf/components/nghttp/nghttp2/lib/includes
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/nghttp -lnghttp
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += $(PROJECT_PATH)/3rd-party/esp-idf/components/nghttp/nghttp2
COMPONENT_LIBRARIES += nghttp
nghttp-build: 
