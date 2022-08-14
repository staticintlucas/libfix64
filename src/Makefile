BUILDDIR = build
OBJDIR = $(BUILDDIR)/obj
PREFIX = /usr/local
DESTDIR =

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(SOURCES:%.c=$(OBJDIR)/%.o)
DEPFILES = $(OBJECTS:%.o=%.d)

CFLAGS += -O3
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -std=c99
CFLAGS += -I.

.PHONY: all
all: $(BUILDDIR)/libfix64.a

.PHONY: install
install: all
	mkdir -p $(DESTDIR)$(PREFIX)/lib
	cp -f $(BUILDDIR)/libfix64.a $(DESTDIR)$(PREFIX)/lib
	mkdir -p $(DESTDIR)$(PREFIX)/include
	cp -f ./fix64.h $(DESTDIR)$(PREFIX)/include

$(BUILDDIR)/libfix64.a: $(OBJECTS)
	@mkdir -p $(@D)
	$(AR) rcs $@ $^

$(OBJECTS): $(OBJDIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) -r $(BUILDDIR)
