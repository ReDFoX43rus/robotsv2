#ifndef UTIL_SPIBUILDER_H
#define UTIL_SPIBUILDER_H

#include <driver/spi_master.h>
#include <string.h>

class CSPIBuilder{
public:
	CSPIBuilder(){
		memset(&t, 0, sizeof(t));
	}

	CSPIBuilder &UseTxData() { t.flags |= SPI_TRANS_USE_TXDATA; return *this; }
	CSPIBuilder &UseRxData() { t.flags |= SPI_TRANS_USE_RXDATA; return *this; }
	CSPIBuilder &UseTRxData() { t.flags |= SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA; return *this; }

	CSPIBuilder &SetLength(uint8_t length) { t.length = (length + 1) * 8; return *this; }
	CSPIBuilder &SetRxLength(uint8_t rxLength) { t.rxlength = (rxLength + 1) * 8; return *this; }

	CSPIBuilder &SetTx(uint8_t *txBuffer) { t.tx_buffer = txBuffer; return *this; }
	CSPIBuilder &SetRx(uint8_t *rxBuffer) { t.rx_buffer = rxBuffer; return *this; }

	spi_transaction_t build() {return t;}
private:
	spi_transaction_t t;
};

#endif /* UTIL_SPIBUILDER_H */