#ifndef UTIL_LUAHANDLER_LEAHANDLER_H
#define UTIL_LUAHANDLER_LEAHANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Allocates lua state if any free ones exist
 * Returns descriptor on success, -1 on fail */
int lua_handler_init(void);

/* Destroys lua state by allocated descriptor
 * Returns 0 on success */
int lua_handler_destroy(int handler_n);

/* Make lua state with 'id' descriptor run 'filename' script */
int lua_setup_and_run(int id, const char* filename);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: UTIL_LUAHANDLER_LEAHANDLER_H */
