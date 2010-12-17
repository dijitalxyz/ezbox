# EdSim51 Example Makefile
# require sdcc version 3.0.0 and up

EZCC ?= sdcc
EZAS ?= sdas8051
EZLD ?= sdld
PACKIHX ?= packihx
ARCH ?= mcs51

CC := $(EZCC)
AS := $(EZAS)
LD := $(EZLD)

TOP_DIR ?= ${CURDIR}/../../..
CORE_DIR ?= $(TOP_DIR)/core

CFLAGS = --model-small
CFLAGS += -m$(ARCH)
CFLAGS += --std-c89
CFLAGS += --Werror
CFLAGS += -I.
#CFLAGS += --nostdlib

LDFLAGS = --iram-size 0x80
LDFLAGS += --code-size 0x1000
LDFLAGS += --code-loc 0x0000
LDFLAGS += --stack-loc 0x30
#LDFLAGS += --stack-after-data
#LDFLAGS += --data-loc 0x30
LDFLAGS += --out-fmt-ihx

OBJECTS = ezus.rel bb.rel bh.rel universe.rel

all: ezus.hex

ezus.hex: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^
	$(PACKIHX) ezus.ihx > ezus.hex

clean:
	rm -f *.asm
	rm -f *.hex *.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym

%.rel : %.c
	$(CC) $(CFLAGS) -c $<

