#include "controller.h"

CController::CController(gpio_num_t leftDoorClose, gpio_num_t rightDoorClose, gpio_num_t leftDoorOpen, gpio_num_t rightDoorOpen,
				gpio_num_t  leftWindowClose, gpio_num_t rightWindowClose, gpio_num_t leftWindowOpen, gpio_num_t rightWindowOpen){
	m_LeftDoorClosePin = leftDoorClose;
	m_RightDoorClosePin = rightDoorClose;
	m_LeftDoorOpenPin = leftDoorOpen;
	m_RightDoorOpenPin = rightDoorOpen;

	m_LeftWindowClosePin = leftWindowClose;
	m_RightWindowClosePin = rightWindowClose;
	m_LeftWindowOpenPin = leftWindowOpen;
	m_RightWindowOpenPin = rightWindowOpen;
}
CController::~CController(){

}

void CController::OpenCar(){
	// OpenDoor(LEFT);
	// OpenDoor(RIGHT);
	TriggerPin(m_LeftDoorOpenPin, DOOR_TRIGGER_DELAY_MS);
}

void CController::CloseCar(){
	// CloseDoor(LEFT);
	// CloseDoor(RIGHT);
	TriggerPin(m_LeftDoorClosePin, DOOR_TRIGGER_DELAY_MS);
}

void CController::OpenWindows(){
	xTaskCreate(WindowTriggerTask, "window_trigger_task_open_left", 1024, (void*)m_LeftDoorOpenPin, 4, NULL);
	xTaskCreate(WindowTriggerTask, "window_trigger_task_open_right", 1024, (void*)m_RightDoorOpenPin, 4, NULL);
}

void CController::CloseWindows(){
	xTaskCreate(WindowTriggerTask, "window_trigger_task_close_left", 1024, (void*)m_LeftDoorClosePin, 4, NULL);
	xTaskCreate(WindowTriggerTask, "window_trigger_task_close_right", 1024, (void*)m_RightDoorClosePin, 4, NULL);
}

void CController::WindowTriggerTask(void *arg){
	int pin = (int)arg;

	TriggerPin((gpio_num_t)pin, WINDOW_TRIGGER_DELAY_MS);
	vTaskDelete(NULL);
}

void CController::OpenDoor(side_t side){
	gpio_num_t pin = side == LEFT ? m_LeftDoorOpenPin : m_RightDoorOpenPin;
	TriggerPin(pin, DOOR_TRIGGER_DELAY_MS);
}

void CController::CloseDoor(side_t side){
	gpio_num_t pin = side == LEFT ? m_LeftDoorClosePin : m_RightDoorClosePin;
	TriggerPin(pin, DOOR_TRIGGER_DELAY_MS);
}

void CController::TriggerPin(gpio_num_t pin, uint32_t delay_ms){
	gpio_pad_select_gpio(pin);
	gpio_set_direction(pin, GPIO_MODE_OUTPUT);

	gpio_set_level(pin, 1);
	vTaskDelay(pdMS_TO_TICKS(delay_ms));
	gpio_set_level(pin, 0);	
}
