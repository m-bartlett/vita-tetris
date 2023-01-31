# ROOT_DIR := $(notdir $(patsubst %/,%,$(CURDIR)))
MAKEFILE := $(firstword $(MAKEFILE_LIST))
SHELL := /bin/bash

TARGET := ttytris

LIBS     :=
PKG_LIBS :=

CC        := gcc
CFLAGS    := $(shell pkg-config --cflags $(PKG_LIBS)) -std=c11
LIB_FLAGS := $(shell pkg-config --libs $(PKG_LIBS))
LIB_FLAGS += $(addprefix -l, $(LIBS))

PREFIX    ?= /usr/local
BINPREFIX := $(PREFIX)/bin

SOURCES   := $(wildcard *.c)
HEADERS   := $(wildcard *.h)
OBJECTS   := $(SOURCES:.c=.o)


all:	# Multi-threaded make by default
	$(MAKE) -j $(shell nproc) $(TARGET)

debug: CFLAGS += -D DEBUG
debug: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIB_FLAGS) $^ -o $@ -O3

$(OBJECTS): $(SOURCES) $(HEADERS) $(ICON_HEADERS)

%.o: %.c
	$(CC) $(FEATURES) $(CFLAGS) $(LDFLAGS) $(LIB_FLAGS) -c $< -o $@

$(SOURCES): $(MAKEFILE) # If Makefile changes, recompile
	@touch $(SOURCES)


install: $(TARGET)
	install -m 755 -D --target-directory "$(BINPREFIX)" "$(TARGET)"

uninstall:
	rm -f "$(BINPREFIX)/$(TARGET)"

clean:
	rm -f $(TARGET) $(OBJECTS) $(ICON_HEADERS)
	rm -rf icon/

.PHONY: debug default uninstall clean icons