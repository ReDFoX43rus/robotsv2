#include "iobase/iobase.h"
#include "util/auth/auth.h"
#include "string.h"

void CmdAuthHandler(CIOBase &io, int argc, char *argv[]){
	if(argc < 2){
		io << "Usage: " << argv[0] << " password" << endl;
		return;
	}

	CAuth *auth = CAuth::Instance();

	if(argc == 3 && !strcmp(argv[1], "set")){
		char *newPwd = argv[2];
		io << (auth->StorePass(newPwd) == 0 ? "Ok" : "Failed") << endl;
		return;
	}

	int res = auth->TryAuth(argv[1]);
	io << "Auth " << (res == 0 ? "success" : "failed") << endl;
	if(res)
		io << "Error code: " << res << endl;
}
