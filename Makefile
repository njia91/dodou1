CC=gcc

SRCSTCP = $(wildcard src-tcp/*)
SRCSUDP = $(wildcard src-udp/*)

RINGHEADER = ringmain.h

OUT_MAINTCP=tcpNode
OUT_MAINUDP=udpNode



CFLAGS = -std=c99 -Wall -Werror -D_POSIX_C_SOURCE=200112L -lpthread -pthread 

#	-Wextra -Wmissing-declarations\
# -Werror-implicit-function-declaration\
#	-Wreturn-type -Wparentheses -Wunused -Wold-style-definition\
#	-Wundef -Wshadow -Wstrict-prototypes -Wswitch-default\
#	-Wstrict-prototypes -Wunreachable-code -pthread

#-Wmissing-prototypes
#
tcp: ringTcp.o

udp: ringUdp.o

ringUdp.o:  $(SRCSUDP) $(RINGHEADER)
	$(CC) $(CFLAGS) $(SRCSUDP) -I. -o $(OUT_MAINUDP) -g

ringTcp.o:  $(SRCSTCP)  $(RINGHEADER)
	$(CC) $(CFLAGS) $(RINGHEADER) $(SRCSTCP) -I. -o $(OUT_MAINTCP) -g

clean:
	rm -f *.o *.o core
