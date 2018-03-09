#include "iobase/iobase.h"
#include "util/lua_handler/lua_handler.h"

void CmdLuaHandler(CIOBase &io, int argc, char *argv[]){
	int lua_id = lua_handler_init();

	io << "Lua id " << lua_id << endl;
	if (lua_id < 0)
		return;

	int res = lua_setup_and_run(lua_id, argv[1]);
	if (res < 0)
	{
		io << res << endl;
		return;
	}

	res = lua_handler_destroy(lua_id);
	io << "lua_handler_destroy " << res << endl;
}
