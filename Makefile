CC = clang
CFLAGS += -O3 -pthread -flto -std=gnu99 -march=native -Wall -Wextra \
	  -fno-exceptions -fno-strict-aliasing -fno-stack-protector \
	  -fomit-frame-pointer -fno-asynchronous-unwind-tables

ifneq (,$(findstring gcc,$(CC)))
	CFLAGS += -Wno-implicit-fallthrough
endif

ASMFLAGS := $(filter-out -flto,$(CFLAGS))

LIBS += -lncurses

MAKE = make

ASMDIR = ./asm
BINDIR = ./bin
BLDDIR = ./build
SRCDIR = ./src
TSTDIR = ./tests

MAIN = watermelon
BIN ?= $(MAIN)

SRCS = $(filter-out $(SRCDIR)/$(MAIN).c,$(wildcard $(SRCDIR)/*.c))
ASML = $(patsubst $(SRCDIR)/%.c,$(ASMDIR)/%.S,$(SRCS))
DEPS = $(patsubst $(SRCDIR)/%.c,$(BLDDIR)/%.d,$(SRCS))
OBJS = $(patsubst $(SRCDIR)/%.c,$(BLDDIR)/%.o,$(SRCS))

all: binary tests

debug: CFLAGS += -DDEBUG
debug: binary

tree: CFLAGS += -DDEBUG -DTREE
tree: binary

binary: mkdir $(BINDIR)/$(BIN)

$(BINDIR)/$(BIN): $(SRCDIR)/$(MAIN).c $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

$(BLDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

asm: mkdir $(ASML)

$(ASMDIR)/%.S: $(SRCDIR)/%.c
	$(CC) $(ASMFLAGS) -S $< -o $@

tests: $(OBJS)
	$(MAKE) -C $(TSTDIR)

mkdir:
	@mkdir -p $(BINDIR)
	@mkdir -p $(BLDDIR)
	@mkdir -p $(ASMDIR)

clean:
	$(RM) $(BINDIR)/* $(BLDDIR)/* $(ASMDIR)/*
	$(MAKE) -C $(TSTDIR) clean

.PHONY: all debug tree binary asm tests mkdir clean

-include $(DEPS) $(TDEPS)
