CC=gcc
CFLAGS=-Wall
LDFLAGS=

TARGET=hg522c-cfg hg522c-upload

all: $(TARGET)

hg522c-cfg: hg522c-cfg.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

hg522c-upload: hg522c-upload.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -f *.o $(TARGET)
