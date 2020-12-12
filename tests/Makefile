BINDIR = ./bin
BLDDIR = ./build
SRCDIR = ./src
SVCDIR = ./services

PDIR = ./../
PBLDDIR = $(PDIR)/$(BLDDIR)
PSRCDIR = $(PDIR)/$(SRCDIR)
PSVCDIR = $(PDIR)/$(SVCDIR)

CC = gcc
CFLAGS += -std=c99 -march=native -Wall -Wextra -flto \
	  -fno-exceptions -fno-strict-aliasing -fno-stack-protector \
	  -fomit-frame-pointer -fno-asynchronous-unwind-tables
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
BINS = $(patsubst $(SRCDIR)/%.c,$(BINDIR)/%,$(SRCS))
DEPS = $(patsubst $(SRCDIR)/%.c,$(BLDDIR)/%.d,$(SRCS))

OBJS = $(wildcard $(PBLDDIR)/*.o)

all: mkdir $(BINS)

info: CFLAGS += -DINFO
info: all

tree: CFLAGS += -DINFO -DTREE
tree: all

$(BINDIR)/%: $(SRCDIR)/%.c $(OBJS)
	$(CC) $(CFLAGS) -pthread -O3 -I$(PSRCDIR) -I$(PSVCDIR) $^ $(LDFLAGS) -o $@

MAKE = make

iwyu:
	$(MAKE) clean
	$(MAKE) -k CC='include-what-you-use \
		-Xiwyu --no_fwd_decls \
		-Xiwyu --mapping_file=./../.watermelon.imp'

mkdir:
	@mkdir -p $(BINDIR)
	@mkdir -p $(BLDDIR)

clean:
	$(RM) -r $(BINDIR)/* $(BLDDIR)/*

.PHONY: all info tree iwyu mkdir clean

-include $(DEPS)
