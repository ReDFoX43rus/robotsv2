#include "dht11.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "uart.h"

static inline uint32_t timestamp(void){
    uint32_t ccount;
    asm volatile ("rsr %0, ccount" : "=r"(ccount));

    /* This devider will work only if esp32 is running on 16MHz freq 
     * TODO: remove hardcoded value */
    return ccount / 160;
}

dht11_response_t CDht11::Measure(){
    dht11_response_t res = {0, 0, 0, 0, 0}; // compiler, plz

    gpio_pad_select_gpio(m_Pin);
    gpio_pad_select_gpio(GPIO_NUM_5);
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);

    Output();
    Down();
    vTaskDelay(pdMS_TO_TICKS(18));
    Up();

    Input();

    uint8_t timeout = 100;
    while(timeout-- && ReadPin())
        ets_delay_us(1);

    timeout = 100;
    while(timeout-- && ReadPin() == 0){
        ets_delay_us(1);
    }

    if(!timeout){
        uart << "Error in first 80us" << endl;
        return res;
    }

    timeout = 100;
    while(timeout-- && ReadPin() == 1){
        ets_delay_us(1);
    }
    

    uint32_t data = 0;
    uint8_t checksum = 0;

    uint8_t timeouts[40];

    for(int i = 0; i < 32; i++){
        timeout = 65;
        while(timeout-- && ReadPin() == 0){
            ets_delay_us(1);
        }

        if(!timeout){
            uart << "Error" << endl;
            break;
        }

        timeout = 90;
        while(timeout-- && ReadPin() == 1){
            ets_delay_us(1);
        }

        if(!timeout){
            uart << "Error 2" << endl;
            break;
        }

        timeouts[i] = timeout;

        if(timeout < 50){
            data |= (1 << (31 - i));
        }
    }

    for(int i = 0; i < 8; i++){
        timeout = 65;
        while(timeout-- && ReadPin() == 0){
            ets_delay_us(1);
        }

        if(!timeout){
            uart << "Error" << endl;
            break;
        }

        timeout = 90;
        while(timeout-- && ReadPin() == 1){
            ets_delay_us(1);
        }

        if(!timeout){
            uart << "Error 2" << endl;
            break;
        }

        timeouts[32 + i] = timeout;

        if(timeout < 50){
            checksum |= (1 << (7 - i));
        }
    }

    res.rh1 = (data >> 8) & 0xFF;
    res.rh2 = (data) & 0xFF;
    res.temp1 = (data >> 24) & 0xFF;
    res.temp2 = (data >> 16) & 0xFF;

    res.ok = checksum == (res.temp1 + res.rh1);

    return res;
}

int CDht11::WaitAnotherLevel(int &currentLevel, int timeout){
    while(timeout--){
        if(ReadPin() != currentLevel){
            currentLevel = !currentLevel;
            break;
        }

        ets_delay_us(1);
    }

    return timeout;
}