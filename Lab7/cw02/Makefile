CC=gcc
CFLAGS=-Wall -std=gnu99 -lpthread -lrt

.PHONY: clean

all: creator barber

clients: creator.c
	$(CC) $< -o $@ $(CFLAGS)

barber: barber.c
	$(CC) $< -o $@ $(CFLAGS)

clean:
	rm {creator,barber} 2> /dev/null
