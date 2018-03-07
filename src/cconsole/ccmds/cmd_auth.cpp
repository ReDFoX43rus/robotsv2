#include "iobase/iobase.h"
#include "util/auth/auth.h"

void CmdAuthHandler(CIOBase &io, int argc, char *argv[]){
	if(argc < 2){
		io << "Usage: " << argv[0] << " password" << endl;
		return;
	}

	CAuth *auth = CAuth::Instance();

	io << "Auth " << (auth->TryAuth(argv[1]) == 0 ? "success" : "failed") << endl;
}
