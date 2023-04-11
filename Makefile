
TITLEID    := VGLGLCUBE
TARGET	   := glescube
TARGETVPK  := $(TARGET).vpk
VITA3K_DIR := "$(HOME)/Projects/vita/3K"
VITA3K_SDK_DIR := "$(VITA3K_DIR)/sdk"
VITA3K_INSTALL_DIR := "$(VITA3K_DIR)/config/ux0/app/$(TITLEID)"

SOURCES := .
			
INCLUDES := include

LIBS := -lvitaGL \
		-lc \
		-lSceCommonDialog_stub \
		-lm \
		-lSceGxm_stub \
		-lSceDisplay_stub \
		-lSceAppMgr_stub \
		-lmathneon \
		-lvitashark \
		-lSceCtrl_stub \
		-lSceShaccCgExt \
		-ltaihen_stub \
		-lSceShaccCg_stub \
		-lSceKernelDmacMgr_stub

CFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.c))
CPPFILES := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.cpp))
BINFILES := $(foreach dir,$(DATA), $(wildcard $(dir)/*.bin))
OBJS     := $(addsuffix .o,$(BINFILES)) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) 

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
CFLAGS  = -g -Wl,-q -O2 -ftree-vectorize
CXXFLAGS  = $(CFLAGS) -fno-exceptions -std=gnu++11 -fpermissive
ASFLAGS = $(CFLAGS)

SHELL := /bin/bash

.PHONY: 3k

all: $(TARGETVPK)

$(TARGETVPK): eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLEID) "$(TARGET)" param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin \
		$(shell for f in *.cg *.pgm; do echo -n "-a $$f=$$f "; done) $@ 

eboot.bin: $(TARGET).velf
	vita-make-fself $< eboot.bin	
	
%.velf: %.elf
	vita-elf-create $< $@
	
$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@
	
clean:
	@rm -rf *.velf *.elf *.vpk $(OBJS) param.sfo eboot.bin

3k: $(TARGETVPK) param.sfo eboot.bin
	builtin source "$(VITA3K_SDK_DIR)/vita-env"
	$(MAKE) $(TARGETVPK)
	if [ -d "$(VITA3K_INSTALL_DIR)" ]; then \
		cp -v eboot.bin "$(VITA3K_INSTALL_DIR)/" ;\
		cp -v param.sfo "$(VITA3K_INSTALL_DIR)/sce_sys/" ;\
		cp -v *.cg *.pgm "$(VITA3K_INSTALL_DIR)/" ;\
		cd "$(VITA3K_DIR)" ;\
		./Vita3K -B Vulkan --installed-path $(TITLEID) ;\
	else \
		cd "$(VITA3K_DIR)" ;\
		./Vita3K -B Vulkan  "$(CURDIR)$(TARGETVPK)" ;\
	fi	
