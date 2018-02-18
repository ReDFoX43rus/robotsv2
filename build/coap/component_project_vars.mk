# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(PROJECT_PATH)/3rd-party/esp-idf/components/coap/port/include $(PROJECT_PATH)/3rd-party/esp-idf/components/coap/port/include/coap $(PROJECT_PATH)/3rd-party/esp-idf/components/coap/libcoap/include $(PROJECT_PATH)/3rd-party/esp-idf/components/coap/libcoap/include/coap
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/coap -lcoap
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += $(PROJECT_PATH)/3rd-party/esp-idf/components/coap/libcoap
COMPONENT_LIBRARIES += coap
component-coap-build: 
