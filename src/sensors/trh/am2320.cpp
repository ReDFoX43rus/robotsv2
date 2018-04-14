#include "am2320.h"
#include "i2c-ths.h"
#include "esp_system.h"
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "uart.h"

#define AM2320_ADDR 0x5C
#define AM2320_READ_REG 0x03
#define TIMEOUT_MS pdMS_TO_TICKS(1500)

CAm2320::CAm2320(){
	memset(m_Buffer, 0, AM2320_MAX_BYTES_READ+4);
	m_Temperature = 0;
	m_Humidity = 0;
	m_Ok = false;
}

void CAm2320::Measure(){
	ReadRegs(0, 4);

	uint16_t receivedCrc = 0;
	receivedCrc |= (m_Buffer[7] << 8) | m_Buffer[6];

	m_Ok = receivedCrc == CRC16(m_Buffer, 6);
	if(m_Ok)
	{
		m_Humidity = ((m_Buffer[2] << 8) | m_Buffer[3])/10;

		m_Temperature = ((m_Buffer[4] & 0x7F) << 8) | m_Buffer[5];
		if ((m_Buffer[2] & 0x80) >> 8 == 1)
			m_Temperature /= -10;
		else
			m_Temperature /= 10;
	}
}
am2320_ret_t CAm2320::GetData(){
	am2320_ret_t ret = {
		.temperature = m_Temperature,
		.humidity = m_Humidity,
		.ok = m_Ok
	};
	return ret;
}

void CAm2320::Wakeup(){
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, AM2320_ADDR << 1 | I2C_MASTER_WRITE, true);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin_safe(I2C_NUM_0, cmd, TIMEOUT_MS);
	i2c_cmd_link_delete(cmd);
}

int CAm2320::ReadRegs(uint8_t startAddr, uint8_t numRegs){
	Wakeup();

	if(numRegs > AM2320_MAX_BYTES_READ)
		return -1;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, AM2320_ADDR << 1 | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, AM2320_READ_REG, true);
	i2c_master_write_byte(cmd, startAddr, true);
	i2c_master_write_byte(cmd, numRegs, true);
	i2c_master_stop(cmd);
	esp_err_t err = i2c_master_cmd_begin_safe(I2C_NUM_0, cmd, TIMEOUT_MS);
	i2c_cmd_link_delete(cmd);

	if(err != ESP_OK)
		return -2;

	ets_delay_us(1500);

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, AM2320_ADDR << 1 | I2C_MASTER_READ, true);

	for(int i = 0; i < numRegs + 3; i++)
		i2c_master_read_byte(cmd, &m_Buffer[i], I2C_MASTER_ACK);

	i2c_master_read_byte(cmd, &m_Buffer[numRegs + 3], I2C_MASTER_NACK);

	i2c_master_stop(cmd);
	err = i2c_master_cmd_begin_safe(I2C_NUM_0, cmd, TIMEOUT_MS);
	i2c_cmd_link_delete(cmd);

	if(err != ESP_OK)
		return -3;

	return 0;
}

uint16_t CAm2320::CRC16(uint8_t *byte, uint8_t numByte) {
	uint16_t crc = 0xFFFF;          // 16-bit crc register

	while (numByte) {               // loop until process all bytes
		crc ^= *byte;                   // exclusive-or crc with first byte

		for (int i = 0; i < 8; i++) {       // perform 8 shifts
			unsigned int lsb = crc & 0x01;  // extract LSB from crc
			crc >>= 1;                      // shift be one position to the right

			if (lsb == 0) {                 // LSB is 0
				continue;                   // repete the process
			}
			else {                          // LSB is 1
				crc ^= 0xA001;              // exclusive-or with 1010 0000 0000 0001
			}
		}

		numByte--;          // decrement number of byte left to be processed
		byte++;             // move to next byte
	}

	return crc;
}
