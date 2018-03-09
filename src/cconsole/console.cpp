#include "console.h"
#include "stddef.h"
#include "string.h"
#include "stdlib.h"

#include "util/util.h"
#include "util/auth/auth.h"

#include "uart.h"

CConsole console;

bool CConsole::HandleCmd(CIOBase &io, char *cmdstr){
	splited_string_t splited = string_split(cmdstr);

	char **argv = splited.data;
	int argc = splited.size;

	const char *name = argv[0]; // original cmd name

	CAuth *auth = CAuth::Instance();

	for(int i = 0; i < MAX_CMDS; i++){
		const char *cmdName = m_Cmds[i].name;

		if(!auth->IsAuthed() && strcmp(name, "auth")){
			io << "You are unauthed, auth first" << endl;
			release_split(splited);
			return true;
		}

		if(!strcmp(cmdName, name)){
			m_Cmds[i].Handle(io, argc, argv);

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

	if(!HandleCmd(io, str))
		io << "Unknown cmd: " << str << endl;

	free(str);
}

extern "C" int system(const char *cmd){
	return console.HandleCmd(uart, (char*)cmd) ? 0 : -1;
}
