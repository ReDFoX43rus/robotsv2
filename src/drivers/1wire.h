#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

class COneWire{
public:
    /* It's highly recommended to call this function before every transaction 
     * Since your current COneWire instance can be deleted by another thread 
     * P.S. Look at FreeInstance */
    static COneWire *GetInstance(gpio_num_t pin);
    
    /* This function may take some time to execute
     * since we have to wait last transaction to end */
    static bool FreeInstance(gpio_num_t pin);

    /* Locks semaphore
     * Returns true on success
     * NOTE: You have to do transtaction ASAP, 
     * don't forget to EndTransaction */
    bool BeginTransaction();

    /* Releases semaphore
     * Returns true on success */
    bool EndTransaction();

    /* ============================================================== */
    /* The functions below should be called only with taken semaphore */
    /* ============================================================== */

    void SendByte(char byte); // send byte to bus
    char ReadByte(); // read byte from bus

    /* Send reset pulse on bus
     * Returnes true if any slaves exist */
    bool PulseReset();

    enum Commands{
        FIND_ROM = 0xF0,
        READ_ROM = 0x33,
        MATCH_ROM = 0x55,
        SKIP_ROM = 0xCC,
        FIND_WARNING = 0xEC
    };
private:
    static COneWire *s_apOneWires[GPIO_NUM_MAX];
    static SemaphoreHandle_t s_ArrSem;

    COneWire(gpio_num_t pin);
    ~COneWire();

    gpio_num_t m_Pin;

    SemaphoreHandle_t m_Sem;

    /* GPIO-related functions */
    void Down() {gpio_set_level(m_Pin, 0);}
    void Up() {gpio_set_level(m_Pin, 1);}
    void Input() {gpio_set_direction(m_Pin, GPIO_MODE_INPUT);}
    void Output() {gpio_set_direction(m_Pin, GPIO_MODE_OUTPUT);}
    
    uint8_t ReadPin() {return gpio_get_level(m_Pin);}
};