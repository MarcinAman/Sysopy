all: creator barber

creator: creator.c
	gcc -Wall -std=gnu99 -lpthread -lrt creator.c -o creator

barber: barber.c
	gcc barber.c -o barber -Wall -std=gnu99 -lpthread -lrt

clean:
	rm -f barber creator
