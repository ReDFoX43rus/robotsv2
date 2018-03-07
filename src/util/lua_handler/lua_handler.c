#include "lua_handler.h"
#include "lualib.h"
#include "lauxlib.h"

#define LUA_MAX_HANDLERS 4

static lua_State *apHandlers[LUA_MAX_HANDLERS] = {0};

int lua_handler_init(void)
{
	for (int i = 0; i < LUA_MAX_HANDLERS; i++)
	{
		if (apHandlers[i] == NULL)
		{
			apHandlers[i] = luaL_newstate();
			return i;
		}
	}

	return -1;
}

int lua_handler_destroy(int handler_n)
{
	if (handler_n < 0 || handler_n >= LUA_MAX_HANDLERS)
		return -1;

	if (apHandlers[handler_n] == NULL)
		return -2;

	lua_close(apHandlers[handler_n]);
	apHandlers[handler_n] = NULL;

	return 0;
}

static int lua_get_sensor_data(lua_State *state)
{
	int args = lua_gettop(state);
	if (args != 1)
		return 0;

	//TODO: Cматреть сюды!
}

int lua_setup_and_run(int id, const char* filename)
{
	if (id < 0 || id >= LUA_MAX_HANDLERS)
		return -1;

	lua_State *state = apHandlers[id];

	if (state == NULL)
		return -2;

	luaL_openlibs(state);

	int result = luaL_loadfile(state, filename);
	if (result != LUA_OK)
		return -3;

	result = lua_pcall(state, 0, LUA_MULTRET, 0);
	if (result != LUA_OK)
		return -4;

	return 0;
}
