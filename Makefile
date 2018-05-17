ifeq ($(ERL_INCLUDE_DIR),)
$(warning ERL_INCLUDE_DIR not set. Invoke via mix)
endif

# Set Erlang-specific compile and linker flags
ERL_CFLAGS ?= -I$(ERL_INCLUDE_DIR)
ERL_LDFLAGS ?= -L$(ERL_LIBDIR)

LDFLAGS ?=
CFLAGS ?=

PORT := priv/csvm

.PHONY: all

all: priv priv/csvm

%.o: %.c
	$(CC) -c $(CFLAGS) $(ERL_CFLAGS) -o $@ $<

priv/csvm: c_src/csvm.o c_src/cJSON.c
	$(CC) $^ $(LDFLAGS) $(ERL_LDFLAGS) -o $@

priv:
	mkdir -p priv

clean:
	$(RM) c_src/*.o
	$(RM) priv/csvm
