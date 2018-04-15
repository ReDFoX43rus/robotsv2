#include "1wire.h"

/* wait time is about 1 byte transaction time */
#define ONEWIRE_SEM_WAIT_TIME pdMS_TO_TICKS(800)

COneWire::COneWire(gpio_num_t pin){
    m_Pin = pin;
    m_Sem = xSemaphoreCreateMutex();

    // gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT);
    gpio_pad_select_gpio(pin);
    // gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
}

COneWire::~COneWire(){
    vSemaphoreDelete(m_Sem);
}

bool COneWire::BeginTransaction(){
    return xSemaphoreTake(m_Sem, ONEWIRE_SEM_WAIT_TIME) == pdTRUE;
}

bool COneWire::EndTransaction(){
    return xSemaphoreGive(m_Sem) == pdTRUE;
}

bool COneWire::PulseReset(){
    Output();
    Down();
    ets_delay_us(550);
    Up();
    Input();

    ets_delay_us(30 + 50);
    uint8_t level = ReadPin();
    if(level)
        return false;

    ets_delay_us(280);
    if(!level && !ReadPin())
        return false;

    return true;
}

void COneWire::SendByte(char byte){
    Output();
    
    for(int i = 0; i < 8; i++){
        if(byte & 1){
            Down();
            ets_delay_us(2);
            Up();
            ets_delay_us(80);
        } else {
            Down();
            ets_delay_us(65);
            Up();
            ets_delay_us(15);
        }

        byte = byte >> 1;
    }
}

char COneWire::ReadByte(){
    char byte = 0;

    for(int i = 0; i < 8; i++){
        Output();
        Down();
        ets_delay_us(2);
        Up();

        ets_delay_us(15);
        Input();

        byte |= (ReadPin() << i);

        ets_delay_us(15);
    }

    return byte;
}

COneWire* COneWire::s_apOneWires[GPIO_NUM_MAX] = {0};
SemaphoreHandle_t COneWire::s_ArrSem = xSemaphoreCreateMutex();

COneWire *COneWire::GetInstance(gpio_num_t pin){
    if(pin < 0 || pin >= GPIO_NUM_MAX)
        return NULL;

    if(xSemaphoreTake(s_ArrSem, ONEWIRE_SEM_WAIT_TIME) != pdTRUE)
        return NULL;

    if(s_apOneWires[pin] == NULL){
        s_apOneWires[pin] = new COneWire(pin);

        xSemaphoreGive(s_ArrSem);
        return s_apOneWires[pin];
    }

    xSemaphoreGive(s_ArrSem);

    return s_apOneWires[pin];
}

bool COneWire::FreeInstance(gpio_num_t pin){
    if(pin < 0 || pin >= GPIO_NUM_MAX)
        return false;

    if(xSemaphoreTake(s_ArrSem, ONEWIRE_SEM_WAIT_TIME) != pdTRUE)
        return NULL;

    if(s_apOneWires[pin] == NULL){
        xSemaphoreGive(s_ArrSem);
        return false;
    }

    SemaphoreHandle_t sem = s_apOneWires[pin]->m_Sem;

    if(xSemaphoreTake(sem, ONEWIRE_SEM_WAIT_TIME) != pdTRUE){
        xSemaphoreGive(s_ArrSem);
        return false;
    }
    
    xSemaphoreGive(sem);

    delete s_apOneWires[pin];
    s_apOneWires[pin] = NULL;

    xSemaphoreGive(s_ArrSem);
    return true;
}