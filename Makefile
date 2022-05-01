SRCDIR := src
BUILDDIR := build
OBJDIR := build
JINJADIR := $(BUILDDIR)/jinja
TESTDIR := $(BUILDDIR)/tests
SCRIPTDIR := scripts

SRCFILES := $(wildcard $(SRCDIR)/*.c)
OBJFILES := $(SRCFILES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DEPFILES := $(SRCFILES:$(SRCDIR)/%.c=$(OBJDIR)/%.d)
JINJACFILES := $(wildcard $(SRCDIR)/*.c.jinja)
JINJAHFILES := $(wildcard $(SRCDIR)/*.h.jinja)
JINJASRCFILES := $(JINJACFILES:$(SRCDIR)/%.c.jinja=$(JINJADIR)/%.c)
JINJAHDRFILES := $(JINJAHFILES:$(SRCDIR)/%.h.jinja=$(JINJADIR)/%.h)
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

$(OBJFILES): $(OBJDIR)/%.o: $(SRCDIR)/%.c $(JINJAHDRFILES) | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(JINJAOBJFILES): $(OBJDIR)/%.o: $(JINJADIR)/%.c $(JINJAHDRFILES) | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(JINJASRCFILES) $(JINJAHDRFILES): $(JINJADIR)/%: $(SRCDIR)/%.jinja | $(JINJADIR)
	python3 $(SCRIPTDIR)/jinja_render.py $< > $@

$(JINJADIR): | $(BUILDDIR)
	mkdir $(JINJADIR)

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean
clean:
	$(RM) -r $(BUILDDIR)

-include $(DEPFILES) $(JINJADEPFILES)
