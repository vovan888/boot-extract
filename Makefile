
CFLAGS=-Wall -arch i386
PROGS=boot-extract bootloader-extract

all: $(PROGS)

boot-extract: boot-extract.c

bootloader-extract: bootloader-extract.c

clean:
	rm -f *.o
	rm -f $(PROGS)

