#pragma once

#include "drivers/1wire.h"

class CDs18b20{
public:
    CDs18b20(gpio_num_t pin, uint64_t addr = 0);
    ~CDs18b20();

    bool GetTemperature(float *temp);

    enum Commands{
        CONVERT_TEMPERATURE = 0x44,
        WRITE_MEM = 0x4E,
        READ_MEM = 0xBE,
        COPY_MEM = 0x48,
        FLASH_EEPROM = 0xB8,
        READ_POWER = 0xB4
    };
private:
    COneWire *m_OneWireBus;
    uint64_t m_Addr;
    gpio_num_t m_Pin;
};