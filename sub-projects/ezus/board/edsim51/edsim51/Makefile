# EdSim51 Example Makefile
# require sdcc version 3.0.0 and up

SDCC ?= sdcc
SDAS ?= asx8051
PACKIHX ?= packihx
ARCH ?= mcs51

CC := $(SDCC)
AS := $(SDAS)

TOP_DIR ?= ${CURDIR}/../../..
CORE_DIR ?= $(TOP_DIR)/core

SDCCCFLAGS = --model-small
SDCCCFLAGS += -m$(ARCH)
SDCCCFLAGS += --std-c89
SDCCCFLAGS += --Werror
SDCCCFLAGS += -I.
#SDCCCFLAGS += --nostdlib

ASLINKFLAGS = --iram-size 0x80
ASLINKFLAGS += --code-size 0x1000
ASLINKFLAGS += --code-loc 0x0000
ASLINKFLAGS += --stack-loc 0x30
#ASLINKFLAGS += --stack-after-data
#ASLINKFLAGS += --data-loc 0x30
ASLINKFLAGS += --out-fmt-ihx

OBJECTS = ezus.rel bb.rel bh.rel universe.rel

all: ezus.hex

ezus.hex: $(OBJECTS)
	$(CC) $(SDCCCFLAGS) $(ASLINKFLAGS) $^
	$(PACKIHX) ezus.ihx > ezus.hex

clean:
	rm -f *.asm
	rm -f *.hex *.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym

%.rel : %.c
	$(CC) $(SDCCCFLAGS) -c $<

