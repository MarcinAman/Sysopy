
CC=gcc
CFLAGS=-Wall -std=gnu99

.PHONY: clean

all: creator barber

creator: creator.c
	$(CC) $< -o $@ $(CFLAGS)

barber: barber.c
	$(CC) $< -o $@ $(CFLAGS)

clean:
	rm {creator,barber} 2> /dev/null

