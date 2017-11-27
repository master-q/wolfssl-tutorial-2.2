CC=gcc
CFLAGS=-Wall -Werror -I ../include
DEPS = ../include/unp.h
OBJ = echoserver.o
DEFS = \
	-DWC_RSA_BLINDING

echoserver: ../lib/*.c tcpserv04.c $(DEPS)
	$(CC) $(CFLAGS) $(DEFS) -o echoserver ../lib/*.c tcpserv04.c -I ../include -lm -lwolfssl

.PHONY: clean

clean:
	rm -f *.o echoserver
