# EdSim51 Example Makefile

CC ?= sdcc
PACKIHX ?= packihx

SDCCCFLAGS = --model-small --std-c89

#ASLINKFLAGS = --code-loc 0x2000 --data-loc 0x30 --xram-loc 0x6000
ASLINKFLAGS = --iram-size 0x80

all: ledPatterns.hex

ledPatterns.hex: ledPatterns.c
	$(CC) $(SDCCCFLAGS) $(ASLINKFLAGS) $^
	$(PACKIHX) ledPatterns.ihx > ledPatterns.hex

clean:
	rm -f ledPatterns.asm
	rm -f *.hex *.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym

%.rel : %.c %.h
	$(CC) $(SDCCCFLAGS) -c $<

