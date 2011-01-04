CC=gcc
CFLAGS=-Wall
LDFLAGS=

MAKE=make
CP=cp -f

TARGET=hg522c-cfg hg522c-upload hg522c-bootpd

all: $(TARGET)

hg522c-cfg: hg522c-cfg.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

hg522c-upload: hg522c-upload.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

hg522c-bootpd:
	$(MAKE) CC=$(CC) -C bootp
	$(CP) bootp/bootpd $@

clean:
	rm -f *.o $(TARGET)
	$(MAKE) CC=$(CC) -C bootp clean
