#include "iobase/iobase.h"
#include "cconsole/console.h"
#include "fileio.h"

#define EXEC_STRING_SIZE 256
void CmdExecHandler(CIOBase &io, int argc, char *argv[]){
	if(argc < 2){
		io << "Usage: " << argv[0] << " file_to_execute" << endl;
		return;
	}

	const char *filename = argv[1];

	CFile file = CFile(filename, FM_READ);
	if(!file.IsOpened()){
		io << "Error opening file" << endl;
		return;
	}

	char string[EXEC_STRING_SIZE];
	uint32_t pointer = 0;
	char c;
	uint32_t size = file.GetSize();

	for(int i = 0; i < size; i++){
		c = file.GetChar();
		string[pointer++] = c;

		if(c == '\n'){
			string[--pointer] = '\0';
			pointer = 0;

			console.HandleCmd(io, string);
		}
	}

	file.Close();
}
