TARGET_NAME = libbuffer
PREFIX ?= /usr
LIBBUFFER_CFLAGS = -g -Wall -Wextra -Werror -fPIC -Iinclude
LIBS = -lm
SONAME_VERSION = 0

STATIC_LIB=$(TARGET_NAME).a

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LIBS += -lrt
	DYNAMIC_LIB=$(TARGET_NAME).so.$(SONAME_VERSION)
	DYNAMIC_LIB_LINK=$(TARGET_NAME).so
	SONAME=-Wl,-soname,$(DYNAMIC_LIB)
endif
ifeq ($(UNAME_S),Darwin)
	DYNAMIC_LIB=$(TARGET_NAME).$(SONAME_VERSION).dylib
	DYNAMIC_LIB_LINK=$(TARGET_NAME).dylib
	SONAME=-Wl,-install_name,$(TARGET_NAME).$(SONAME_VERSION).dylib
endif

.PHONY: default all clean test

default: $(DYNAMIC_LIB) $(STATIC_LIB)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard src/*.c))
HEADERS = $(wildcard include/**/*.h include/*.h)

%.o: %.c $(HEADERS) Makefile
	$(CC) $(LIBBUFFER_CFLAGS) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(DYNAMIC_LIB) $(STATIC_LIB) $(OBJECTS)

$(DYNAMIC_LIB): $(OBJECTS)
	$(CC) -shared $(SONAME) $(OBJECTS) -Wall $(LIBS) -o $@
	ln -sf $(DYNAMIC_LIB) $(DYNAMIC_LIB_LINK)

$(STATIC_LIB): $(OBJECTS)
	rm -f $@
	$(AR) r $@ $^

test: $(DYNAMIC_LIB)
	make -C test
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):. ./test/clar_test

clean:
	-rm -f src/*.o
	-rm -f *.a *.dylib *.so *.so.*
	-rm -f failbot-report
	-rm -rf *.dSYM
	-make -C test clean

install: $(DYNAMIC_LIB)
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/bin
	mkdir -p $(PREFIX)/include/github
	install -c -m 644 $(DYNAMIC_LIB) $(PREFIX)/lib/$(DYNAMIC_LIB)
	cd $(PREFIX)/lib && ln -sf $(DYNAMIC_LIB) $(DYNAMIC_LIB_LINK)
	install -c -m 644 include/github/buffer.h $(PREFIX)/include/github/buffer.h
	install -c -m 644 include/github/buffer_ext.h $(PREFIX)/include/github/buffer_ext.h

uninstall:
	rm $(PREFIX)/lib/{$(DYNAMIC_LIB),$(DYNAMIC_LIB_LINK)}
