#include "compass.h"
#include "math.h"
#include "i2c-ths.h"

#define HMC_ADDR_READ 0x3D
#define HMC_ADDR_WRITE 0x3C

#define HMC_CRA 0x0 // Configuration Register A
#define HMC_CRB 0x1 // Configuration Register B
#define HMC_MR 0x2  // Mode Register

#define HMC_PTR_OUTPUT 0x3

bool CHMC5883L::Configure(gauge_t gauge, sensivity_t sens, bool cont){
	uint8_t cra = (((uint8_t)gauge) << 5) + (6 << 2); // setup gauge and 75gz(max) update rate
	uint8_t crb = ((uint8_t)sens) << 5;
	uint8_t mr = (uint8_t)!cont;

	WriteReg(HMC_CRA, cra);
	WriteReg(HMC_CRB, crb);
	WriteReg(HMC_MR, mr);

	return true;
}

compass_data_t CHMC5883L::GetPosition(){
	uint8_t data[6];

	SetReg(HMC_PTR_OUTPUT);

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, HMC_ADDR_READ, I2C_MASTER_NACK);
	i2c_master_read_byte(cmd, data,   I2C_MASTER_ACK); //"Data Output X MSB Register"
	i2c_master_read_byte(cmd, data+1, I2C_MASTER_ACK); //"Data Output X LSB Register"
	i2c_master_read_byte(cmd, data+2, I2C_MASTER_ACK); //"Data Output Z MSB Register"
	i2c_master_read_byte(cmd, data+3, I2C_MASTER_ACK); //"Data Output Z LSB Register"
	i2c_master_read_byte(cmd, data+4, I2C_MASTER_ACK); //"Data Output Y MSB Register"
	i2c_master_read_byte(cmd, data+5, I2C_MASTER_NACK); //"Data Output Y LSB Register "
	i2c_master_stop(cmd);
	i2c_master_cmd_begin_safe(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	short x = data[0] << 8 | data[1];
	short z = data[2] << 8 | data[3];
	short y = data[4] << 8 | data[5];
	short angle = (int)(atan2((double)y,(double)x) * (180 / 3.14159265) + 2*180) % 360;

	compass_data_t compass = {
		.x = x,
		.y = y,
		.z = z,
		.angle = angle
	};

	return compass;
}

void CHMC5883L::WriteReg(uint8_t reg, uint8_t data){
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);

	i2c_master_write_byte(cmd, HMC_ADDR_WRITE, I2C_MASTER_NACK);
	i2c_master_write_byte(cmd, reg, I2C_MASTER_NACK);
	i2c_master_write_byte(cmd, data, I2C_MASTER_NACK);

	i2c_master_stop(cmd);
	i2c_master_cmd_begin_safe(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
}

void CHMC5883L::SetReg(uint8_t reg){
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);

	i2c_master_write_byte(cmd, (0x1E << 1) | I2C_MASTER_WRITE, I2C_MASTER_NACK);
	i2c_master_write_byte(cmd, reg, I2C_MASTER_NACK);

	i2c_master_stop(cmd);
	i2c_master_cmd_begin_safe(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
}
