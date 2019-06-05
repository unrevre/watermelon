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
CFLAGS += -std=c99 -march=native -Wall -Wextra \
	  -fno-exceptions -fno-strict-aliasing -fno-stack-protector \
	  -fomit-frame-pointer -fno-asynchronous-unwind-tables \
	  -D_POSIX_C_SOURCE=199309L

ifneq (,$(findstring gcc,$(CC)))
	CFLAGS += -Wno-implicit-fallthrough
endif

LIBS += -lncurses

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
	$(CC) $(CFLAGS) -O3 -pthread -flto -MMD -c $< -o $@

$(BLDDIR)/%.o: $(SVCDIR)/%.c
	$(CC) $(CFLAGS) -Os -I$(SRCDIR) -MMD -c $< -o $@

binary: $(BINDIR)/$(BIN)

$(BINDIR)/$(BIN): $(WMNDIR)/$(MAIN).c $(OBJS)
	$(CC) $(CFLAGS) -O3 -pthread -I$(SRCDIR) -I$(SVCDIR) $^ $(LIBS) -o $@

asm: mkdir $(ASML)

$(ASMDIR)/%.S: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -O3 -S $< -o $@

MAKE = make

tests: $(OBJS)
	$(MAKE) -C $(TSTDIR)

mkdir:
	@mkdir -p $(BINDIR)
	@mkdir -p $(BLDDIR)
	@mkdir -p $(ASMDIR)

clean:
	$(RM) $(BINDIR)/* $(BLDDIR)/* $(ASMDIR)/*
	$(MAKE) -C $(TSTDIR) clean

.PHONY: all debug tree objects binary asm tests mkdir clean

-include $(DEPS) $(TDEPS)
