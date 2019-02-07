CC = gcc
CFLAGS += -O3 -std=gnu99 -march=native -Wall -Wextra \
	  -fno-exceptions -fno-strict-aliasing -fno-stack-protector \
	  -fomit-frame-pointer

ifeq ($(CC), clang)
	CFLAGS += -Wno-for-loop-analysis
else
	CFLAGS += -Wno-implicit-fallthrough
endif

ASMDIR = ./asm
BINDIR = ./bin
BLDDIR = ./build
SRCDIR = ./src

BIN = watermelon

SRCS = $(filter-out $(SRCDIR)/$(BIN).c,$(wildcard $(SRCDIR)/*.c))
ASML = $(patsubst $(SRCDIR)/%.c,$(ASMDIR)/%.S,$(SRCS))
DEPS = $(patsubst $(SRCDIR)/%.c,$(BLDDIR)/%.d,$(SRCS))
OBJS = $(patsubst $(SRCDIR)/%.c,$(BLDDIR)/%.o,$(SRCS))

all: $(BINDIR)/$(BIN) tests

debug: CFLAGS += -DDEBUG
debug: $(BINDIR)/$(BIN)

tree: CFLAGS += -DDEBUG -DTREE
tree: $(BINDIR)/$(BIN)

$(BINDIR)/$(BIN): $(SRCDIR)/$(BIN).c $(OBJS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

$(BLDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BLDDIR)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

TESTDIR = ./tests

TSRCDIR = $(TESTDIR)/$(SRCDIR)
TBLDDIR = $(TESTDIR)/$(BLDDIR)
TBINDIR = $(TESTDIR)/$(BINDIR)

TSRCS = $(wildcard $(TSRCDIR)/*.c)
TDEPS = $(patsubst $(TSRCDIR)/%.c,$(TBLDDIR)/%.d,$(TSRCS))
TESTS = $(patsubst $(TSRCDIR)/%.c,$(TBINDIR)/%,$(TSRCS))

tests: $(TESTS)

$(TBINDIR)/%: $(TSRCDIR)/%.c $(OBJS)
	@mkdir -p $(TBINDIR)
	@mkdir -p $(TBLDDIR)
	$(CC) $(CFLAGS) $^ -o $@

asm: $(ASML)

$(ASMDIR)/%.S: $(SRCDIR)/%.c
	@mkdir -p $(ASMDIR)
	$(CC) $(CFLAGS) -S $< -o $@

.PHONY: all debug tree tests asm clean

clean:
	@$(RM) $(BINDIR)/$(BIN) $(OBJS) $(DEPS)
	@$(RM) $(TESTS) $(TDEPS)

-include $(DEPS) $(TDEPS)
