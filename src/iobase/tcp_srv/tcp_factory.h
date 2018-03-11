#ifndef IOBASE_TCPSRV_TCPFACTORY_H
#define IOBASE_TCPSRV_TCPFACTORY_H

#define TCPFACTORY_MAX_TCPS 4

#include "tcp_srv.h"

class CTcpFactory{
public:
	static CTcpFactory *Instance();

	int CreateTcp(uint16_t port, uint32_t heartbeatDelay = 0);
	/* fixme: another access method to tcps */
	CTcp *GetTcpByPort(uint16_t port);
private:
	CTcpFactory();
	~CTcpFactory();

	bool IsPortFree(uint16_t port);
	int NumberOfFreeTcp();

	static CTcpFactory *m_Factory;
	CTcp *m_apTcps[TCPFACTORY_MAX_TCPS];
};

#endif /* end of include guard: IOBASE_TCPSRV_TCPFACTORY_H */
