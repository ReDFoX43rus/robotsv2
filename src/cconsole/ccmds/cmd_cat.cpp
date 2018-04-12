#include "iobase/iobase.h"
#include "iobase/fileio/fileio.h"
#include "util/util.h"

#define CAT_BUFF_SIZE 128

void CmdCatHandler(CIOBase &io, int argc, char **argv){
	if(argc < 2){
		io << "Usage: cat filename" << endl;
		return;
	}

	char *filename = argv[1];

	CFile file = CFile(filename, FM_READ);
	if(!file.IsOpened()){
		io << "Cannot open file: " << filename << endl;
		return;
	}

	char buffer[CAT_BUFF_SIZE];
	size_t size = file.GetSize();
	size_t readBytes = 0;

	size_t currentDiff;
	while(readBytes < size){
		currentDiff = size - readBytes > CAT_BUFF_SIZE ? CAT_BUFF_SIZE : size - readBytes;

		file.GetString(buffer, currentDiff);
		io.Write(buffer, currentDiff);

		readBytes += currentDiff;
	}

	file.Close();
	io << endl << "File size: " << size << endl;
	io << endl;
}
