#ifndef DRIVERS_FREQCOUNTER_H
#define DRIVERS_FREQCOUNTER_H

#include "driver/gpio.h"
#include "driver/pcnt.h"

class CPulseHandler{
public:
    static CPulseHandler* GetInstance(pcnt_unit_t pcnt_unit);
    static bool IsUnitFree(pcnt_unit_t pcnt_unit);
    static bool RemoveInstance(pcnt_unit_t pcnt_unit);

    CPulseHandler &LoadTmpConfig(pcnt_channel_t channel);
    CPulseHandler &ClearTmpConfig();
    CPulseHandler &SetPulseGPIO(gpio_num_t gpio);
    CPulseHandler &SetControlGPIO(gpio_num_t gpio);
    CPulseHandler &SetLowControlMode(pcnt_ctrl_mode_t mode);
    CPulseHandler &SetHighControlMode(pcnt_ctrl_mode_t mode);
    CPulseHandler &SetPosMode(pcnt_count_mode_t mode);
    CPulseHandler &SetNegMode(pcnt_count_mode_t mode);
    CPulseHandler &SetHighLimit(int limit);
    CPulseHandler &SetLowLimit(int limit);
    bool ApplyConfig(pcnt_channel_t channel);

    bool FilterEnable();
    bool FilterDisable();
    bool SetFilterValue(uint16_t value);
    uint16_t GetFilterValue();

    bool Pause();
    bool Resume();
    void Clear();

    short GetValue();

    uint32_t GetFrequency(uint32_t delay_ms = 10);

private:
    static CPulseHandler *s_apHandlers[PCNT_UNIT_MAX];

    CPulseHandler(pcnt_unit_t pcnt_unit);
    ~CPulseHandler();

    pcnt_unit_t m_Unit;
    pcnt_config_t m_Configs[PCNT_CHANNEL_MAX];
    pcnt_config_t m_TmpConfig;

    bool m_Paused;
};

#endif /* end of include guard: DRIVERS_FREQCOUNTER_H */
