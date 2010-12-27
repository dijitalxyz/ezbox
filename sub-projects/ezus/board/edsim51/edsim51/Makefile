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

ASFLAGS := -plosgff

CFLAGS := --model-small
CFLAGS += -m$(ARCH)
CFLAGS += --std-c89
CFLAGS += --Werror
CFLAGS += -I.
CFLAGS += --nostdlib

# iram-size = 0x100 -0x30 = 0xD0
LDFLAGS := --iram-size 0xD0
LDFLAGS += --code-size 0x1000
LDFLAGS += --code-loc 0x0000
#LDFLAGS += --stack-loc 0x30
#LDFLAGS += --stack-after-data
#LDFLAGS += --stack-auto
LDFLAGS += --data-loc 0x30
LDFLAGS += --out-fmt-ihx

C_SOURCES := bh.c bb.c time.c universe.c world.c thread_context_switch.c init.c
C_SOURCES += _gptrget.c _moduint.c _modulong.c
ASM_SOURCES = ezus.s chaos.s #thread_context_switch.s

# user define thread routines
C_SOURCES += thread1.c

C_OBJECTS := $(patsubst %.c,%.rel,$(C_SOURCES))
C_ASM_OBJS := $(patsubst %.c,%.asm,$(C_SOURCES))

ASM_OBJECTS := $(patsubst %.s,%.rel,$(ASM_SOURCES))

all: ezus.hex

ezus.hex: $(ASM_OBJECTS) $(C_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^
	$(PACKIHX) ezus.ihx > ezus.hex

clean:
	rm -f $(C_ASM_OBJS)
	rm -f *.hex *.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym

%.rel : %.c
	$(CC) $(CFLAGS) -c $<

%.rel : %.s
	$(AS) $(ASFLAGS) $<
