#include "iobase/iobase.h"
#include "iobase/tcp_srv/tcp_factory.h"

static uint16_t port = 3000;

void CmdTcpHandler(CIOBase &io, int argc, char *argv[]){
	if(argc < 2){
		io << "Usage: " << argv[0] << " cmd_num" << endl;
		return;
	}

	CTcpFactory *factory = CTcpFactory::Instance();

	int mode = atoi(argv[1]);

	if(mode == 0){
		int res = factory->CreateTcp(port);
		io << "Factory " << (res ? "failed to create" : "created") << " tcp server at port " << port << endl;
		if(res)
			io << "Error code: " << res << endl;

		CTcp *tcp = factory->GetTcpByPort(port);
		if(!tcp){
			io << "Cannot get tcp by port" << endl;
			return;
		}

		io << "Setup console: " << tcp->SetupConsole() << endl;
		io << "Handle client: " << tcp->AcceptAndHandle() << endl;
	} else if(mode == 1){
		CTcp *tcp = factory->GetTcpByPort(port);
		if(!tcp){
			io << "Cannot get tcp by port" << endl;
			return;
		}

		*tcp << "Bye, client on port " << port << endl;
		io << "Drop client: " << tcp->DropClient() << endl;
	} else if(mode == 2){
		CTcp *tcp = factory->GetTcpByPort(port);
		if(!tcp){
			io << "Cannot get tcp by port" << endl;
			return;
		}

		io << "Destroy client handler: " << tcp->DestroyClientHandler() << endl;
	}
}
