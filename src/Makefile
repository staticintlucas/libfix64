BUILDDIR = build
OBJDIR = $(BUILDDIR)/obj
PREFIX = /usr/local
DESTDIR =

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(SOURCES:%.c=$(OBJDIR)/%.o)
TEST_OBJECTS = $(SOURCES:%.c=$(OBJDIR)/%_test.o)

CFLAGS += -O3 -fPIC
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -std=c99
CFLAGS += -I.
TEST_CFLAGS += -Og -fPIC
TEST_CFLAGS += -Wall -Wextra -Wpedantic -Werror
TEST_CFLAGS += -std=c99
TEST_CFLAGS += -I. -DFIX64_TEST
TEST_LDFLAGS += -shared

.PHONY: all
all: $(BUILDDIR)/libfix64.a

.PHONY: tests
tests: $(BUILDDIR)/libfix64_tests.so

.PHONY: install
install: all
	mkdir -p $(DESTDIR)$(PREFIX)/lib
	cp -f $(BUILDDIR)/libfix64.a $(DESTDIR)$(PREFIX)/lib
	mkdir -p $(DESTDIR)$(PREFIX)/include
	cp -f ./fix64.h $(DESTDIR)$(PREFIX)/include

$(BUILDDIR)/libfix64_tests.so: $(TEST_OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(TEST_LDFLAGS) $^ -o $@

$(BUILDDIR)/libfix64.a: $(OBJECTS)
	@mkdir -p $(@D)
	$(AR) rcs $@ $^

$(OBJECTS): $(OBJDIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_OBJECTS): $(OBJDIR)/%_test.o: %.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) -r $(BUILDDIR)
