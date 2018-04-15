#ifndef SENSORS_TRH_AM2320_H
#define SENSORS_TRH_AM2320_H

#define AM2320_MAX_BYTES_READ 10

#include "inttypes.h"

typedef struct {
	short temperature;
	short humidity;
	bool ok;
} am2320_ret_t;

class CAm2320{
public:
	CAm2320();

	void Measure();

	am2320_ret_t GetData();
	short GetTemperature() {return m_Temperature;}
	short GetHumidity() {return m_Humidity;}
private:
	short m_Temperature;
	short m_Humidity;
	bool m_Ok;

	uint8_t m_Buffer[AM2320_MAX_BYTES_READ+4];

	void Wakeup();
	int ReadRegs(uint8_t startAddr, uint8_t numRegs);
	uint16_t CRC16(uint8_t *byte, uint8_t numByte);
};

#endif /* end of include guard: SENSORS_TRH_AM2320_H */
