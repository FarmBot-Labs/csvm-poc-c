#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <stdio.h>

/** This is a marker that counts the number of `inbox` messages created for the
 * Lua VM */
int autoinc = 0;
/** A global variable (inside the Lua VM) that queues inbound (C -> Lua)
 * messages */
const char *INBOX = "inbox";

/** Appends a key/value pair (string:string) to the last `inbox` message. */
void inbox_string(lua_State *L, char *key, char *value)
{
  lua_getglobal(L, INBOX);
  lua_pushstring(L, key);
  lua_pushstring(L, value);
  lua_rawset(L, -3); /* Stores the pair in the table */
  lua_pop(L, 1);
}

/** Appends a key/value pair (string:integer) to the last `inbox` message. */
void inbox_int(lua_State *L, char *key, int value)
{
  lua_getglobal(L, INBOX);
  lua_pushstring(L, key);
  lua_pushinteger(L, value);
  lua_rawset(L, -3); /* Stores the pair in the table */
  lua_pop(L, 1);

}

/** Increase the `inbox` message count. */
void bump_autoinc(lua_State *L)
{
  inbox_int(L, "autoinc", autoinc++);
}

/** Add new entry to end of the `inbox` */
void add_message(lua_State *L,
                 int channel,
                 int namespace,
                 int operation,
                 char *payload)
{
  bump_autoinc(L);
  inbox_int(L, "channel", channel);
  inbox_int(L, "namespace", namespace);
  inbox_int(L, "operation", operation);
  inbox_string(L, "payload", payload);
}
int main(void)
{
  int status, result;
  lua_State *L;

  L = luaL_newstate();

  luaL_openlibs(L); /* Load Lua libraries */

  status = luaL_loadfile(L, "script.lua");
  if (status)
  {
    fprintf(stderr, "Couldn't load script.lua: %s\n", lua_tostring(L, -1));
    exit(1);
  }
  lua_newtable(L); /* We will pass a table */
  lua_setglobal(L, INBOX);

  add_message(L, 1, 2, 3, "FOUR");
  /* Ask Lua to run our little script */
  result = lua_pcall(L, 0, LUA_MULTRET, 0);
  if (result)
  {
    fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
    exit(1);
  }

  lua_close(L); /* Cya, Lua */

  return 0;
}
