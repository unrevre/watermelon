CC = gcc
CFLAGS += -O3 -std=gnu99 -march=native -Wall -Wextra \
	  -fno-exceptions -fno-strict-aliasing -fno-stack-protector \
	  -fomit-frame-pointer

ASMDIR = ./asm
BINDIR = ./bin
BLDDIR = ./build
SRCDIR = ./src

BIN = watermelon

SRCS = $(filter-out $(SRCDIR)/$(BIN).c,$(wildcard $(SRCDIR)/*.c))
ASML = $(patsubst $(SRCDIR)/%.c,$(ASMDIR)/%.S,$(SRCS))
DEPS = $(patsubst $(SRCDIR)/%.c,$(BLDDIR)/%.d,$(SRCS))
OBJS = $(patsubst $(SRCDIR)/%.c,$(BLDDIR)/%.o,$(SRCS))

$(BINDIR)/$(BIN): $(SRCDIR)/$(BIN).c $(OBJS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

$(BLDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BLDDIR)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

asm: $(ASML)

$(ASMDIR)/%.S: $(SRCDIR)/%.c
	@mkdir -p $(ASMDIR)
	$(CC) $(CFLAGS) -S $< -o $@

.PHONY: asm clean

clean:
	@$(RM) $(BINDIR)/$(BIN) $(OBJS) $(DEPS) $(ASML)

-include $(DEPS)
