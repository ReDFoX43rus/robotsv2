#ifndef UTIL_LUAHANDLER_LEAHANDLER_H
#define UTIL_LUAHANDLER_LEAHANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

int lua_handler_init(void);
int lua_handler_destroy(int handler_n);
int lua_setup_and_run(int id, const char* filename);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: UTIL_LUAHANDLER_LEAHANDLER_H */
