CC = gcc
CFLAGS += -O3 -flto -std=gnu99 -march=native -Wall -Wextra \
	  -fno-exceptions -fno-strict-aliasing -fno-stack-protector \
	  -fomit-frame-pointer -fno-asynchronous-unwind-tables

ifeq ($(CC), gcc)
	CFLAGS += -Wno-implicit-fallthrough
endif

LIBS += -lncurses

MAKE = make

BINDIR = ./bin
BLDDIR = ./build
SRCDIR = ./src

BIN = watermelon

SRCS = $(wildcard $(SRCDIR)/*.c)
DEPS = $(patsubst $(SRCDIR)/%.c,$(BLDDIR)/%.d,$(SRCS))
TEST = $(patsubst $(SRCDIR)/%.c,$(BINDIR)/%,$(SRCS))

MAIN = $(filter-out ../$(SRCDIR)/$(BIN).c,$(wildcard ../$(SRCDIR)/*.c))
OBJS = $(patsubst ../$(SRCDIR)/%.c,../$(BLDDIR)/%.o,$(MAIN))

tests: mkdir $(TEST)

$(BINDIR)/%: $(SRCDIR)/%.c $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

mkdir:
	@mkdir -p $(BINDIR)
	@mkdir -p $(BLDDIR)

clean:
	$(RM) $(BINDIR)/* $(BLDDIR)/*

.PHONY: tests mkdir clean

-include $(DEPS)
