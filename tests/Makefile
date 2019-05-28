CC = gcc
CFLAGS += -O3 -pthread -flto -std=gnu99 -march=native -Wall -Wextra \
	  -fno-exceptions -fno-strict-aliasing -fno-stack-protector \
	  -fomit-frame-pointer -fno-asynchronous-unwind-tables

ifneq (,$(findstring gcc,$(CC)))
	CFLAGS += -Wno-implicit-fallthrough
endif

LIBS += -lncurses

MAKE = make

BINDIR = ./bin
BLDDIR = ./build
SRCDIR = ./src

SRCS = $(wildcard $(SRCDIR)/*.c)
DEPS = $(patsubst $(SRCDIR)/%.c,$(BLDDIR)/%.d,$(SRCS))
TEST = $(patsubst $(SRCDIR)/%.c,$(BINDIR)/%,$(SRCS))

PDIR = ./../
PSRCDIR = $(PDIR)/$(SRCDIR)
PBLDDIR = $(PDIR)/$(BLDDIR)

MAIN = watermelon

PSRCS = $(filter-out $(PSRCDIR)/$(MAIN).c,$(wildcard $(PSRCDIR)/*.c))
POBJS = $(patsubst $(PSRCDIR)/%.c,$(PBLDDIR)/%.o,$(PSRCS))

tests: mkdir $(TEST)

$(BINDIR)/%: $(SRCDIR)/%.c $(POBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

mkdir:
	@mkdir -p $(BINDIR)
	@mkdir -p $(BLDDIR)

clean:
	$(RM) $(BINDIR)/* $(BLDDIR)/*

.PHONY: tests mkdir clean

-include $(DEPS)
