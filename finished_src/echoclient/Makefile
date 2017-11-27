CC=gcc
CFLAGS=-Wall -Werror -I ../include
DEPS = ../include/unp.h
OBJ = echoclient.o
DEFS = \
	-DWC_RSA_BLINDING

echoclient: ../lib/*.c tcpcli01.c $(DEPS)
	$(CC) $(CFLAGS) $(DEFS) -o echoclient ../lib/*.c tcpcli01.c -I ../include -lm -lwolfssl

.PHONY: clean

clean:
	rm -f *.o echoclient
