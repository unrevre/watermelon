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
	  -fno-asynchronous-unwind-tables \
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

all: CFLAGS += -fomit-frame-pointer -O3
all: mkdir objects binary tests

icc: CC = icc
icc: CFLAGS += -isysroot $(SDKROOT) -fomit-frame-pointer -O3
icc: mkdir objects binary

san: CFLAGS += -fsanitize=address,integer,undefined
san: mkdir objects binary

info: CFLAGS += -DINFO -fomit-frame-pointer -O3
info: mkdir objects binary

tree: CFLAGS += -DINFO -DTREE -fomit-frame-pointer -O3
tree: mkdir objects binary

debug: CFLAGS += -g -O1
debug: mkdir objects binary

objects: $(OBJS)

$(BLDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -pthread -MMD -c $< -o $@

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
	$(RM) -r $(BINDIR)/* $(BLDDIR)/* $(ASMDIR)/*
	$(MAKE) -C $(TSTDIR) clean

.PHONY: all icc san info tree debug objects binary asm tests iwyu mkdir clean

-include $(DEPS)
