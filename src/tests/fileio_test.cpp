#include "uart.h"
#include "fileio.h"

void fileio_test(void *arg){
	const char filename[] = "/fat/fileio_test.txt";

	CFile file = CFile(filename, FM_WRITE);

	if(!file.IsOpened()){
		uart << "Cannot open file :c" << endl;
		return;
	}

	for(int i = 0; i < 4096; i++){
		file << i << endl;
	}

	file.Close();
	file.Open(filename, FM_READ);

	int temp = 0;
	for(int i = 0; i < 4096; i++){
		file >> temp;
		if(i != temp){
			uart << "Read from file: " << temp << " expected: " << i << endl;
			uart << "Terminating test" << endl;
			break;
		}
	}

	file.Close();
}
