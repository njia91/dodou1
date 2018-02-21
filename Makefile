CC=gcc

SRCS = $(wildcard src/*.c)
OUT_MAIN=mfind


CFLAGS = -std=c11 -Wall -Wextra -Wmissing-declarations\
	-Wmissing-prototypes -Werror-implicit-function-declaration\
	-Wreturn-type -Wparentheses -Wunused -Wold-style-definition\
	-Wundef -Wshadow -Wstrict-prototypes -Wswitch-default\
	-Wstrict-prototypes -Wunreachable-code -pthread


all: ring.o



ring.o: ringmain.c fqdnReader.c server.c
	$(CC) $(CFLAGS) $(SRCS) -o $(OUT_MAIN) -g

clean:
	rm -f *.o *.o core
