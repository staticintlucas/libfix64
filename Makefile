SRCDIR = src
SCRIPTDIR = scripts
BUILDDIR = build
OBJDIR = $(BUILDDIR)/obj
GENDIR = $(BUILDDIR)/src
PREFIX = /usr/local
DESTDIR =

JINJA_SOURCES = $(wildcard $(SRCDIR)/*.c.jinja)
JINJA_HEADERS = $(wildcard $(SRCDIR)/*.h.jinja)
SOURCES = $(JINJA_SOURCES:$(SRCDIR)/%.jinja=$(GENDIR)/%)
HEADERS = $(JINJA_HEADERS:$(SRCDIR)/%.jinja=$(GENDIR)/%)
OBJECTS = $(SOURCES:$(GENDIR)%.c=$(OBJDIR)%.o)
DEPFILES = $(OBJECTS:%.o=%.d)

CFLAGS += -O3
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -std=c99
CFLAGS += -I$(GENDIR)

.PHONY: all
all: $(BUILDDIR)/libfix64.a

.PHONY: install
install: all
	mkdir -p $(DESTDIR)$(PREFIX)/lib
	cp -f $(BUILDDIR)/libfix64.a $(DESTDIR)$(PREFIX)/lib
	mkdir -p $(DESTDIR)$(PREFIX)/include
	cp -f $(GENDIR)/fix64.h $(DESTDIR)$(PREFIX)/include

$(BUILDDIR)/libfix64.a: $(OBJECTS)
	@mkdir -p $(@D)
	$(AR) rcs $@ $^

$(OBJECTS): $(OBJDIR)/%.o: $(GENDIR)/%.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(SOURCES) $(HEADERS): $(GENDIR)/%: $(SRCDIR)/%.jinja $(SCRIPTDIR)/jinja_render.py
	@mkdir -p $(@D)
	python3 $(SCRIPTDIR)/jinja_render.py $< > $@~
	mv -f $@~ $@

.PHONY: clean
clean:
	$(RM) -r $(BUILDDIR)
