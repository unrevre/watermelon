MAIN = watermelon
BIN ?= $(MAIN)

ASMDIR = ./asm
BINDIR = ./bin
BLDDIR = ./build
SRCDIR = ./src
SVCDIR = ./services
TSTDIR = ./tests
WMNDIR = ./watermelon

CC = clang
CFLAGS += -std=c99 -march=native -Wall -Wextra -flto \
	  -fno-exceptions -fno-strict-aliasing -fno-stack-protector \
	  -fomit-frame-pointer -fno-asynchronous-unwind-tables \
	  -D_POSIX_C_SOURCE=199309L

ifneq (,$(findstring gcc,$(CC)))
	CFLAGS += -Wno-implicit-fallthrough
endif

LIBS += -lncurses
LDFLAGS += $(LIBS)

UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
	LDFLAGS += -dead_strip
endif

SRCS = $(wildcard $(SRCDIR)/*.c)
ASML = $(patsubst $(SRCDIR)/%.c,$(ASMDIR)/%.S,$(SRCS))
DEPS = $(patsubst $(SRCDIR)/%.c,$(BLDDIR)/%.d,$(SRCS))
OBJS = $(patsubst $(SRCDIR)/%.c,$(BLDDIR)/%.o,$(SRCS))

SVCS = $(wildcard $(SVCDIR)/*.c)
DEPS += $(patsubst $(SVCDIR)/%.c,$(BLDDIR)/%.d,$(SVCS))
OBJS += $(patsubst $(SVCDIR)/%.c,$(BLDDIR)/%.o,$(SVCS))

all: mkdir objects binary tests

debug: CFLAGS += -DDEBUG
debug: mkdir objects binary

tree: CFLAGS += -DDEBUG -DTREE
tree: mkdir objects binary

objects: $(OBJS)

$(BLDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -O3 -pthread -MMD -c $< -o $@

$(BLDDIR)/%.o: $(SVCDIR)/%.c
	$(CC) $(CFLAGS) -Os -I$(SRCDIR) -MMD -c $< -o $@

binary: $(BINDIR)/$(BIN)

$(BINDIR)/$(BIN): $(WMNDIR)/$(MAIN).c $(OBJS)
	$(CC) $(CFLAGS) -pthread -O3 -I$(SRCDIR) -I$(SVCDIR) $^ $(LDFLAGS) -o $@

asm: CFLAGS :=$(filter-out -flto,$(CFLAGS))
asm: mkdir $(ASML)

$(ASMDIR)/%.S: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -O3 -S $< -o $@

MAKE = make

tests: $(OBJS)
	$(MAKE) -C $(TSTDIR)

iwyu:
	$(MAKE) clean
	$(MAKE) -k CC='include-what-you-use \
		-Xiwyu --no_fwd_decls \
		-Xiwyu --mapping_file=.watermelon.imp'

mkdir:
	@mkdir -p $(BINDIR)
	@mkdir -p $(BLDDIR)
	@mkdir -p $(ASMDIR)

clean:
	$(RM) $(BINDIR)/* $(BLDDIR)/* $(ASMDIR)/*
	$(MAKE) -C $(TSTDIR) clean

.PHONY: all debug tree objects binary asm tests iwyu mkdir clean

-include $(DEPS) $(TDEPS)
