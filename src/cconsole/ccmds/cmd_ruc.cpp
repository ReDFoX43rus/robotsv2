#include "iobase/iobase.h"
#include "string.h"

extern "C" void *ruc_import(void *filename);

extern "C" void test_ruc_threadsv2(void);
extern "C" void test_ruc_hello(void);

void CmdRucHandler(CIOBase &io, int argc, char **argv){
	if(argc < 2){
		io << "Usage: ruc filename" << endl;
		return;
	}

	if(!strcmp(argv[1], "th_test")){
		io << "Threads: hello test" << endl;
		test_ruc_hello();
		io << "Threads: full test" << endl;
		test_ruc_threadsv2();
	} else ruc_import(argv[1]);

}
