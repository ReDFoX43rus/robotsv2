#include "uart.h"
#include "iobase/tcp_srv/tcp_srv.h"

void CmdTcpHandler(int argc, char *argv[]){
	CTcp *tcp = new CTcp(80);
	tcp->Init();

	tcp->AcceptAndRecv();

	delete tcp;
}
