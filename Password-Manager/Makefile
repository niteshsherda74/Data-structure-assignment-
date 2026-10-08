CC = gcc
CFLAGS = -Wall -Wextra -O2

all: pwmanager

pwmanager: main.c vault.c crypto.c vault.h crypto.h
	$(CC) $(CFLAGS) -o pwmanager main.c vault.c crypto.c

test_crypto: test_crypto.c crypto.c crypto.h
	$(CC) $(CFLAGS) -o test_crypto test_crypto.c crypto.c

clean:
	rm -f pwmanager pwmanager.exe test_crypto test_crypto.exe vault.pmv *.pmv.tmp

.PHONY: all clean test_crypto
