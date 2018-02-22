CC=gcc

SRCS = $(wildcard src/*.c)
OUT_MAIN=ring


CFLAGS = -std=c99 -Wall -Werror -D_POSIX_C_SOURCE=200112L -lpthread -pthread 

#	-Wextra -Wmissing-declarations\
# -Werror-implicit-function-declaration\
#	-Wreturn-type -Wparentheses -Wunused -Wold-style-definition\
#	-Wundef -Wshadow -Wstrict-prototypes -Wswitch-default\
#	-Wstrict-prototypes -Wunreachable-code -pthread

#-Wmissing-prototypes
#
all: ring.o



ring.o:  $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(OUT_MAIN) -g

clean:
	rm -f *.o *.o core
