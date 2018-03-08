#include "lua_handler.h"
#include "sensors/handler_c.h"
#include "stdlib.h"

#include "lualib.h"
#include "lauxlib.h"

#define LUA_MAX_HANDLERS 4

static lua_State *apHandlers[LUA_MAX_HANDLERS] = {0};

typedef struct {
	int error;
	int data;
} lua_chk_t;

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

static int lua_check_count_args(lua_State *state)
{
	int args = lua_gettop(state);
	if (args < 2)
		return -1;

	return args;
}

static lua_chk_t lua_check_and_get_value(lua_State *state, int index)
{
	lua_chk_t check = { 0, 0 };
	if (!lua_isnumber(state, index))
		check.error = 1;
	else
		check.data = (int)lua_tonumber(state, index);

	return check;
}

static int lua_get_count_sensor_args(int id)
{
	switch (id) {
		case SENSOR_COLOR_RED:
		case SENSOR_COLOR_GREEN:
		case SENSOR_COLOR_BLUE:
		case SENSOR_COMPASS_X:
		case SENSOR_COMPASS_Y:
		case SENSOR_COMPASS_Z:
		case SENSOR_COMPASS_ANGLE:
		case SENSOR_LINE_AN:
		case SENSOR_FLAME_AN:
		case SENSOR_SHARP:
		case SENSOR_SOUND:
		case SENSOR_CRASH_BUTTON:
		case SENSOR_HCSR04:
			return 2;
		case SENSOR_LINE_DIG:
		case SENSOR_FLAME_DIG:
		default:
			return 0;
	}
}

static int* lua_get_sensor_pins(lua_State *state, int count)
{
	int *pins = (int*)malloc(sizeof(int)*count);

	for (int i = 2; i <= count + 1; i++)
	{
		lua_chk_t check = lua_check_and_get_value(state, i);
		if (check.error)
		{
			free(pins);
			return NULL;
		}

		pins[i - 2] = check.data;
	}

	return pins;
}

static int lua_get_sensor_data(lua_State *state)
{
	int c = 0;

	int argc = lua_check_count_args(state);
	if (argc == -1)
		return 0;

	lua_chk_t id = lua_check_and_get_value(state, 1);
	if (id.error)
		return 0;

	int count = lua_get_count_sensor_args(id.data);
	if (count < 1)
		return 0;

	if (argc - count != 1)
		return 0;

	int *pins = lua_get_sensor_pins(state, count);
	if (!pins)
		return 0;

	int result = handle_sensor(id.data, pins);
	free(pins);

	lua_pushnumber(state, (lua_Number)result);
	return 1;
}

int lua_setup_and_run(int id, const char* filename)
{
	if (id < 0 || id >= LUA_MAX_HANDLERS)
		return -1;

	lua_State *state = apHandlers[id];

	if (state == NULL)
		return -2;

	luaL_openlibs(state);
	lua_register(state, "get_sensor_data", lua_get_sensor_data);

	int result = luaL_loadfile(state, filename);
	if (result != LUA_OK)
		return -3;

	result = lua_pcall(state, 0, LUA_MULTRET, 0);
	if (result != LUA_OK)
		return -4;

	return 0;
}
