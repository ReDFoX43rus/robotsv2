#include "ds18b20.h"
#include "uart.h"

CDs18b20::CDs18b20(gpio_num_t pin, uint64_t addr){
    m_OneWireBus = COneWire::GetInstance(pin);
    m_Addr = addr;
}

CDs18b20::~CDs18b20(){
    COneWire::FreeInstance(m_Pin);
}

bool CDs18b20::GetTemperature(float *temp){
    *temp = 0.f;
    if(m_OneWireBus && m_OneWireBus->BeginTransaction()){

        bool result = false;
        if(m_OneWireBus->PulseReset()){
            m_OneWireBus->SendByte(COneWire::SKIP_ROM);
            m_OneWireBus->SendByte(CONVERT_TEMPERATURE);

            /* TODO: Dynamically get info from sensor */
            vTaskDelay(pdMS_TO_TICKS(750));

            m_OneWireBus->PulseReset();
            m_OneWireBus->SendByte(COneWire::SKIP_ROM);
            m_OneWireBus->SendByte(READ_MEM);

            char temp1, temp2;
            temp1 = m_OneWireBus->ReadByte();
            temp2 = m_OneWireBus->ReadByte();

            uart << "Temp1: " << (int)temp1 << " Temp2: " << (int)temp2 << endl;

            *temp = (float)((temp2 << 8) + temp1) / 16;
            result = true;
        }

        m_OneWireBus->EndTransaction();

        return result;
    }

    return false;
}