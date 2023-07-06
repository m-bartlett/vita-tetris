
TITLEID    := VGLGLCUBE
TARGET	   := vitetris
TARGETVPK  := $(TARGET).vpk
VITA3K_DIR := "$(HOME)/Projects/vita/3K"
VITA3K_SDK_DIR := "$(VITA3K_DIR)/sdk"
VITA3K_INSTALL_DIR := "$(VITA3K_DIR)/config/ux0/app/$(TITLEID)"

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

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
CFLAGS  = -g -Wl,-q -O2 -ftree-vectorize
CXXFLAGS  = $(CFLAGS) -fno-exceptions -std=gnu++11 -fpermissive
ASFLAGS = $(CFLAGS)

SHELL := /bin/bash

.PHONY: 3k

all: $(TARGETVPK)


echo:
	@echo SOURCE_FILES: $(SOURCE_FILES)
	@echo OBJECTS: $(OBJECTS)


$(TARGETVPK): eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLEID) "$(TARGET)" param.sfo
	vita-pack-vpk \
		-s param.sfo \
		-b eboot.bin \
		-a "$(SOURCE_DIR)/graphics/texture=texture" \
		-a "$(SOURCE_DIR)/graphics/shader=shader" \
		$@

# 		$(shell for f in *.cg *.bmp; do echo -n "-a $$f=$$f "; done) $@ 

eboot.bin: $(TARGET).velf
	vita-make-fself $< eboot.bin	
	
%.velf: %.elf
	vita-elf-create $< $@
	
$(TARGET).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@
	
clean:
	@rm -rfv *.velf *.elf *.vpk $(OBJECTS) param.sfo eboot.bin

3k: $(TARGETVPK) param.sfo eboot.bin
	builtin source "$(VITA3K_SDK_DIR)/vita-env"
	$(MAKE) $(TARGETVPK)
	if [ -d "$(VITA3K_INSTALL_DIR)" ]; then \
		cp -v eboot.bin "$(VITA3K_INSTALL_DIR)/" ;\
		cp -v param.sfo "$(VITA3K_INSTALL_DIR)/sce_sys/" ;\
		cp -v -r shader texture "$(VITA3K_INSTALL_DIR)/" ;\
		cd "$(VITA3K_DIR)" ;\
		./Vita3K -B Vulkan --installed-path $(TITLEID) ;\
	else \
		cd "$(VITA3K_DIR)" ;\
		./Vita3K -B Vulkan  "$(CURDIR)$(TARGETVPK)" ;\
	fi	
