SRCDIR  := src
OBJDIR  := build
TESTDIR := $(objdir)/tests

SRCFILES := $(wildcard $(SRCDIR)/*.c)
OBJFILES := $(SRCFILES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DEPFILES := $(SRCFILES:$(SRCDIR)/%.c=$(OBJDIR)/%.d)

CFLAGS += -O3
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -std=c99
CFLAGS += -MMD -MP

.PHONY: default
default: $(OBJDIR)/libfix64.a

$(OBJDIR)/libfix64.a: $(OBJFILES) | $(OBJDIR)
	$(AR) rcs $@ $^

$(OBJFILES): $(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir $(OBJDIR)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)

-include $(DEPFILES)
