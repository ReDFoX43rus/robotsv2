#pragma once

#include <inttypes.h>

typedef enum{
	CMD_CLOSE_DOORS = 1,
	CMD_OPEN_DOORS,
	CMD_OPEN_WINDOWS,
	CMD_REQUEST_VALUE
} controller_cmds_t;

static uint8_t g_NrfAddr[5] = {040, 012, 033, 105, 234};