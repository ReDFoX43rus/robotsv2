#ifndef SENSORS_COMPASS_H
#define SENSORS_COMPASS_H

#include "inttypes.h"

typedef enum GaugeType{
	G_DEFAULT,
	G_DOUBLE,
	G_QUADRO,
	G_OCTA
} gauge_t;

typedef enum SensivityType{
	S1370,
	S1090,
	S820,
	S660,
	S440,
	S390,
	S330,
	S230
} sensivity_t;

typedef struct CompassData{
	short x;
	short y;
	short z;
	short angle;
} compass_data_t;

/* HMC5883L impl */
class CHMC5883L{
public:
	bool Configure(gauge_t gauge, sensivity_t sens, bool cont = true);
	compass_data_t GetPosition();
private:
	void WriteReg(uint8_t reg, uint8_t data);
	void SetReg(uint8_t reg);
};

#endif /* end of include guard: SENSORS_COMPASS_H */
