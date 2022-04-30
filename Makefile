SRCDIR := src
BUILDDIR := build
OBJDIR := build
JINJADIR := $(BUILDDIR)/jinja
TESTDIR := $(BUILDDIR)/tests
SCRIPTDIR := scripts

SRCFILES := $(wildcard $(SRCDIR)/*.c)
OBJFILES := $(SRCFILES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DEPFILES := $(SRCFILES:$(SRCDIR)/%.c=$(OBJDIR)/%.d)
JINJAFILES := $(wildcard $(SRCDIR)/*.jinja)
JINJASRCFILES := $($(wildcard $(SRCDIR)/*.c.jinja):$(SRCDIR)/*.c.jinja=$(JINJADIR)/%.c)
JINJAOBJFILES := $(JINJASRCFILES:$(JINJADIR)/%.c=$(OBJDIR)/%.o)
JINJADEPFILES := $(JINJASRCFILES:$(JINJADIR)/%.c=$(OBJDIR)/%.d)

CFLAGS += -O3
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -std=c99
CFLAGS += -MMD -MP
CFLAGS += -I$(SRCDIR) -I$(JINJADIR)

.PHONY: default
default: $(BUILDDIR)/libfix64.a

$(BUILDDIR)/libfix64.a: $(OBJFILES) $(JINJAOBJFILES) | $(BUILDDIR)
	$(AR) rcs $@ $^

$(OBJFILES): $(OBJDIR)/%.o: $(SRCDIR)/%.c jinjarender | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(JINJAOBJFILES): $(OBJDIR)/%.o: $(JINJADIR)/%.c jinjarender | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: jinjarender
jinjarender: $(JINJAFILES) | $(JINJADIR)
	python3 $(SCRIPTDIR)/jinja_render.py

$(BUILDDIR):
	mkdir $(BUILDDIR)

$(JINJADIR): | $(BUILDDIR)
	mkdir $(JINJADIR)

.PHONY: clean
clean:
	$(RM) -r $(BUILDDIR)

-include $(DEPFILES) $(JINJADEPFILES)
