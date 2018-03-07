#include "tcp_factory.h"
#include "string.h"

CTcpFactory::CTcpFactory(){
	for(int i = 0; i < TCPFACTORY_MAX_TCPS; i++)
		m_apTcps[i] = NULL;
}
CTcpFactory::~CTcpFactory(){
	for(int i = 0; i < TCPFACTORY_MAX_TCPS; i++)
		if(m_apTcps[i])
			delete m_apTcps[i];
}

int CTcpFactory::CreateTcp(uint16_t port){
	if(!IsPortFree(port))
		return -1;

	int n = NumberOfFreeTcp();
	if(n == -1)
		return -2;

	m_apTcps[n] = new CTcp(port);
	return m_apTcps[n]->Init() * 10;
}

CTcp *CTcpFactory::GetTcpByPort(uint16_t port){
	for(int i = 0; i < TCPFACTORY_MAX_TCPS; i++)
		if(m_apTcps[i] && m_apTcps[i]->GetPort() == port)
			return m_apTcps[i];

	return NULL;
}

inline bool CTcpFactory::IsPortFree(uint16_t port){
	return GetTcpByPort(port) == NULL;
}

int CTcpFactory::NumberOfFreeTcp(){
	for(int i = 0; i < TCPFACTORY_MAX_TCPS; i++)
		if(!m_apTcps[i])
			return i;
	return -1;
}

CTcpFactory *CTcpFactory::m_Factory = NULL;

CTcpFactory *CTcpFactory::Instance(){
	if(!CTcpFactory::m_Factory){
		CTcpFactory::m_Factory = new CTcpFactory();
		return CTcpFactory::m_Factory;
	} else return CTcpFactory::m_Factory;
}
