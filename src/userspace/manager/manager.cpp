#include "manager.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

CManager::~CManager(){
	DetachInterrupts();
	if(m_Nrf != NULL)
		delete m_Nrf;
	
	if(m_Display != NULL)
		delete m_Display;
}

void CManager::SetupNrf(spi_host_device_t spi, gpio_num_t cs, gpio_num_t ce){
	m_Nrf = new CNRFLib(cs, ce);
	m_Nrf->AttachToSpiBus(spi);
	m_Nrf->Begin(nrf_tx_mode);
	m_Nrf->SetTxAddr(g_NrfAddr, 5);
	m_Nrf->SetPipeAddr(0, g_NrfAddr, 5);
}

void CManager::SetupButtons(){
	assert(!m_InterruptsAttached);

	gpio_num_t irqs[3] = {m_BtnDoorsOpen, m_BtnDoorsClose, m_BtnWindowsOpen};
	gpio_isr_t handlers[3] = {OnBtnDoorsOpenPressed, OnBtnDoorClosePressed, OnBtnWindowsOpenPressed};

	gpio_num_t irq;
	gpio_isr_t handler;

	gpio_install_isr_service(ESP_INTR_FLAG_EDGE);
	for(int i = 0; i < 3; i++){
		irq = irqs[i];
		handler = handlers[i];

		gpio_set_direction(irq, GPIO_MODE_INPUT);
		gpio_set_pull_mode(irq, GPIO_PULLUP_ONLY);

		gpio_intr_enable(irq);
		gpio_set_intr_type(irq, GPIO_INTR_NEGEDGE);
		gpio_isr_handler_add(irq, handler, this);
	}
}

void CManager::SetupDisplay(uint16_t width, uint16_t height){
	m_Display = new COledDisplay(width, height);
	m_Display->SetBrightness(0xFF);
}

void CManager::DetachInterrupts(){
	if(!m_InterruptsAttached)
		return;

	gpio_num_t irqs[3] = {m_BtnDoorsOpen, m_BtnDoorsClose, m_BtnWindowsOpen};

	gpio_num_t irq;
	for(int i = 0; i < 3; i++){
		irq = irqs[3];

		gpio_intr_disable(irq);
		gpio_isr_handler_remove(irq);
	}
	gpio_uninstall_isr_service();
}

void CManager::SendCmd(controller_cmds_t cmd){
	if(m_Nrf == NULL)
		return;

	uint8_t buffer[NRF_MAX_PAYLOAD] = {0};
	buffer[0] = cmd;
	m_Nrf->Send(buffer, NRF_MAX_PAYLOAD);
}

bool CManager::CanRunTask(void *pData){
	if(pData == NULL)
		return false;

	CManager *manager = (CManager*)pData;
	if(manager->m_RunningTasksCounter != 0)
		return false;

	manager->m_RunningTasksCounter++;
	return true;
}

void CManager::OnBtnDoorsOpenPressed(void *pData){
	if(!CanRunTask(pData))
		return;

	xTaskCreatePinnedToCore(BtnDoorsOpenTask, "mng_do", 1024, pData, 4, NULL, MANAGER_WORKING_CORE);
}

void CManager::BtnDoorsOpenTask(void *pData){
	CManager *manager = (CManager*)pData;
	manager->SendCmd(CMD_OPEN_DOORS);

	manager->m_RunningTasksCounter--;
	vTaskDelete(NULL);
}

void CManager::OnBtnDoorClosePressed(void *pData){
	if(!CanRunTask(pData))
		return;

	xTaskCreatePinnedToCore(BtnDoorsCloseTask, "mng_dc", 1024, pData, 4, NULL, MANAGER_WORKING_CORE);
}

void CManager::BtnDoorsCloseTask(void *pData){
	if(pData == NULL)
		return;

	CManager *manager = (CManager*)pData;
	manager->SendCmd(CMD_CLOSE_DOORS);
	
	manager->m_RunningTasksCounter--;
	vTaskDelete(NULL);
}

void CManager::OnBtnWindowsOpenPressed(void *pData){
	if(!CanRunTask(pData))
		return;

	xTaskCreatePinnedToCore(BtnWindowsOpenTask, "mng_wo", 1024, pData, 4, NULL, MANAGER_WORKING_CORE);
}

void CManager::BtnWindowsOpenTask(void *pData){
	if(pData == NULL)
		return;

	CManager *manager = (CManager*)pData;
	manager->SendCmd(CMD_OPEN_WINDOWS);
	
	manager->m_RunningTasksCounter--;
	vTaskDelete(NULL);
}