#include <erl_nif.h>

#include <stdio.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <lualib.h>

static ERL_NIF_TERM function(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
  char buff[256];
  int error;

  lua_State *L;

  L = luaL_newstate();

  // while (fgets(buff, sizeof(buff), stdin) != NULL) {
  //   error = luaL_loadbuffer(L, buff, strlen(buff), "line") ||
  //           lua_pcall(L, 0, 0, 0);
  //   if (error) {
  //     fprintf(stderr, "%s", lua_tostring(L, -1));
  //     lua_pop(L, 1);  /* pop error message from the stack */
  //   }
  // }

  lua_close(L);
  return enif_make_atom(env, "ok");
}

static ErlNifFunc nif_funcs[] = {
  {"function", 0, function},
};

ERL_NIF_INIT(Elixir.NifTest, nif_funcs, NULL, NULL, NULL, NULL)
