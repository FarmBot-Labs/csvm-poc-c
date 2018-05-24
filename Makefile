ifeq ($(ERL_INCLUDE_DIR),)
$(warning ERL_INCLUDE_DIR not set. Invoke via mix)
else
# Set Erlang-specific compile and linker flags
ERL_CFLAGS ?= -I$(ERL_INCLUDE_DIR)
ERL_LDFLAGS ?= -L$(ERL_LIBDIR)
endif

BUILD_DIR := $(PWD)/_build

LUA_VERSION := 5.3.4
MAJOR_VER=5
LUA_NAME := lua-$(LUA_VERSION)
LUA_DL := $(LUA_NAME).tar.gz
LUA_DL_URL := "https://www.lua.org/ftp/$(LUA_DL)"
LUA_SRC_DIR := $(PWD)/c_src/$(LUA_NAME)/src
LUA_BUILD_DIR := $(BUILD_DIR)/$(LUA_NAME)

LUA_INCLUDE_DIR := $(LUA_BUILD_DIR)/include
LUA_LIBDIR := $(LUA_BUILD_DIR)/lib

LUA_CFLAGS := -I$(LUA_INCLUDE_DIR)
LUA_LDFLAGS := -L$(LUA_LIBDIR) -llua

LUA_LIB := $(LUA_LIBDIR)/liblua.so

SRC=$(wildcard c_src/*.c) $(CJSON_SRC)
OBJ=$(SRC:.c=.o)

LDFLAGS ?= -pthread
CFLAGS ?= -Wall -std=gnu99

ifdef DEBUG
	CFLAGS += -g -DDEBUG
endif

PORT := priv/csvm

.PHONY: all lua_clean lua_fullclean

all: priv $(LUA_LIB) priv/csvm

$(LUA_SRC_DIR):
	wget $(LUA_DL_URL)
	tar xf $(LUA_DL)
	$(RM) $(LUA_DL)
	mv $(LUA_NAME) c_src/
	cd c_src/$(LUA_NAME) && patch -p1 -i ../../lua.patch

$(LUA_BUILD_DIR):
	mkdir -p $(LUA_BUILD_DIR)

$(LUA_LIB): $(LUA_SRC_DIR) $(LUA_BUILD_DIR)
	cd c_src/$(LUA_NAME) && make MYCFLAGS="$(CFLAGS) -fPIC -DLUA_COMPAT_5_2 -DLUA_COMPAT_5_1" MYLDFLAGS="$(LDFLAGS)" linux
	cd c_src/$(LUA_NAME) && make -e TO_LIB="liblua.a liblua.so liblua.so.$(LUA_VERSION)" INSTALL_DATA='cp -d' INSTALL_TOP=$(LUA_BUILD_DIR) install

%.o: %.c
	$(CC) -c $(CFLAGS) $(ERL_CFLAGS) $(LUA_CFLAGS) -o $@ $<

$(PORT): $(OBJ)
	$(CC) $^ $(LDFLAGS) $(ERL_LDFLAGS) $(LUA_LDFLAGS) -o $@

priv/nif_test.so: c_src/nif_test/nif_test.c
	$(CC) -o $@ $< $(LUA_CFLAGS) $(ERL_CFLAGS) $(CFLAGS) $(NIF_CFLAGS) $(LDFLAGS) $(ERL_LDFLAGS) $(LUA_LDFLAGS) $(NIF_LDFLAGS)

priv:
	mkdir -p priv

clean:
	$(RM) c_src/*.o
	$(RM) priv/csvm

lua_clean:
	cd $(LUA_SRC_DIR) && make clean

lua_fullclean: lua_clean
	$(RM) -r c_src/$(LUA_NAME)
	$(RM) -r $(LUA_BUILD_DIR)
