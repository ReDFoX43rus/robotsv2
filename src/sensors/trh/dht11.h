#pragma once

#include "driver/gpio.h"

typedef struct {
    char temp1;
    char temp2;
    uint8_t rh1;
    uint8_t rh2;
    uint8_t ok;
} dht11_response_t;

class CDht11{
public:
    CDht11(gpio_num_t pin):m_Pin(pin) {}
    ~CDht11(){}

    dht11_response_t Measure();

private:
    gpio_num_t m_Pin;

    int WaitAnotherLevel(int &currentLevel, int timeout);

    /* GPIO-related functions */
    void Down() {gpio_set_level(m_Pin, 0);}
    void Up() {gpio_set_level(m_Pin, 1);}
    void Input() {gpio_set_direction(m_Pin, GPIO_MODE_INPUT);}
    void Output() {gpio_set_direction(m_Pin, GPIO_MODE_OUTPUT);}
    
    uint8_t ReadPin() {return gpio_get_level(m_Pin);}
};