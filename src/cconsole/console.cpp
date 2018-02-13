#include "console.h"
#include "stddef.h"
#include "string.h"
#include "stdlib.h"

#include "util/util.h"

bool CConsole::HandleCmd(char *cmdstr){
	splited_string_t splited = string_split(cmdstr);

	char **argv = splited.data;
	int argc = splited.size;

	const char *name = argv[0]; // original cmd name

	for(int i = 0; i < MAX_CMDS; i++){
		const char *cmdName = m_Cmds[i].name;
		if(!strcmp(cmdName, name)){
			m_Cmds[i].Handle(argc, argv);

			release_split(splited);
			return true;
		}
	}

	release_split(splited);
	return false;
}


void CConsole::WaitForCmd(CIOBase &io){
	io << "esp32> ";
	char *str = io.GetString();
	io << endl;

	if(!HandleCmd(str))
		io << "Unknown cmd: " << str << endl;

	free(str);
}
