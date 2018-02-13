#include "../../iobase/uart/uart.h"
#include "../../iobase/fileio/fileio.h"
#include "../../util/util.h"

#define RECEIVE_BUFF_SIZE 1024

void CmdFilereceiveHandler(int argc, char **argv){
	if(argc < 3){
		uart << "Usage: filereceive filename size" << endl;
		return;
	}

	char *filename = argv[1];
	char *size = argv[2];

	//uart << "Filename: " << filename << " size: " << size << endl;

	char *pEnd;
	size_t fileSize = strtol(size, &pEnd, 10);

	//uart << "Converted size: " << size << endl;

	CFile file = CFile(filename, FM_WRITE);
	if(!file.IsOpened()){
		uart << "Error: cannot open file" << endl;
		return;
	}

	char *buffer;
	bool isSwap = false;
	if((buffer = (char*)malloc(fileSize)) != NULL)
		isSwap = true;
	else buffer = (char*)malloc(RECEIVE_BUFF_SIZE);

	//uart << "Using swap: " << (isSwap ? "yes" : "no") << endl;
	uart << (int)isSwap << endl;

	size_t receivedBytes = 0;
	int uartBufDSize, currentDiff;
	while(receivedBytes < fileSize){
		uartBufDSize = uart.GetBufferedDataLength();
		if(uartBufDSize < 0){
			uart << "Error receiving file :C" << endl;
			file.Close();
			return;
		}

		if(isSwap){
			// this variable used to control overflow
			currentDiff = receivedBytes + uartBufDSize;
			receivedBytes += uart.GetBytes(buffer + receivedBytes, currentDiff < fileSize ? uartBufDSize : fileSize - receivedBytes);
		} else {
			currentDiff = fileSize - receivedBytes > RECEIVE_BUFF_SIZE ? RECEIVE_BUFF_SIZE : fileSize - receivedBytes;
			while(uartBufDSize < currentDiff)
				uartBufDSize = uart.GetBufferedDataLength();

			uart.GetBytes(buffer, currentDiff);
			file.Write(buffer, currentDiff);
			receivedBytes += currentDiff;
		}

	}

	if(isSwap)
		file.Write(buffer, fileSize);

	file.Close();

	free(buffer);
	uart << "Done!" << endl;
}
