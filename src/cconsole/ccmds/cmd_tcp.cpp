#include "iobase/iobase.h"
#include "iobase/tcp_srv/tcp_srv.h"

void CmdTcpHandler(CIOBase &io, int argc, char *argv[]){
	CTcp *tcp = new CTcp(80);
	tcp->Init();

	tcp->SetupConsole();

	while(1)
		tcp->AcceptAndRecv();

	delete tcp;
}
