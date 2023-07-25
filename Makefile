
TITLEID    := VGLTETRIS
TARGET	   := vitetris
TARGETVPK  := $(TARGET).vpk

VITA3K := $(shell command -v Vita3K)
VITA3K_DIR := $(shell dirname $(VITA3K) 2>/dev/null)
VITA3K_APP_DIR := $(VITA3K_DIR)/config/ux0/app/$(TITLEID)

PROJECT_ROOT := $(shell git rev-parse --show-toplevel)
MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
VITASDK_DIR := $(PROJECT_ROOT)/sdk
VITASDK_ENV_SCRIPT := $(shell find $(VITASDK_DIR) -maxdepth 1 -type f -executable)
TARGET_PLATFORM ?= Vita3K

ifndef VITASDK
$(error VITASDK is undefined in the environment. Please execute or source $(VITASDK_ENV_SCRIPT))
# $(shell source "$(VITASDK_ENV_SCRIPT)" "$(TARGET_PLATFORM)" && $(MAKE) $(MAKEFLAGS) $(MAKEOVERRIDES) $(MAKECMDGOALS))
endif

SOURCE_DIR := src
			
INCLUDES := include

LIBS := -lvitaGL \
		-lvitashark \
		-lc \
		-lm \
		-lmathneon \
		-ltaihen_stub \
		-lSceAppMgr_stub \
		-lSceCommonDialog_stub \
		-lSceCtrl_stub \
		-lSceDisplay_stub \
		-lSceGxm_stub \
		-lSceKernelDmacMgr_stub \
		-lSceShaccCg_stub \
		-lSceShaccCgExt

SOURCE_FILES := $(shell find $(SOURCE_DIR) -type f -name '*.c')
OBJECTS     := $(SOURCE_FILES:.c=.o)

FEATURE_FLAGS :=
ifneq ($(DEBUG),)
	FEATURE_FLAGS += -DDEBUG
endif

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
CFLAGS  = -g -Wl,-q -O2 -ftree-vectorize $(FEATURE_FLAGS)
CXXFLAGS  = $(CFLAGS) -fno-exceptions -std=gnu++11 -fpermissive
ASFLAGS = $(CFLAGS)

SHELL := /bin/bash

# .ONESHELL:

.PHONY: 3k

all: $(TARGETVPK)

debug:
	DEBUG=1 $(MAKE) $(TARGETVPK)

echo: 
	@echo "TITLEID = $(TITLEID)"
	@echo "TARGET = $(TARGET)"
	@echo "TARGETVPK = $(TARGETVPK)"
	@echo "VITA3K_DIR = $(VITA3K_DIR)"
	@echo "VITA3K_SDK_DIR = $(VITA3K_SDK_DIR)"
	@echo "VITA3K_APP_DIR = $(VITA3K_APP_DIR)"
	@echo "LIBS = $(LIBS)"
	@echo "SOURCE_FILES = $(SOURCE_FILES)"
	@echo "OBJECTS = $(OBJECTS)"
	@echo "PREFIX = $(PREFIX)"
	@echo "CC = $(CC)"
	@echo "CXX = $(CXX)"
	@echo "CFLAGS = $(CFLAGS)"
	@echo "CXXFLAGS = $(CXXFLAGS)"
	@echo "ASFLAGS = $(ASFLAGS)"


# %.o: %.c %.h Makefile
# 	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

%.velf: %.elf
	vita-elf-create $< $@
	
eboot.bin: $(TARGET).velf
	vita-make-fself $< eboot.bin	

$(TARGETVPK): eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLEID) "$(TARGET)" param.sfo
	vita-pack-vpk \
		-s param.sfo \
		-b eboot.bin \
		-a "$(SOURCE_DIR)/graphics/texture=texture" \
		-a "$(SOURCE_DIR)/graphics/shader=shader" \
		$@
	
clean:
	@rm -rfv *.velf *.elf *.vpk $(OBJECTS) param.sfo eboot.bin

3k: $(TARGETVPK) param.sfo eboot.bin
ifndef VITA3K
$(error No executable named Vita3K was found, please add the Vita3K install directory to $$PATH)
endif
	$(MAKE) $(TARGETVPK)
	if [ -d "$(VITA3K_APP_DIR)" ]; then \
		cp -v eboot.bin "$(VITA3K_APP_DIR)/" ;\
		cp -v param.sfo "$(VITA3K_APP_DIR)/sce_sys/" ;\
		cp -v -r "$(SOURCE_DIR)/shader" "$(SOURCE_DIR)/texture" "$(VITA3K_APP_DIR)/" ;\
		Vita3K -B Vulkan --installed-path $(TITLEID) ;\
	else \
		Vita3K -B Vulkan -- "$(MAKEFILE_DIR)$(TARGETVPK)" ;\
	fi

vita:
	TARGET_PLATFORM=vita $(MAKE) $(TARGET)