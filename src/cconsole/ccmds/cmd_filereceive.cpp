#include "iobase/iobase.h"
#include "iobase/fileio/fileio.h"
#include "util/util.h"

#define RECEIVE_BUFF_SIZE 1024

void CmdFilereceiveHandler(CIOBase &io, int argc, char **argv){
	if(argc < 3){
		io << "Usage: filereceive filename size" << endl;
		return;
	}

	char *filename = argv[1];
	char *size = argv[2];

	CFile debug = CFile("/fat/fr_debug", FM_WRITE);
	debug << "Filename: " << filename << " Size: " << size << endl;

	char *pEnd;
	size_t fileSize = strtol(size, &pEnd, 10);

	debug << "Converted size: " << size << endl;

	CFile file = CFile(filename, FM_WRITE);
	if(!file.IsOpened()){
		io << "Error: cannot open file" << endl;

		debug << "Error opening file" << endl;
		debug.Close();
		return;
	}

	char *buffer;
	bool isSwap = false;
	if((buffer = (char*)malloc(fileSize)) != NULL)
		isSwap = true;
	else buffer = (char*)malloc(RECEIVE_BUFF_SIZE);

	io << (int)isSwap << endl;
	debug << "Swap: " << (isSwap ? "yes" : "no");
	if(isSwap)
		debug << ", Allocated size: " << fileSize;
	debug << endl;

	size_t receivedBytes = 0;
	int ioBufDSize, currentDiff;
	while(receivedBytes < fileSize){
		ioBufDSize = io.GetBufferedDataLength();
		if(ioBufDSize < 0){
			io << "Error receiving file :C" << endl;
			file.Close();

			debug << "Error: Negative buffered data length" << endl;
			debug.Close();
			return;
		}

		if(isSwap){
			// this variable used to control overflow
			currentDiff = receivedBytes + ioBufDSize;
			receivedBytes += io.GetBytes(buffer + receivedBytes, currentDiff < fileSize ? ioBufDSize : fileSize - receivedBytes);
		} else {
			currentDiff = fileSize - receivedBytes > RECEIVE_BUFF_SIZE ? RECEIVE_BUFF_SIZE : fileSize - receivedBytes;
			while(ioBufDSize < currentDiff)
				ioBufDSize = io.GetBufferedDataLength();

			io.GetBytes(buffer, currentDiff);
			file.Write(buffer, currentDiff);
			receivedBytes += currentDiff;
		}

		debug << "Received bytes: " << receivedBytes << " Total: " << fileSize << endl;
	}

	if(isSwap)
		file.Write(buffer, fileSize);

	file.Close();

	free(buffer);
	io << "Done!" << endl;

	debug << "Done!" << endl;
	debug.Close();
}
