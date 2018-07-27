#ifndef USERSPACE_MANAGER_H
#define USERSPACE_MANAGER_H

#include <driver/gpio.h>
#include <drivers/nrf24l01p_lib.h>
#include <drivers/display.h>
#include <userspace/variables.h>

#define MANAGER_WORKING_CORE 0

class CManager{
public:
	CManager(gpio_num_t btnDO, gpio_num_t btnDC, gpio_num_t btnWO) : 
		m_BtnDoorsOpen(btnDO), m_BtnDoorsClose(btnDC), m_BtnWindowsOpen(btnWO) {}
	~CManager();

	void SetupNrf(spi_host_device_t spi, gpio_num_t cs, gpio_num_t ce);
	void SetupButtons();
	void SetupDisplay(uint16_t width, uint16_t height);

	void DetachInterrupts();

private:
	gpio_num_t m_BtnDoorsOpen;
	gpio_num_t m_BtnDoorsClose;
	gpio_num_t m_BtnWindowsOpen;

	bool m_InterruptsAttached;
	uint8_t m_RunningTasksCounter;

	CNRFLib *m_Nrf;
	COledDisplay *m_Display;

	void SendCmd(controller_cmds_t cmd);

	static bool CanRunTask(void *pData);

	static void OnBtnDoorsOpenPressed(void *pData);
	static void BtnDoorsOpenTask(void *pData);

	static void OnBtnDoorClosePressed(void *pData);
	static void BtnDoorsCloseTask(void *pData);

	static void OnBtnWindowsOpenPressed(void *pData);
	static void BtnWindowsOpenTask(void *pData);
};

#endif /* USERSPACE_MANAGER_H */