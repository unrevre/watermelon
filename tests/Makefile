BINDIR = ./bin
BLDDIR = ./build
SRCDIR = ./src
SVCDIR = ./services

PDIR = ./../
PBLDDIR = $(PDIR)/$(BLDDIR)
PSRCDIR = $(PDIR)/$(SRCDIR)
PSVCDIR = $(PDIR)/$(SVCDIR)

CC = gcc
CFLAGS += -std=c99 -march=native -Wall -Wextra \
	  -fno-exceptions -fno-strict-aliasing -fno-stack-protector \
	  -fomit-frame-pointer -fno-asynchronous-unwind-tables
	  -D_POSIX_C_SOURCE=199309L

ifneq (,$(findstring gcc,$(CC)))
	CFLAGS += -Wno-implicit-fallthrough
endif

LIBS += -lncurses

SRCS = $(wildcard $(SRCDIR)/*.c)
BINS = $(patsubst $(SRCDIR)/%.c,$(BINDIR)/%,$(SRCS))
DEPS = $(patsubst $(SRCDIR)/%.c,$(BLDDIR)/%.d,$(SRCS))

OBJS = $(wildcard $(PBLDDIR)/*.o)

tests: mkdir $(BINS)

$(BINDIR)/%: $(SRCDIR)/%.c $(OBJS)
	$(CC) $(CFLAGS) -O3 -pthread -I$(PSRCDIR) -I$(PSVCDIR) $^ $(LIBS) -o $@

mkdir:
	@mkdir -p $(BINDIR)
	@mkdir -p $(BLDDIR)

clean:
	$(RM) $(BINDIR)/* $(BLDDIR)/*

.PHONY: tests mkdir clean

-include $(DEPS)
