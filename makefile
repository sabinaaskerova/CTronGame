LDFLAGS = -lncurses
CFLAGS = -Wall -g

all: client serveur

client: client.c client.o common.h
		gcc $(CFLAGS) $< $(LDFLAGS) -o $@
 
serveur: serveur.c serveur.o common.h
		gcc $(CFLAGS) $< -o $@