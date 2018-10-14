CC = gcc
CFLAGS += -O3 -std=gnu99 -march=native -Wall -Wextra \
	  -fno-exceptions -fno-strict-aliasing -fno-stack-protector \
	  -fomit-frame-pointer

BIN = watermelon

BUILDDIR = ./build
ASMDIR = ./asm

SRCS = $(wildcard *.c)
DEPS = $(patsubst %.c,$(BUILDDIR)/%.d,$(SRCS))
OBJS = $(patsubst %.c,$(BUILDDIR)/%.o,$(SRCS))
ASM = $(patsubst %.c,$(ASMDIR)/%.S,$(SRCS))

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

asm: $(ASM)

$(ASMDIR)/%.S: %.c
	@mkdir -p $(ASMDIR)
	$(CC) $(CFLAGS) -S $< -o $@

.PHONY: clean

clean:
	$(RM) $(OBJS) $(DEPS) $(BIN) $(ASM)

-include $(DEPS)
