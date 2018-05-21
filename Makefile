ifeq ($(ERL_INCLUDE_DIR),)
$(warning ERL_INCLUDE_DIR not set. Invoke via mix)
else
# Set Erlang-specific compile and linker flags
ERL_CFLAGS ?= -I$(ERL_INCLUDE_DIR)
ERL_LDFLAGS ?= -L$(ERL_LIBDIR)
endif

CJSON_SRC=c_src/cJSON/cJSON.c

SRC=$(wildcard c_src/*.c) $(CJSON_SRC)
OBJ=$(SRC:.c=.o)


LDFLAGS ?=
CFLAGS ?= -Wall

PORT := priv/csvm

.PHONY: all

all: priv priv/csvm

%.o: %.c
	$(CC) -g -c $(CFLAGS) $(ERL_CFLAGS) -o $@ $<

priv/csvm: $(OBJ)
	$(CC) $^ $(LDFLAGS) $(ERL_LDFLAGS) -o $@

priv:
	mkdir -p priv

clean:
	$(RM) c_src/*.o
	$(RM) priv/csvm
