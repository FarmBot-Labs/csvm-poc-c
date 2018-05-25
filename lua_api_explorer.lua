apidemo = require 'apidemo'
apidemo.setup_globals()

-- CREATE A GLOBAL "inbox" variable
INBOX = "inbox"
L = luaL_newstate()
lua_newtable(L)
lua_setglobal(L, INBOX)

-- Add a K/V pair called "IS_INBOX" so I don't get lost in the stack.
print("______")
lua_getglobal(L, INBOX)
lua_pushstring(L, "IS_INBOX");
lua_pushstring(L, "Yes, it is.");
lua_rawset(L, -3)
print("======")
-- Add a nested table to INBOX global
-- SEE: http://lua-users.org/lists/lua-l/2009-10/msg00420.html
lua_pushstring(L, "nested_table");
lua_newtable(L)
lua_rawset(L, -3)

-- print("You can try C API stuff interactively here.")
